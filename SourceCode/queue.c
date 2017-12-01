/*-------------------------------------------------------------------------------
** Light Rider Queue
**-----------------------------------------------------------------------------*/

#include "queue.h"

void queue_init(QUEUE *q) {
	q->first = 0;
	q->last = 0;
}

void queue_add(QUEUE *q, int item) {
	q->items[q->last++] = item;
}

int queue_empty(QUEUE *q) {
	return q->first >= q->last;
}

int queue_next(QUEUE *q) {
	return q->items[q->first++];
}

//END
