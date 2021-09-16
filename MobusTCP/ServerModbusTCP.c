/*
 * ServerModbusTCP.c
 *
 *  Created on: Aug 24, 2021
 *      Author: rura
 */

#include "ServerModbusTCP.h"
#include "DebugLogger.h"
#include "sockets.h"

//static fd_set refset;
//static int fdmax;
void ServerModbusTCPLoop() {
	modbus_t *ctx = modbus_new_tcp("0.0.0.0", 502);
	uint8_t *query = (uint8_t*) pvPortMalloc(MODBUS_TCP_MAX_ADU_LENGTH);

	modbus_set_debug(ctx, TRUE);

	modbus_mapping_t *mb_mapping = modbus_mapping_new(10, 16, 12, 13);
	if (mb_mapping == NULL) {
		Debug_Message(LOG_FATAL, "Failed to allocate the mapping");
		modbus_free(ctx);
		return;
	}

	int listen_socket = modbus_tcp_listen(ctx, 1);
	if (listen_socket == -1) {
		Debug_Message(LOG_FATAL, "server modbus not listen ");
		modbus_mapping_free(mb_mapping);
		modbus_free(ctx);
		return;
	}
	for(;;){
		int client_socket=modbus_tcp_accept(ctx,&listen_socket);
		if(client_socket==-1) break;
		for(;;){
			int rc = modbus_receive(ctx, query);
			if (rc > 0) {
				modbus_reply(ctx, query, rc, mb_mapping);
			} else break;
		}
		close(client_socket);
	}
	close(listen_socket);
	modbus_mapping_free(mb_mapping);
	vPortFree(query);
	modbus_free(ctx);
}
