#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdint.h>

#define CQ_CAPACITY 256

typedef struct {
	char buf[CQ_CAPACITY];
	uint32_t begin; // inclusive
	uint32_t end; // exclusive
	int full;	// If begin==end: if full==0, the queue is empty. If full==1, the queue is full.
} CircularQueue;

uint32_t CQGetLen(CircularQueue* cq);
int CQPush(CircularQueue* cq, char x);
int CQPush_n(CircularQueue* cq, const char* src, uint32_t len);
char CQPop(CircularQueue* cq);
void CQClear(CircularQueue* cq);

#endif
