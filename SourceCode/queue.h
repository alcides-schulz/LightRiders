/*-------------------------------------------------------------------------------
** Light Rider Queue
**-----------------------------------------------------------------------------*/

#include "definitions.h"

typedef struct {
	int		items[FIELD_SIZE];
	int		first;
	int		last;
}			QUEUE;

void	queue_init(QUEUE *q);
void	queue_add(QUEUE *q, int item);
int		queue_empty(QUEUE *q);
int		queue_next(QUEUE *q);

//END
