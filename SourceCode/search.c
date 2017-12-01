/*-------------------------------------------------------------------------------
** Light Rider AI Engine
**-----------------------------------------------------------------------------*/

#include "definitions.h"

extern GAME_STATE		game_state;
extern GAME_SETTINGS	game_settings;

unsigned	time_limit = 0;
unsigned	start_time = 0;
int			abort_search = 0;
int			node_count = 0;
int			start_turn;
int			min_row = 0;
int			max_row = 15;
int			min_col = 0;
int			max_col = 15;

int			move_hist[2][256][4];

void clear_hist(void) {
	memset(move_hist, 0, sizeof(move_hist));
}

int get_move_index(char move)
{
	switch (move)
	{
	case MOVE_UP: return 0;
	case MOVE_DOWN: return 1;
	case MOVE_RIGHT: return 2;
	case MOVE_LEFT: return 3;
	}
	return 0;
}

void save_hist(int turn, int index, char move, int depth)
{
	move_hist[turn][index][get_move_index(move)] += (depth * depth);
}

int get_hist(int turn, int index, char move)
{
	return move_hist[turn][index][get_move_index(move)];
}

void sort_moves(int turn, int index, char *list) {
	int i;
	int len = strlen(list);
	int change = TRUE;
	char temp;
	if (len <= 1) return;

	while (change) {
		change = FALSE;
		for (i = 1; i < len; i++) {
			if (get_hist(turn, index, list[i]) > get_hist(turn, index, list[i - 1])) {
				temp = list[i - 1];
				list[i - 1] = list[i];
				list[i] = temp;
				change = TRUE;
			}
		}
	}
}

void check_time(void) {
	node_count++;
	if (node_count % 1000 != 0) return;
	if (get_time() >= time_limit) abort_search = TRUE;
}

void make_move(PLAYER *player, char move) {
	assert(player == (&game_state.player[0]) || player == (&game_state.player[1]));
	assert(strchr(MOVE_LIST, move) != NULL);

    int from = player->index;
    int to = player->index;

	switch (move) {
	case MOVE_UP:    assert(can_move_up(player));    to -= FIELD_COLS; break;
	case MOVE_DOWN:  assert(can_move_down(player));  to += FIELD_COLS; break;
	case MOVE_RIGHT: assert(can_move_right(player)); to += 1; break;
	case MOVE_LEFT:  assert(can_move_left(player));  to -= 1; break;
	default: assert(0);
	}

    game_state.key ^= zkey_turn(game_state.turn);
    game_state.key ^= zkey_value(from);
    game_state.key ^= zkey_value(to);

    game_state.field[from] = WALL;
    game_state.field[to] = player->id;
    player->index = to;
    game_state.turn ^= 1;

    game_state.key ^= zkey_value(to);
    game_state.key ^= zkey_value(from);
    game_state.key ^= zkey_turn(game_state.turn);
}

void undo_move(PLAYER *player, char move) {
	assert(player == &game_state.player[0] || player == &game_state.player[1]);
	assert(strchr(MOVE_LIST, move) != NULL);

    int from = player->index;
    int to = player->index;

	switch (move) {
	case MOVE_UP:    to += FIELD_COLS; break;
	case MOVE_DOWN:  to -= FIELD_COLS; break;
	case MOVE_RIGHT: to -= 1; break;
	case MOVE_LEFT:  to += 1; break;
	default: assert(0);
	}

    game_state.key ^= zkey_turn(game_state.turn);
    game_state.key ^= zkey_value(from);
    game_state.key ^= zkey_value(to);

	game_state.field[from] = EMPTY;
    game_state.field[to] = player->id;
	player->index = to;
    game_state.turn ^= 1;

    game_state.key ^= zkey_value(to);
    game_state.key ^= zkey_value(from);
    game_state.key ^= zkey_turn(game_state.turn);
}

/*
update game round 104
update game field x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,x,x,x,x,x,.,.,x,x,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,0,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,.,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,1,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,x,x,x,x,.,.,.,.,x,x,x,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x
note: correct move is down, without eval_end_position moved right
*/
int eval_end_position() {
	int neighbour0[4];
	int neighbour1[4];

    int moves0 = get_empty_neighbours(game_state.player[0].index, neighbour0);
    int moves1 = get_empty_neighbours(game_state.player[1].index, neighbour1);

	if (moves0 == 0 && moves1 == 0) return END_POS_DRAW;
	if (moves0 != 0 && moves1 == 0) return END_POS_WIN0;
	if (moves0 == 0 && moves1 != 0) return END_POS_WIN1;
	if (moves0 != 1 && moves1 != 1) return END_POS_UNDEF;

	if (neighbour0[0] == neighbour1[0]) return END_POS_DRAW;

	return END_POS_UNDEF;
}

int search(int depth, int ply, int64_t alfa, int64_t beta) {
	char	list[5];
	int 	val;
	int 	best = -INF;
	int		i;
	int		end_pos;
    int     turn = game_state.turn;

	check_time();
	if (abort_search) return 0;

	if (start_turn == turn) {
		end_pos = eval_end_position();
		if (end_pos == END_POS_DRAW) return 0;
		if (end_pos == END_POS_WIN0) return (turn == 0 ? +WIN - ply : -WIN + ply);
		if (end_pos == END_POS_WIN1) return (turn == 0 ? -WIN + ply : +WIN - ply);
	}

	if (depth <= 0) {
		// evaluate after both players moved.
		if (start_turn == turn)	return evaluation(turn);
		depth = 1;
	}

	get_moves(&game_state.player[turn], list);
	sort_moves(turn, game_state.player[turn].index, list);

	for (i = 0; list[i]; i++) {
		make_move(&game_state.player[turn], list[i]);
		val = -search(depth - 1, ply + 1, -beta, -alfa);
		undo_move(&game_state.player[turn], list[i]);
            
        if (abort_search) return 0;

		if (val > best) {
			best = val;
			if (val > alfa) {
				alfa = val;
			}
			if (val >= beta) {
				save_hist(turn, game_state.player[turn].index, list[i], depth);
				return val;
			}
		}
	}

	if (best == -INF)
		return -WIN + ply;

	return best;
}

char root(int depth, int64_t alfa, int64_t beta) {
	char	list[5];
	int		i;
	char	best = MOVE_NONE;
	int 	val;
    int     turn = game_state.turn;

	get_moves(&game_state.player[turn], list);
	sort_moves(turn, game_state.player[turn].index, list);

	for (i = 0; list[i]; i++) {
		make_move(&game_state.player[turn], list[i]);
		val = -search(depth - 1, 0, -beta, -alfa);
		undo_move(&game_state.player[turn], list[i]);

		//if (depth > 5)	fprintf(stderr, "root d=%2d (%5d) m=%c v=%5d time=%5d abort=%d\n", depth, node_count, list[i], val, get_time() - start_time, abort_search);

		if (abort_search) break;

		if (val > alfa) {
			best = list[i];
			alfa = val;
			save_hist(turn, game_state.player[turn].index, list[i], depth);
		}
	}

	return best;
}

char find_move(int time, int turn, int max_depth) {
	int		depth;
	char	move;
	char	best_move = MOVE_NONE;
	int		best_depth = 2;

	start_time = get_time();
	time_limit = start_time + (time / 5);
	node_count = 0;
	abort_search = FALSE;
	start_turn = turn;
    if (game_state.turn != turn) {
        game_state.key ^= zkey_turn(turn);
        game_state.key ^= zkey_turn(turn ^ 1);
    }
    game_state.turn = turn;

	memset(move_hist, 0, sizeof(move_hist));

	prepare_initial_area();

	for (depth = 2; depth <= MAX_DEPTH && depth <= max_depth; depth += 2) {
		if (get_time() > start_time + (time / 5 * 3)) break;
		move = root(depth, -INF, INF);
		if (abort_search) break;
		best_move = move;
		best_depth = depth;
	}

	fprintf(stderr, "round: %d nodes %d move: %c time: %d/%d depth: %d\n",
		game_state.round, node_count, best_move, (get_time() - start_time), time, best_depth);

	return best_move;
}

//END
