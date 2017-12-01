/*-------------------------------------------------------------------------------
** Game state
**-----------------------------------------------------------------------------*/

#include "definitions.h"
#include "queue.h"

GAME_STATE      game_state;

void init_state(void) {
	memset(&game_state, 0, sizeof(GAME_STATE));
	game_state.player[0].id = PLAYER0;
    game_state.player[0].my_number = 0;
    game_state.player[0].opp_number = 1;
	game_state.player[1].id = PLAYER1;
    game_state.player[1].my_number = 1;
    game_state.player[1].opp_number = 0;
    game_state.turn = 0;
    game_state.key = 0;
}

void update_field(char *value) {
	int		i;
	int		j;

	for (i = j = 0; i < FIELD_SIZE && value[j]; j++) {
		if (value[j] == ',') continue;
		game_state.field[i] = value[j];
		if (value[j] == '0') game_state.player[0].index = i;
		if (value[j] == '1') game_state.player[1].index = i;
		i++;
	}
    game_state.key = calc_zkey(game_state.turn);
}

void update(char *player, char *type, char *value) {

	assert(player != NULL);
	assert(type != NULL);
	assert(value != NULL);

	if (!strcmp(player, "game")) {
		if (!strcmp(type, "round")) {
			game_state.round = atoi(value);
		}
		if (!strcmp(type, "field")) {
			update_field(value);
		}
	}
}

int is_empty(int index) {
	assert(index >= 0 && index < field_size());
	return game_state.field[index] == EMPTY ? TRUE : FALSE;
}

int opposite(char move) {
	assert(strchr("udrl", move) != NULL);

	if (move == MOVE_UP)    return MOVE_DOWN;
	if (move == MOVE_DOWN)  return MOVE_UP;
	if (move == MOVE_LEFT)  return MOVE_RIGHT;
	if (move == MOVE_RIGHT) return MOVE_LEFT;

	return move;
}

int can_move_up(PLAYER *player) {
	assert(player != NULL);
	assert(player->index >= 0 && player->index < field_size());

	if (player->index < FIELD_COLS) return FALSE;
	if (!is_empty(player->index - FIELD_COLS)) return FALSE;

	return TRUE;
}

int can_move_down(PLAYER *player) {
	int		row;

	assert(player != NULL);
	assert(player->index >= 0 && player->index < field_size());

	row = get_row(player->index);
	if (row >= FIELD_ROWS - 1) return FALSE;
	if (!is_empty(player->index + FIELD_COLS)) return FALSE;

	return TRUE;
}

int can_move_left(PLAYER *player) {
	int		col;

	assert(player != NULL);
	assert(player->index >= 0 && player->index < field_size());

	col = get_col(player->index);
	if (col < 1) return FALSE;
	if (!is_empty(player->index - 1)) return FALSE;

	return TRUE;
}

int can_move_right(PLAYER *player) {
	int		col;

	assert(player != NULL);
	assert(player->index >= 0 && player->index < field_size());

	col = get_col(player->index);
	if (col >= FIELD_COLS - 1) return FALSE;
	if (!is_empty(player->index + 1)) return FALSE;

	return TRUE;
}

int can_move(PLAYER *player, char move) {
	assert(player == &game_state.player[0] || player == &game_state.player[1]);
	assert(strchr(MOVE_LIST, move) != NULL);

	switch (move) {
	case MOVE_UP:    return can_move_up(player);
	case MOVE_DOWN:  return can_move_down(player);
	case MOVE_RIGHT: return can_move_right(player);
	case MOVE_LEFT:  return can_move_left(player);
	}

	return FALSE;
}

void get_neighbours(int index, int *list) {
	int		i = 0;
	int		row = get_row(index);
	int		col = get_col(index);

	assert(index >= 0 && index < field_size());

	if (row > 0)
		list[i++] = -FIELD_COLS;
	if (row < FIELD_ROWS - 1)
		list[i++] = FIELD_COLS;
	if (col > 0)
		list[i++] = -1;
	if (col < FIELD_COLS - 1)
		list[i++] = 1;
	list[i] = 0;
}

int get_empty_neighbours(int index, int list[]) {
	int		i = 0;
	int		row = get_row(index);
	int		col = get_col(index);

	assert(index >= 0 && index < FIELD_SIZE);

	if (row > 0 && game_state.field[index - FIELD_COLS] == EMPTY)
		list[i++] = index - FIELD_COLS;
	if (row < FIELD_ROWS - 1 && game_state.field[index + FIELD_COLS] == EMPTY)
		list[i++] = index + FIELD_COLS;
	if (col > 0 && game_state.field[index - 1] == EMPTY)
		list[i++] = index - 1;
	if (col < FIELD_COLS - 1 && game_state.field[index + 1] == EMPTY)
		list[i++] = index + 1;
    return i;
}

void gen_move_to(PLAYER *player, int *list) {
	int i = 0;
	if (can_move_up(player))    list[i++] = player->index - FIELD_COLS;
	if (can_move_down(player))  list[i++] = player->index + FIELD_COLS;
	if (can_move_left(player))  list[i++] = player->index - 1;
	if (can_move_right(player)) list[i++] = player->index + 1;
	list[i] = 0;
}

void get_moves(PLAYER *player, char *list) {
	int i = 0;
	if (can_move_up(player)) list[i++] = MOVE_UP;
	if (can_move_down(player)) list[i++] = MOVE_DOWN;
	if (can_move_right(player)) list[i++] = MOVE_RIGHT;
	if (can_move_left(player)) list[i++] = MOVE_LEFT;
	list[i] = '\0';
}

int count_empty_neighbours(int index) {
	int		row = get_row(index);
	int		col = get_col(index);
	int		count = 0;

	assert(index >= 0 && index < field_size());

	if (row > 0 && game_state.field[index - FIELD_COLS] == EMPTY)
		count++;
	if (row < FIELD_ROWS - 1 && game_state.field[index + FIELD_COLS] == EMPTY)
		count++;
	if (col > 0 && game_state.field[index - 1] == EMPTY)
		count++;
	if (col < FIELD_COLS - 1 && game_state.field[index + 1] == EMPTY)
		count++;
	return count;
}

int players_are_connected(void) {
	int		current;
	int		neighbour;
	int		list[5];
	int		i;
	QUEUE	queue[1];
	char	visited[FIELD_SIZE];

	memset(visited, 0, sizeof(visited));

	queue_init(queue);

	queue_add(queue, game_state.player[0].index);
	visited[game_state.player[0].index] = TRUE;

	while (!queue_empty(queue)) {
		current = queue_next(queue);
		assert(current >= 0 && current < FIELD_SIZE);
		get_neighbours(current, list);
		for (i = 0; list[i]; i++) {
			assert(list[i] == 1 || list[i] == -1 || list[i] == FIELD_COLS || list[i] == -FIELD_COLS);
			neighbour = current + list[i];
			assert(neighbour >= 0 && neighbour < FIELD_SIZE);
            if (visited[neighbour]) continue;
            visited[neighbour] = TRUE;
            if (game_state.field[neighbour] == PLAYER1) return TRUE;
			if (game_state.field[neighbour] != EMPTY) continue;
			queue_add(queue, neighbour);
		}
	}

	return FALSE;
}

void print_state(void) {
	int     row;
	int     col;
	char	item;

	for (row = 0; row < FIELD_ROWS; row++) {
		fprintf(stderr, "    ");
		for (col = 0; col < FIELD_COLS; col++) {
			if (row == 0) {
				if (col == 0)
					fprintf(stderr, "%c", 218);
				else
					fprintf(stderr, "%c", 194);
			}
			else {
				if (col == 0)
					fprintf(stderr, "%c", 195);
				else
					fprintf(stderr, "%c", 197);
			}
			fprintf(stderr, "%c%c%c", 196, 196, 196);
		}
		if (row == 0)
			fprintf(stderr, "%c\n", 191);
		else
			fprintf(stderr, "%c\n", 180);

		for (col = 0; col < FIELD_COLS; col++) {
			if (col == 0)
				fprintf(stderr, "%3d %c", row * FIELD_COLS, 179);
			item = game_state.field[row * FIELD_ROWS + col];
			if (item == EMPTY) item = ' ';
			fprintf(stderr, " %c %c", item, 179);
		}

		fprintf(stderr, " %3d\n", row * FIELD_COLS + FIELD_COLS - 1);
	}

	fprintf(stderr, "    ");
	for (col = 0; col < FIELD_COLS; col++) {
		if (col == 0)
			fprintf(stderr, "%c", 192);
		else
			fprintf(stderr, "%c", 193);
		fprintf(stderr, "%c%c%c", 196, 196, 196);
	}
	fprintf(stderr, "%c\n", 217);

	fprintf(stderr, "player[0]: index: %d  id: %c\n", game_state.player[0].index, game_state.player[0].id);
	fprintf(stderr, "player[1]: index: %d  id: %c\n", game_state.player[1].index, game_state.player[1].id);

    fprintf(stderr, "turn: %d key=%llu  calc_key0=%llu calc_key1=%llu\n", game_state.turn, game_state.key, calc_zkey(0), calc_zkey(1));
}

//END