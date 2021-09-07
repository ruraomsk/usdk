/*
 * Camera.c
 *
 *  Created on: Sep 2, 2021
 *      Author: rura
 */
#include "lwip.h"
#include "DebugLogger.h"
#include "Transport.h"
#include "parson.h"
#include "sockets.h"
#include "Files.h"

#include "Camera.h"

#define MAX_LEN_CAMERA_MESSAGE 1024
int socket = -1;
char *buffer = NULL;

void initCamera() {

}
void badCamera() {
	free(buffer);
	if (socket < 0)
		return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
int sendAndRecv(){
	int err = send(socket, buffer, strlen(buffer), 0);
	osDelay(1000);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "Camera Не смог передать строку %s", buffer);
		return -1;
	}
	memset(buffer, 0, MAX_LEN_CAMERA_MESSAGE);
	int len = recv(socket, buffer, MAX_LEN_CAMERA_MESSAGE, 0);
	if (len < 2) {
		Debug_Message(LOG_ERROR, "Camera Ошибка чтения ");
		return -1;
	}
	if (buffer[len - 2] != '\r' ||buffer[len - 1] != '\n' ) {
		Debug_Message(LOG_ERROR, "Camera Неверное завершение строки %s",
				buffer);
		return -1;
	}
	buffer[len - 2] = 0;
	return strlen(buffer);
}
void cameraMainLoop() {
	struct sockaddr_in srv_addr;
	JSON_Value *root = ShareGetJson("camera");
	JSON_Object *object = json_value_get_object(root);
	inet_aton(json_object_get_string(object, "ip"), &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((int ) json_object_get_number(object, "port"));
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR, "Camera не могу создать сокет %d", errno);
		badCamera(buffer, socket);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = 5;
	int err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = 5;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	err = connect(socket, (struct sockaddr* ) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "Camera нет соединения с сервером %s:%d",
				json_object_get_string(object, "ip"),
				(int) json_object_get_number(object, "port"));
		badCamera();
		return;
	}
	buffer = malloc(MAX_LEN_TCP_MESSAGE);
	sprintf(buffer, "%s\r\n%s\r\n", json_object_get_string(object, "login"),
			json_object_get_string(object, "password"));
	if (sendAndRecv() < 0 ) {
		badCamera();
		return;
	}
	if (strcasecmp(buffer, "Gamotron like detector with 1 identifier.") != 0) {
		Debug_Message(LOG_ERROR,
				"Camera неверный логин или пароль для сервера %s:%d",
				json_object_get_string(object, "ip"),
				(int) json_object_get_number(object, "port"));
		badCamera();
		return;
	}
	for(;;){
		sprintf(buffer,"gd last\r\n");
		if (sendAndRecv()<0){
			badCamera();
			return;
		}
		Debug_Message(LOG_INFO,"Пришло %s",buffer);
		osDelay((int) json_object_get_number(object, "step")*1000);
	}
}
