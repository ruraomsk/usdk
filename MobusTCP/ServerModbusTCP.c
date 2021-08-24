/*
 * ServerModbusTCP.c
 *
 *  Created on: Aug 24, 2021
 *      Author: rura
 */

#include "ServerModbusTCP.h"
#include "DebugLogger.h"
#include "sockets.h"

static fd_set refset;
static int fdmax;
static char bufferServerModbus[128];
void ServerModbusTCPLoop() {
	modbus_t *ctx = modbus_new_tcp("0.0.0.0", 502);
	uint8_t *query = (uint8_t*) malloc(MODBUS_TCP_MAX_ADU_LENGTH);

	modbus_set_debug(ctx, TRUE);

	modbus_mapping_t *mb_mapping = modbus_mapping_new(10, 16, 12, 13);
	if (mb_mapping == NULL) {
		Debug_Message(LOG_FATAL, "Failed to allocate the mapping");
		modbus_free(ctx);
		return;
	}

	int listen_socket = modbus_tcp_listen(ctx, 2);
	if (listen_socket == -1) {
		Debug_Message(LOG_FATAL, "server modbus not listen ");
		modbus_mapping_free(mb_mapping);
		modbus_free(ctx);
		return;
	}
	FD_ZERO(&refset);
	FD_SET(listen_socket, &refset);
	fdmax = listen_socket;
	for (;;) {
		struct timeval timeout = { 0, 1 };
		int result = select(fdmax + 1, &refset, NULL, NULL, &timeout);
		if (result == 0) {
			osDelay(100);
			continue;
		}
		for (int master_socket = 0; master_socket <= fdmax; master_socket++) {
			if (!FD_ISSET(master_socket, &refset))
				continue;
			if (master_socket == listen_socket) {
				//new connect
				Debug_Message(LOG_DEBUG, "New connection for modbus server");
				socklen_t addrlen;
				struct sockaddr_in clientaddr;
				int newfd;
				addrlen = sizeof(clientaddr);
				memset(&clientaddr, 0, sizeof(clientaddr));
				newfd = accept(listen_socket, (struct sockaddr* ) &clientaddr,
						&addrlen);
				if (newfd == -1) {
					Debug_Message(LOG_ERROR, "Server accept() error");
				} else {
					FD_SET(newfd, &refset);

					if (newfd > fdmax) {
						/* Keep track of the maximum */
						fdmax = newfd;
					}
					sprintf(bufferServerModbus,
							"New connection from %s:%d on socket %d\n",
							inet_ntoa(clientaddr.sin_addr),
							(int) clientaddr.sin_port, newfd);
					Debug_Message(LOG_DEBUG, bufferServerModbus);
				}

			} else {
				modbus_set_socket(ctx, master_socket);
				int rc = modbus_receive(ctx, query);
				if (rc > 0) {
					modbus_reply(ctx, query, rc, mb_mapping);
				} else if (rc == -1) {
					Debug_Message(LOG_DEBUG, "Connection closed");
					close(master_socket);
					FD_CLR(master_socket, &refset);
					if (master_socket == fdmax)
						fdmax--;
				}
			}
		}
	}
	close(listen_socket);
	modbus_mapping_free(mb_mapping);
	free(query);
	modbus_free(ctx);
}
