/*-------------------------------------------------------------------------------
** Light Rider Voronoi Heuristic
**-----------------------------------------------------------------------------*/

#include "queue.h"

extern GAME_STATE		game_state;
extern GAME_SETTINGS	game_settings;

int node_distance[2][256];
int initial_area[2][256];

void voronoi_calculate(int player_number) {
	int		neighbour[4];
	int		i;
	QUEUE	queue[1];
    char    visited[FIELD_SIZE];

	assert(player_number == 0 || player_number = 1);

	memset(node_distance[player_number], 0, sizeof(node_distance[player_number]));

    memset(visited, 0, sizeof(visited));
	queue_init(queue);

	queue_add(queue, game_state.player[player_number].index);
    visited[game_state.player[player_number].index] = TRUE;

	node_distance[player_number][game_state.player[player_number].index] = 0;

	while (!queue_empty(queue)) {
		int current = queue_next(queue);
		assert(current >= 0 && current < FIELD_SIZE);
		int neighbours_count = get_empty_neighbours(current, neighbour);
		for (i = 0; i < neighbours_count; i++) {
			assert(list[i] == current - 1 || list[i] == current + 1 || list[i] == current + FIELD_COLS || list[i] == current - FIELD_COLS);
			assert(neighbour[i] >= 0 && neighbour[i] < FIELD_SIZE);
			if (visited[neighbour[i]]) continue;
            visited[neighbour[i]] = TRUE;
            node_distance[player_number][neighbour[i]] = 1 + node_distance[player_number][current];
			queue_add(queue, neighbour[i]);
		}
	}
}

int voronoi_access(int index) {
	if (node_distance[0][index] == 0 && node_distance[1][index] == 0) return VORONOI_NO_ACCESS;
	if (node_distance[0][index] != 0 && node_distance[1][index] == 0) return VORONOI_PLAYER0;
	if (node_distance[0][index] == 0 && node_distance[1][index] != 0) return VORONOI_PLAYER1;
	if (node_distance[0][index] < node_distance[1][index]) return VORONOI_PLAYER0;
	if (node_distance[0][index] > node_distance[1][index]) return VORONOI_PLAYER1;
	return VORONOI_DRAW_ACCESS;
}

void prepare_initial_area(void) {
	voronoi_calculate(0);
	voronoi_calculate(1);
	memcpy(initial_area, node_distance, sizeof(initial_area));
}

int is_initial_area(int player_num, int index) {
	int opponent = player_num ^ 1;
	if (initial_area[player_num][index] == 0 && initial_area[opponent][index] == 0) return FALSE;
	if (initial_area[player_num][index] != 0 && initial_area[opponent][index] == 0) return TRUE;
	if (initial_area[player_num][index] < initial_area[opponent][index]) return TRUE;
	return FALSE;
}

int get_node_distance(int player_num, int index) {
	return node_distance[player_num][index];
}

// END

