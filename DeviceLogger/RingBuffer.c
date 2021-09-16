/*
 * RingBuffer.c
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#include "RingBuffer.h"
#include "cmsis_os.h"
#include <string.h>

RingBuffer* newRingBuffer(unsigned long int capacity, int elementsize) {
	void *mem = pvPortMalloc(capacity * elementsize);
	if (mem == NULL) 		return NULL;
	RingBuffer *rb = pvPortMalloc(sizeof(RingBuffer));
	if (rb == NULL) {
		vPortFree(mem);
		return NULL;
	}
	rb->start = mem;
	rb->end = mem + capacity * elementsize;
	rb->readptr = mem;
	rb->writeptr = mem;
	rb->elementsize = elementsize;
	rb->overwrite = 0;
	return rb;
}
void setOverWriteRingBuffer(RingBuffer *rb) {
	if (rb == NULL) return;
	rb->overwrite = 1;
}
void deleteRingBuffer(RingBuffer *rb) {
	if (rb == NULL) return;

	vPortFree(rb->start);
	vPortFree(rb);
}

int RingBufferTryWrite(RingBuffer *rb, void *element) {
	if (rb == NULL) return RINGBUFFER_ERR_NULL;

	void *tmp = rb->writeptr + rb->elementsize;
	if (tmp >= rb->end) tmp = rb->start;

	if (tmp == rb->readptr) {
		if (!rb->overwrite)
			return RINGBUFFER_ERR_FULL;
		else {
			//Буфер полон убираем чтением первое сообщение
			void *tmp = pvPortMalloc(rb->elementsize);
			RingBufferTryRead(rb, tmp);
			vPortFree(tmp);
		}
	}

	memcpy(rb->writeptr, element, rb->elementsize);

	tmp = rb->writeptr + rb->elementsize;
	if (tmp >= rb->end) tmp = rb->start;
	rb->writeptr = tmp;

	return RINGBUFFER_OK;
}

int RingBufferTryRead(RingBuffer *rb, void *element) {
	if (rb == NULL) return RINGBUFFER_ERR_NULL;

	if (rb->readptr == rb->writeptr) return RINGBUFFER_ERR_EMPTY;

	memcpy(element, rb->readptr, rb->elementsize);

	void *tmp = rb->readptr + rb->elementsize;
	if (tmp >= rb->end) tmp = rb->start;
	rb->readptr = tmp;

	return RINGBUFFER_OK;
}
