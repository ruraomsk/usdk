/*
 * RingBuffer.h
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdlib.h>

//#define CHAR_SIZE (sizeof(char))

#define RINGBUFFER_OK (0)
#define RINGBUFFER_ERR_NULL (-1)
#define RINGBUFFER_ERR_EMPTY (-2)
#define RINGBUFFER_ERR_FULL (-3)

typedef struct {
	void* start;
	void* end;
	int elementsize;
	char overwrite;   			//если тип ==0 то буфер с переполнением, иначе при записи в полный буфер теряем первую запись
	void* readptr;
	void* writeptr;
} RingBuffer;

RingBuffer* newRingBuffer(unsigned long int capacity,int elementsize);
void setOverWriteRingBuffer(RingBuffer* rb);
void deleteRingBuffer(RingBuffer* rb);
int RingBufferTryWrite(RingBuffer* rb, void* element);
int RingBufferTryRead(RingBuffer* rb, void* element);
#endif /* RINGBUFFER_H_ */
