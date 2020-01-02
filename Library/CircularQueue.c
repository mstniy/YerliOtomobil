#include "CircularQueue.h"

#include <string.h>

uint32_t CQGetLen(CircularQueue* cq) {
	if (cq->end>cq->begin)
		return cq->end-cq->begin;
	else if (cq->begin>cq->end)
		return CQ_CAPACITY-cq->begin+cq->end;
	else
		return cq->full?CQ_CAPACITY:0;
}

int CQPush(CircularQueue* cq, char x) {
	if (cq->full)
		return 0;
	cq->buf[cq->end++]=x;
	if (cq->end == CQ_CAPACITY) cq->end=0;
	if (cq->begin == cq->end)
		cq->full=1;
	return 1;
}

int CQPush_n(CircularQueue* cq, const char* src, uint32_t len) {
	if (CQGetLen(cq)+len > CQ_CAPACITY)
		return 0;
	const int copy_size = len<(CQ_CAPACITY-cq->end)?len:(CQ_CAPACITY-cq->end);
	memcpy(cq->buf+cq->end, src, copy_size);
	memcpy(cq->buf, src+copy_size, len-copy_size);
	cq->end += len;
	if (cq->end >= CQ_CAPACITY)
		cq->end -= CQ_CAPACITY;
	if (cq->begin==cq->end)
		cq->full=1;
	return 1;
}

char CQPop(CircularQueue* cq) {
	if (CQGetLen(cq) <= 0)
		return 0;
  
	const int data = cq->buf[cq->begin];
	cq->begin++;
	if (cq->begin==CQ_CAPACITY) cq->begin=0;
	cq->full=0;
	return data;
}

void CQClear(CircularQueue* cq) {
	cq->begin = cq->end = cq->full = 0;
}
