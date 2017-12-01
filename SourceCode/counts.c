/*-------------------------------------------------------------------------------
** Light Rider Development utils
**-----------------------------------------------------------------------------*/

#include "definitions.h"

void counts_reset(COUNTS *counts) {
    counts->start_node = 0;
    counts->nodes = 0;
    counts->edges = 0;
    counts->reds = 0;
    counts->blacks = 0;
    counts->nodes_adjust = 0;
}

void counts_init(COUNTS *counts, int index) {
	counts->start_node = index;
	counts->nodes = 1;
	counts->edges = count_empty_neighbours(index);
	if (counts_is_red(index)) {
		counts->reds = 1;
		counts->blacks = 0;
	}
	else {
		counts->reds = 0;
		counts->blacks = 1;
	}
	counts->nodes_adjust = 0;
}

// 0,0 = r, 0,1 = b, 0,2 = r
// 1,0 = b, 1,1 = r, 1,2 = b
int counts_is_red(int index) {
	int row = get_row(index);
	int col = get_col(index);
	int row_even = (row % 2 ? FALSE : TRUE);
	int col_even = (col % 2 ? FALSE : TRUE);
	return row_even ? col_even : !col_even;
}

void counts_add(COUNTS *counts, int index) {
	counts->nodes += 1;
	counts->edges += count_empty_neighbours(index);
	if (counts_is_red(index))
		counts->reds++;
	else
		counts->blacks++;
}

void counts_combine(COUNTS *counts, COUNTS *other) {
	counts->nodes += other->nodes;
	counts->edges += other->edges;
	counts->reds += other->reds;
	counts->blacks += other->blacks;
}

void counts_adjust(COUNTS *counts) {
	if (counts_is_red(counts->start_node))
		counts->nodes_adjust = 2 * (MIN(counts->reds - 1, counts->blacks)) + (counts->blacks >= counts->reds ? 1 : 0);
	else
		counts->nodes_adjust = 2 * (MIN(counts->blacks - 1, counts->reds)) + (counts->reds >= counts->blacks ? 1 : 0);
	if (counts->nodes_adjust <= 0) counts->nodes_adjust = 1;
}

void counts_print_board(void) {
	for (int row = 0; row < FIELD_ROWS; row++) {
		for (int col = 0; col < FIELD_COLS; col++) {
			printf("%c ", counts_is_red(row * FIELD_COLS + col) ? 'r' : 'b');
		}
		printf("\n");
	}
}
// END
