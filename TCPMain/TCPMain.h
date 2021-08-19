/*
 * TCPMain.h
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */

#ifndef TCPMAIN_H_
#define TCPMAIN_H_
#define MAX_LEN_TCP_MESSAGE 2048
void TCPMainLoop(void);
void TCPMainReadSetup(void);
void TCPMainWriteSetup(void);

#endif /* TCPMAIN_H_ */
