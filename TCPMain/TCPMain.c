/*
 * TCPMain.c
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */

#include "lwip.h"
#include "TCPMain.h"
#include "DebugLogger.h"
#include "parson.h"

struct {
	ip_addr_t ipAddr;
	int adr1, adr2, adr3, adr4;
	unsigned int port;
	unsigned int timeout;
} TCPMainSetup;
char TCPMainBuffer[MAX_LEN_TCP_MESSAGE];
char *ptrMainBuffer=TCPMainBuffer;

struct tcp_pcb *main_pcb;
int ReadyMainConnect = 0;
void main_client_connection_close(struct tcp_pcb *tpcb) {
	/* remove callbacks */
	tcp_arg(tpcb, NULL);
	tcp_recv(tpcb, NULL);
	tcp_err(tpcb, NULL);
	/* close tcp connection */
	tcp_close(tpcb);
	ReadyMainConnect = 0;
}
//Callback на ошибки сервера
void main_server_error(void *arg, err_t err) {
	LWIP_UNUSED_ARG(err);
	Debug_Message(LOG_FATAL, "Ошибка сервера");
	main_client_connection_close(main_pcb);

}
//Callback на прием от TCP
err_t main_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
		err_t err) {
	err_t ret_err;

	/* Если передали пустой фрейм то это keepalive */
	if (p == NULL) {
		return ERR_OK;
	}
	/* else : a non empty frame was received from server but for some reason err != ERR_OK */
	if (err != ERR_OK) {
		/* free received pbuf*/
		if (p != NULL) {
			pbuf_free(p);
		}
		return err;
	}
	if((ptrMainBuffer+p->len)>(&TCPMainBuffer+MAX_LEN_TCP_MESSAGE-1)) {
		Debug_Message(LOG_FATAL, "Нужно увеличить длину строки TCP");
	}else {
		memcpy(ptrMainBuffer,p->payload,p->len);
		ptrMainBuffer+=p->len;
		*ptrMainBuffer=0;
	}
	if (p->len >2) {
		if(*(ptrMainBuffer-2)=='\n'&& *(ptrMainBuffer-1)=='\r') {
			// Передача строки завершена
			*(ptrMainBuffer-2)=0;
			ptrMainBuffer=TCPMainBuffer;
			tcp_recved(tpcb, p->tot_len);
			pbuf_free(p);
//			mainRecieve();
			return ERR_OK;
		}
	}
	tcp_recved(tpcb, p->tot_len);
	pbuf_free(p);

	/* Acknowledge data reception */
	return ERR_OK;
}

//Callback на соединение с сервером
err_t main_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
	if (err == ERR_OK) {
		tcp_arg(tpcb, NULL);
		/* initialize LwIP tcp_recv callback function */
		tcp_recv(tpcb, main_client_recv);
		tcp_err(tpcb, main_server_error);
		ReadyMainConnect = 1;
		Debug_Message(LOG_INFO, "Подключение по основному каналу успешно");
		return ERR_OK;
	} else {
		/* close connection */
		Debug_Message(LOG_ERROR, "Подключение по основному каналу нет ответа");
		main_client_connection_close(tpcb);
	}
	return err;
}
void TCPMainLoop(void) {
	TCPMainReadSetup();
	main_pcb = tcp_new();
	err_t err;
	if (main_pcb != NULL) {
		/* connect to destination address/port */
		err=tcp_connect(main_pcb, &TCPMainSetup.ipAddr, TCPMainSetup.port,
				main_client_connected);

	} else {
		Debug_Message(LOG_FATAL, "Нет памяти для TCP");
		return;
	}
	while (!ReadyMainConnect) {
		osDelay(100);
	}

}

void TCPMainReadSetup(void) {
	JSON_Value *root = ShareGetJson("tcpmain");
	JSON_Object *object = json_value_get_object(root);
	TCPMainSetup.port = (int) json_object_get_number(object, "port");
	TCPMainSetup.timeout = (int) json_object_get_number(object, "timeout");
	sscanf(json_object_get_string(object, "ip"), "%d.%d.%d.%d",
			&TCPMainSetup.adr1, &TCPMainSetup.adr2, &TCPMainSetup.adr3,
			&TCPMainSetup.adr4);
	IP_ADDR4(&TCPMainSetup.ipAddr, TCPMainSetup.adr1, TCPMainSetup.adr2,
			TCPMainSetup.adr3, TCPMainSetup.adr4);
}

void TCPMainWriteSetup(void) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;
	sprintf(TCPMainBuffer, "%d.%d.%d.%d", TCPMainSetup.adr1, TCPMainSetup.adr2,
			TCPMainSetup.adr3, TCPMainSetup.adr4);
	json_object_set_string(root_object, "ip", TCPMainBuffer);
	json_object_set_number(root_object, "port", TCPMainSetup.port);
	json_object_set_number(root_object, "timeout", TCPMainSetup.timeout);
	ShareSetJson("tcpmain", root_value);
}

