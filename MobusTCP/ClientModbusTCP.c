/*
 * ClientModbusTCP.c
 *
 *  Created on: Aug 24, 2021
 *      Author: rura
 */

#include "ClientModbusTCP.h"
#include "DebugLogger.h"
#include "sockets.h"

void ClientModbusTCPLoop(void) {
	char ipserv[] = "127.0.0.1";
	int port = 502;
	modbus_t *ctx = modbus_new_tcp(ipserv, port);
//	uint8_t *query = (uint8_t*) malloc(MODBUS_TCP_MAX_ADU_LENGTH);

	modbus_set_debug(ctx, TRUE);

	modbus_mapping_t *mb_mapping = modbus_mapping_new(10, 16, 12, 13);
	if (mb_mapping == NULL) {
		Debug_Message(LOG_FATAL, "Failed to allocate the mapping");
		modbus_free(ctx);
		return;
	}
	if (modbus_connect(ctx) == -1) {
		Debug_Message(LOG_ERROR, "Modbus client not connection to server %s:%d", ipserv, port);
		modbus_mapping_free(mb_mapping);
		modbus_free(ctx);
		return;
	}
	modbus_set_response_timeout(ctx, 5, 0);
	int err = 0;
	//Начальная инициализация данными устройства
	if (modbus_read_bits(ctx, mb_mapping->start_bits, mb_mapping->nb_bits, mb_mapping->tab_bits) < 0) err++;
	if (modbus_read_registers(ctx, mb_mapping->start_registers, mb_mapping->nb_registers, mb_mapping->tab_registers)
			< 0) err++;
	if (err) {
		Debug_Message(LOG_ERROR, "Modbus client don't read data from server %s", ipserv);
		modbus_close(ctx);
		modbus_mapping_free(mb_mapping);
		modbus_free(ctx);
		return;
	}

	for (;;) {
		//Отправляем все Coils
		if (modbus_write_bits(ctx, mb_mapping->start_bits, mb_mapping->nb_bits, mb_mapping->tab_bits) < 0) break;
		//Читаем все Di
		if (modbus_read_input_bits(ctx, mb_mapping->start_input_bits, mb_mapping->nb_input_bits,
				mb_mapping->tab_input_bits) < 0) break;
		//Читаем все Ir
		if (modbus_read_input_registers(ctx, mb_mapping->start_input_registers, mb_mapping->nb_input_registers,
				mb_mapping->tab_input_registers) < 0) break;
		//Пишем все Hr
		if (modbus_write_registers(ctx, mb_mapping->start_registers, mb_mapping->nb_registers,
				mb_mapping->tab_registers) < 0) break;
		osDelay(500);

	}
	modbus_close(ctx);
	modbus_mapping_free(mb_mapping);
	modbus_free(ctx);
}
