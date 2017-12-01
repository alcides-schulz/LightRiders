/*-------------------------------------------------------------------------------
** Light Rider openings generator
**-----------------------------------------------------------------------------*/

#ifdef DEVELOP

#include "definitions.h"

extern GAME_STATE		game_state;
extern GAME_SETTINGS	game_settings;

void generate_moves(int index, int count);
void mirror_moves(void);
void save_opening(char *field);

#define MAX_OPEN 50000

int		open_count = 0;
char	open_table[MAX_OPEN][FIELD_SIZE];
FILE	*open_file;

void generate_openings(void) {
	open_file = fopen("opening.txt", "w");
	if (open_file == NULL) {
		fprintf(stderr, "cannot create opening file.");
		return;
	}
	for (int r = 1; r < FIELD_ROWS - 1; r++) {
		for (int c = 1; c < (FIELD_COLS / 2) - 1; c++) {
			for (int i = 0; i < FIELD_SIZE; i++) game_state.field[i] = EMPTY;
			game_state.field[r * FIELD_COLS + c] = WALL;
			generate_moves(r * FIELD_COLS + c, 5);
		}
	}
	fclose(open_file);
}

void generate_moves(int index, int depth) {
	if (depth == 0) {
		game_state.field[index] = PLAYER0;
		mirror_moves();
		memcpy(open_table[open_count++], game_state.field, FIELD_SIZE);
		printf("%d\n", open_count);
		save_opening(game_state.field);
		//print_state();
		//my_pause("gm");
		return;
	}
    int neighbour[4];
	int neighbour_count = get_empty_neighbours(index, neighbour);
	for (int i = 0; i < neighbour_count; i++) {
		if (get_col(neighbour[i]) == 0 || get_col(neighbour[i]) == FIELD_COLS / 2 - 1) continue;
		if (get_row(neighbour[i]) == 0 || get_row(neighbour[i]) == FIELD_ROWS - 1) continue;
		game_state.field[neighbour[i]] = WALL;
		generate_moves(neighbour[i], depth - 1);
		game_state.field[neighbour[i]] = EMPTY;
	}
}

void mirror_moves(void) {
	for (int r = 1; r < FIELD_ROWS - 1; r++) {
		for (int c = 1; c < FIELD_COLS - 1; c++) {
			int index = r * FIELD_COLS + c;
			int op_index = r * FIELD_COLS + (FIELD_COLS - 1 - c);
			if (game_state.field[index] == EMPTY) game_state.field[op_index] = EMPTY;
			if (game_state.field[index] == PLAYER0) game_state.field[op_index] = PLAYER1;
			if (game_state.field[index] == WALL) game_state.field[op_index] = WALL;
		}
	}

}

void save_opening(char *field) {
	for (int i = 0; i < FIELD_SIZE; i++) {
		if (i == 0)
			fprintf(open_file, "%c", field[i]);
		else
			fprintf(open_file, ",%c", field[i]);
	}
	fprintf(open_file, "\n");
}

#endif

// END
