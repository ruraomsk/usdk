/*
 * RingBuffer.c
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#include "RingBuffer.h"
#include <string.h>

RingBuffer* newRingBuffer(unsigned long int capacity,int elementsize) {
	void* mem = malloc(capacity * elementsize);
	if(mem == NULL) {return NULL;}


	RingBuffer* rb = malloc(sizeof(RingBuffer));
	if(rb == NULL) {free(mem); return NULL;}


	rb->start = mem;
	rb->end = mem + capacity * elementsize;
	rb->readptr = mem;
	rb->writeptr = mem;
	rb->elementsize=elementsize;
	rb->overwrite=0;
	return rb;
}
void setOverWriteRingBuffer(RingBuffer* rb){
	if(rb == NULL) return;
	rb->overwrite=1;
}
void deleteRingBuffer(RingBuffer* rb) {
	if(rb == NULL) return;


	free(rb->start);
	free(rb);
}

int RingBufferTryWrite(RingBuffer* rb, void* element) {
	if(rb == NULL) return RINGBUFFER_ERR_NULL;

	if(rb->writeptr + rb->elementsize == rb->readptr && !rb->overwrite){
		return RINGBUFFER_ERR_FULL;
	}
	if(rb->writeptr + rb->elementsize == rb->readptr && rb->overwrite){
		//Буфер полон убираем чтением первое сообщение
		void* tmp = malloc(rb->elementsize);
		RingBufferTryRead(rb, tmp);
		free(tmp);
	}

	memcpy(rb->writeptr,element,rb->elementsize);

	void* tmp = rb->writeptr + rb->elementsize;
	if(tmp >= rb->end) tmp = rb->start;
	rb->writeptr = tmp;


	return RINGBUFFER_OK;
}

int RingBufferTryRead(RingBuffer* rb, void* element) {
	if(rb == NULL) return RINGBUFFER_ERR_NULL;

	if(rb->readptr == rb->writeptr) return RINGBUFFER_ERR_EMPTY;


	memcpy(element,rb->readptr,rb->elementsize);

	void* tmp = rb->readptr + rb->elementsize;
	if(tmp >= rb->end) tmp = rb->start;
	rb->readptr = tmp;


	return RINGBUFFER_OK;
}
