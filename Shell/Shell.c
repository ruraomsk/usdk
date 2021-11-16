/*
 * Shell.c
 *
 *  Created on: 12 нояб. 2021 г.
 *      Author: rura
 */
#include "cmsis_os.h"
#include "Shell.h"
#include "usbd_cdc_if.h"

uint8_t rxFlag;   	//Если ==1 То данные в буфере 0 - ожидаем данные
uint8_t rxBuffer [ APP_RX_DATA_SIZE ];
uint16_t rxLenght;
osThreadId_t ShellHandle;
const osThreadAttr_t Shell_attributes = { .name = "shell", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
char* readStringFromUsb() {
	rxLenght = 0;
	memset(rxBuffer, 0, sizeof(rxBuffer));
	rxFlag = 0;
	while (rxFlag != 1) {
		osDelay(100);
	}
	rxBuffer[rxLenght]=0;
	return rxBuffer;
}
void writeDataToUsb(char* buffer,uint16_t lenght){
	char RS=30;
	while(lenght>0){
		uint16_t len=(lenght>APP_TX_DATA_SIZE)?APP_TX_DATA_SIZE:lenght;
		CDC_Transmit_FS(RS, 1);
		osDelay(100U);
		CDC_Transmit_FS(buffer, len);
		osDelay(100U);
		CDC_Transmit_FS(RS, 1);
		osDelay(100U);
		lenght-=len;
		buffer+=len;
	}
}
void InitShell(void) {
	Rus_Init_Fs();
	ShellHandle = osThreadNew(ShellWork, NULL, &Shell_attributes);
	cmdInit();
}

void ShellWork(void *arg) {
	char *buffer;
	for (;;) {
		buffer=readStringFromUsb();
		doCmd(buffer);
		CDC_Transmit_FS(buffer, (uint16_t) strlen((char*) buffer));
		osDelay(100);
		sayReady(buffer);
	}

}

