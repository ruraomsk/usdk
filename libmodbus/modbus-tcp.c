/*
 * Copyright © 2001-2013 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "lwip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

# include <sockets.h>
//# include <sys/ioctl.h>


//# include <netinet/in.h>
//# include <netinet/ip.h>
//# include <netinet/tcp.h>
//# include <arpa/inet.h>
//# include <netdb.h>


#include "modbus.h"
#include "modbus-private.h"

#include "modbus-tcp.h"
#include "modbus-tcp-private.h"


static int _modbus_set_slave(modbus_t *ctx, int slave)
{
    /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
    if (slave >= 0 && slave <= 247) {
        ctx->slave = slave;
    } else if (slave == MODBUS_TCP_SLAVE) {
        /* The special value MODBUS_TCP_SLAVE (0xFF) can be used in TCP mode to
         * restore the default value. */
        ctx->slave = slave;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

/* Builds a TCP request header */
static int _modbus_tcp_build_request_basis(modbus_t *ctx, int function,
                                           int addr, int nb,
                                           uint8_t *req)
{
    modbus_tcp_t *ctx_tcp = ctx->backend_data;

    /* Increase transaction ID */
    if (ctx_tcp->t_id < UINT16_MAX)
        ctx_tcp->t_id++;
    else
        ctx_tcp->t_id = 0;
    req[0] = ctx_tcp->t_id >> 8;
    req[1] = ctx_tcp->t_id & 0x00ff;

    /* Protocol Modbus */
    req[2] = 0;
    req[3] = 0;

    /* Length will be defined later by set_req_length_tcp at offsets 4
       and 5 */

    req[6] = ctx->slave;
    req[7] = function;
    req[8] = addr >> 8;
    req[9] = addr & 0x00ff;
    req[10] = nb >> 8;
    req[11] = nb & 0x00ff;

    return _MODBUS_TCP_PRESET_REQ_LENGTH;
}

/* Builds a TCP response header */
static int _modbus_tcp_build_response_basis(sft_t *sft, uint8_t *rsp)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation
       Guide V1.0b (page 23/46):
       The transaction identifier is used to associate the future
       response with the request. */
    rsp[0] = sft->t_id >> 8;
    rsp[1] = sft->t_id & 0x00ff;

    /* Protocol Modbus */
    rsp[2] = 0;
    rsp[3] = 0;

    /* Length will be set later by send_msg (4 and 5) */

    /* The slave ID is copied from the indication */
    rsp[6] = sft->slave;
    rsp[7] = sft->function;

    return _MODBUS_TCP_PRESET_RSP_LENGTH;
}


static int _modbus_tcp_prepare_response_tid(const uint8_t *req, int *req_length)
{
    return (req[0] << 8) + req[1];
}

static int _modbus_tcp_send_msg_pre(uint8_t *req, int req_length)
{
    /* Subtract the header length to the message length */
    int mbap_length = req_length - 6;

    req[4] = mbap_length >> 8;
    req[5] = mbap_length & 0x00FF;

    return req_length;
}

static ssize_t _modbus_tcp_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
    /* MSG_NOSIGNAL
       Requests not to send SIGPIPE on errors on stream oriented
       sockets when the other end breaks the connection.  The EPIPE
       error is still returned. */
    return send(ctx->s, (const char *)req, req_length, MSG_NOSIGNAL);
}

static int _modbus_tcp_receive(modbus_t *ctx, uint8_t *req) {
    return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

static ssize_t _modbus_tcp_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length) {
    return recv(ctx->s, (char *)rsp, rsp_length, 0);
}

static int _modbus_tcp_check_integrity(modbus_t *ctx, uint8_t *msg, const int msg_length)
{
    return msg_length;
}

static int _modbus_tcp_pre_check_confirmation(modbus_t *ctx, const uint8_t *req,
                                              const uint8_t *rsp, int rsp_length)
{
    /* Check transaction ID */
    if (req[0] != rsp[0] || req[1] != rsp[1]) {
        if (ctx->debug) {
			Debug_Message(LOG_ERROR, "Invalid transaction ID received 0x%X (not 0x%X)",
                            (rsp[0] << 8) + rsp[1], (req[0] << 8) + req[1]);
        }
        errno = EMBBADDATA;
        return -1;
    }

    /* Check protocol ID */
    if (rsp[2] != 0x0 && rsp[3] != 0x0) {
        if (ctx->debug) {
        	        Debug_Message(LOG_ERROR, "Invalid protocol ID received 0x%X (not 0x0)",
                            (rsp[2] << 8) + rsp[3]);
        }
        errno = EMBBADDATA;
        return -1;
    }

    return 0;
}

static int _modbus_tcp_set_ipv4_options(int s)
{
    int rc;
    int option;

    /* Set the TCP no delay flag */
    /* SOL_TCP = IPPROTO_TCP */
    option = 1;
    rc = setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
                    (const void *)&option, sizeof(int));
    if (rc == -1) {
        return -1;
    }

    /* If the OS does not offer SOCK_NONBLOCK, fall back to setting FIONBIO to
     * make sockets non-blocking */
    /* Do not care about the return value, this is optional */
    option = 1;
    ioctl(s, FIONBIO, &option);

    /**
     * Cygwin defines IPTOS_LOWDELAY but can't handle that flag so it's
     * necessary to workaround that problem.
     **/
    /* Set the IP low delay option */
    option = IPTOS_LOWDELAY;
    rc = setsockopt(s, IPPROTO_IP, IP_TOS,
                    (const void *)&option, sizeof(int));
    if (rc == -1) {
        return -1;
    }

    return 0;
}

static int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen,
                    const struct timeval *ro_tv)
{
    int rc = connect(sockfd, addr, addrlen);

    if (rc == -1 && errno == EINPROGRESS) {
        fd_set wset;
        int optval;
        socklen_t optlen = sizeof(optval);
        struct timeval tv = *ro_tv;

        /* Wait to be available in writing */
        FD_ZERO(&wset);
        FD_SET(sockfd, &wset);
        rc = select(sockfd + 1, NULL, &wset, NULL, &tv);
        if (rc <= 0) {
            /* Timeout or fail */
            return -1;
        }

        /* The connection is established if SO_ERROR and optval are set to 0 */
        rc = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *)&optval, &optlen);
        if (rc == 0 && optval == 0) {
            return 0;
        } else {
            errno = ECONNREFUSED;
            return -1;
        }
    }
    return rc;
}

/* Establishes a modbus TCP connection with a Modbus server. */
static int _modbus_tcp_connect(modbus_t *ctx)
{
    int rc;
    /* Specialized version of sockaddr for Internet socket address (same size) */
    struct sockaddr_in addr;
    modbus_tcp_t *ctx_tcp = ctx->backend_data;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ctx_tcp->port);
//    addr.sin_addr.s_addr = inet_addr(ctx_tcp->ip);
//    inet_aton("192.168.115.159",&addr.sin_addr.s_addr);
    inet_aton(ctx_tcp->ip,&addr.sin_addr.s_addr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        if (ctx->debug) {
        	        Debug_Message(LOG_ERROR, "not sockets");
        }
        return -1;
    }
    lwip_setsockopt(sock, SOL_SOCKET,SO_RCVTIMEO, &ctx->response_timeout, sizeof(struct timeval));
    lwip_setsockopt(sock, SOL_SOCKET,SO_SNDTIMEO, &ctx->response_timeout, sizeof(struct timeval));
//    lwip_setsockopt(sock, SOL_SOCKET,SO_CONTIMEO, &ctx->response_timeout, sizeof(struct timeval));
    ctx->s=sock;
    if (ctx->debug) {
    	        Debug_Message(LOG_INFO, "Connecting to %s:%d socket:%d", ctx_tcp->ip, ctx_tcp->port,ctx->s);
    }
    rc = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (rc == -1) {
    	if(ctx->debug){
        	Debug_Message(LOG_ERROR, "Нет соединения с сервером %s:%d sock:%d errno:%d",ctx_tcp->ip, ctx_tcp->port,ctx->s,errno,sock);
    	}
    	shutdown(ctx->s,0);
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }
    return 0;
}


/* Closes the network connection and socket in TCP mode */
static void _modbus_tcp_close(modbus_t *ctx)
{
    if (ctx->s != -1) {
        shutdown(ctx->s, SHUT_RDWR);
        close(ctx->s);
        ctx->s = -1;
    }
}

static int _modbus_tcp_flush(modbus_t *ctx)
{
    int rc;
    int rc_sum = 0;

    do {
        /* Extract the garbage from the socket */
        char devnull[MODBUS_TCP_MAX_ADU_LENGTH];
        rc = recv(ctx->s, devnull, MODBUS_TCP_MAX_ADU_LENGTH, MSG_DONTWAIT);
        if (rc > 0) {
            rc_sum += rc;
        }
    } while (rc == MODBUS_TCP_MAX_ADU_LENGTH);

    return rc_sum;
}

/* Listens for any request from one or many modbus masters in TCP */
int modbus_tcp_listen(modbus_t *ctx, int nb_connection)
{
    int new_s;
//    int enable;
    int flags;
    struct sockaddr_in addr;
    modbus_tcp_t *ctx_tcp;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    ctx_tcp = ctx->backend_data;


    flags = SOCK_STREAM;


    new_s = socket(AF_INET, flags, 0);
    if (new_s == -1) {
        return -1;
    }

//    enable = 1;
//    if (lwip_setsockopt(new_s, SOL_SOCKET, SO_REUSEADDR,
//                   (char *)&enable, sizeof(enable)) == -1) {
//        close(new_s);
//        return -1;
//    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* If the modbus port is < to 1024, we need the setuid root. */
    addr.sin_port = htons(ctx_tcp->port);
    if (ctx_tcp->ip[0] == '0') {
        /* Listen any addresses */
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    	        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        /* Listen only specified IP address */
        addr.sin_addr.s_addr = inet_addr(ctx_tcp->ip);
    }
    if (bind(new_s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(new_s);
        return -1;
    }

    if (listen(new_s, nb_connection) == -1) {
        close(new_s);
        return -1;
    }

    return new_s;
}


int modbus_tcp_accept(modbus_t *ctx, int *s)
{
    struct sockaddr_in addr;
    socklen_t addrlen;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    addrlen = sizeof(addr);
    ctx->s = accept(*s, (struct sockaddr *)&addr, &addrlen);

    if (ctx->s == -1) {
        return -1;
    }

    if (ctx->debug) {
    	        Debug_Message(LOG_INFO, "The client connection from %s is accepted",
     	               inet_ntoa(addr.sin_addr));

    }

    return ctx->s;
}


static int _modbus_tcp_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read)
{
    int s_rc;
    while ((s_rc = select(ctx->s+1, rset, NULL, NULL, tv)) == -1) {
        if (errno == EINTR) {
            if (ctx->debug) {
            	        Debug_Message(LOG_INFO, "A non blocked signal was caught");
            }
            /* Necessary after an error */
            FD_ZERO(rset);
            FD_SET(ctx->s, rset);
        } else {
            return -1;
        }
    }

    if (s_rc == 0) {
        errno = ETIMEDOUT;
        return -1;
    }

    return s_rc;
}

static void _modbus_tcp_free(modbus_t *ctx) {
    free(ctx->backend_data);
    free(ctx);
}

const modbus_backend_t _modbus_tcp_backend = {
    _MODBUS_BACKEND_TYPE_TCP,
    _MODBUS_TCP_HEADER_LENGTH,
    _MODBUS_TCP_CHECKSUM_LENGTH,
    MODBUS_TCP_MAX_ADU_LENGTH,
    _modbus_set_slave,
    _modbus_tcp_build_request_basis,
    _modbus_tcp_build_response_basis,
    _modbus_tcp_prepare_response_tid,
    _modbus_tcp_send_msg_pre,
    _modbus_tcp_send,
    _modbus_tcp_receive,
    _modbus_tcp_recv,
    _modbus_tcp_check_integrity,
    _modbus_tcp_pre_check_confirmation,
    _modbus_tcp_connect,
    _modbus_tcp_close,
    _modbus_tcp_flush,
    _modbus_tcp_select,
    _modbus_tcp_free
};


//const modbus_backend_t _modbus_tcp_pi_backend = {
//    _MODBUS_BACKEND_TYPE_TCP,
//    _MODBUS_TCP_HEADER_LENGTH,
//    _MODBUS_TCP_CHECKSUM_LENGTH,
//    MODBUS_TCP_MAX_ADU_LENGTH,
//    _modbus_set_slave,
//    _modbus_tcp_build_request_basis,
//    _modbus_tcp_build_response_basis,
//    _modbus_tcp_prepare_response_tid,
//    _modbus_tcp_send_msg_pre,
//    _modbus_tcp_send,
//    _modbus_tcp_receive,
//    _modbus_tcp_recv,
//    _modbus_tcp_check_integrity,
//    _modbus_tcp_pre_check_confirmation,
//    _modbus_tcp_close,
//    _modbus_tcp_flush,
//    _modbus_tcp_select,
//    _modbus_tcp_free
//};

modbus_t* modbus_new_tcp(const char *ip, int port)
{
    modbus_t *ctx;
    modbus_tcp_t *ctx_tcp;
    size_t dest_size;
    size_t ret_size;


    ctx = (modbus_t *)malloc(sizeof(modbus_t));
    if (ctx == NULL) {
        return NULL;
    }
    _modbus_init_common(ctx);

    /* Could be changed after to reach a remote serial Modbus device */
    ctx->slave = MODBUS_TCP_SLAVE;

    ctx->backend = &_modbus_tcp_backend;

    ctx->backend_data = (modbus_tcp_t *)malloc(sizeof(modbus_tcp_t));
    if (ctx->backend_data == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }
    ctx_tcp = (modbus_tcp_t *)ctx->backend_data;

    if (ip != NULL) {
        dest_size = sizeof(char) * 16;
        ret_size = strlcpy(ctx_tcp->ip, ip, dest_size);
        if (ret_size == 0) {
            Debug_Message(LOG_ERROR, "The IP string is empty");
            modbus_free(ctx);
            errno = EINVAL;
            return NULL;
        }

        if (ret_size >= dest_size) {
        	Debug_Message(LOG_ERROR, "The IP string has been truncated");
            modbus_free(ctx);
            errno = EINVAL;
            return NULL;
        }
    } else {
        ctx_tcp->ip[0] = '0';
    }
    ctx_tcp->port = port;
    ctx_tcp->t_id = 0;

    return ctx;
}


