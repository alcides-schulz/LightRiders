/*-------------------------------------------------------------------------------
** Light Rider Board Evaluation
**-----------------------------------------------------------------------------*/

#include "definitions.h"
#include "queue.h"

typedef struct {
    uint64_t key;
    int     score;
}   EVAL_TABLE;

#define EVAL_TABLE_SIZE (65536 * 2)

EVAL_TABLE  eval_table[EVAL_TABLE_SIZE];
int64_t eval_table_probe = 0;
int64_t eval_table_hits = 0;

int EVAL_DEBUG = FALSE;

extern GAME_STATE		game_state;
extern GAME_SETTINGS	game_settings;

void    count_nodes(PLAYER *player, int vertex, COUNTS *main_count, int connected, char visited[]);
int     count_holes(int player_num);

void init_eval(void)
{
    memset(eval_table, 0, sizeof(eval_table));
}

void print_eval_table_stats(void)
{
    int entries = 0;

    for (int i = 0; i < EVAL_TABLE_SIZE; i++) {
        if (eval_table[i].key) entries++;
    }

    fprintf(stderr, "EVAL_TABLE: entries: %d probes: %llu hits: %llu\n", entries, eval_table_probe, eval_table_hits);
}

int evaluation(int turn) 
{
    COUNTS  counts0;
    COUNTS  counts1;
    int     score;
    char    visited[2][FIELD_SIZE];

    int     entry = game_state.key % EVAL_TABLE_SIZE;
    eval_table_probe++;
    if (eval_table[entry].key == game_state.key) {
        eval_table_hits++;
        return eval_table[entry].score;
    }
    
    memset(visited, 0, sizeof(visited));

    int connected = players_are_connected();

    counts_init(&counts0, game_state.player[0].index);
    counts_init(&counts1, game_state.player[1].index);

    if (connected) {
        voronoi_calculate(0);
        voronoi_calculate(1);

        art_calc(&game_state.player[0], connected);
        art_calc(&game_state.player[1], connected);

        count_nodes(&game_state.player[0], game_state.player[0].index, &counts0, connected, visited[0]);
        count_nodes(&game_state.player[1], game_state.player[1].index, &counts1, connected, visited[1]);

        int eval0 = counts0.edges - count_holes(0);
        int eval1 = counts1.edges - count_holes(1);

        score = eval0 - eval1;
    }
    else {
        art_calc(&game_state.player[0], connected);
        art_calc(&game_state.player[1], connected);

        count_nodes(&game_state.player[0], game_state.player[0].index, &counts0, connected, visited[0]);
        count_nodes(&game_state.player[1], game_state.player[1].index, &counts1, connected, visited[1]);

        int eval0 = counts0.nodes_adjust * 1000 + counts0.edges;
        int eval1 = counts1.nodes_adjust * 1000 + counts1.edges;

        score = eval0 - eval1;
    }

    if (turn == 1) score = -score;

    eval_table[entry].key = game_state.key;
    eval_table[entry].score = score;

    return score;
}

void count_nodes(PLAYER *player, int vertex, COUNTS *main_count, int connected, char visited[]) {
    int		neighbour[4];
    int		art_list[256];
    int		art_count = 0;
    COUNTS	sub_count;
    COUNTS	max_count;
    QUEUE   q;

    counts_init(main_count, vertex);
    queue_init(&q);

    visited[vertex] = TRUE;

    if (player->is_articulation[vertex]) {
        art_list[art_count++] = vertex;
    }
    else
    {
        queue_add(&q, vertex);
        while (!queue_empty(&q)) {
            int current = queue_next(&q);
            int neighbour_count = get_empty_neighbours(current, neighbour);
            for (int i = 0; i < neighbour_count; i++) {
                if (visited[neighbour[i]]) continue;
                visited[neighbour[i]] = TRUE;
                if (connected) {
                    int va = voronoi_access(neighbour[i]);
                    if (va == VORONOI_DRAW_ACCESS || va == player->opp_number) counts_add(main_count, neighbour[i]);
                    if (va == player->opp_number) continue;
                }
                counts_add(main_count, neighbour[i]);
                if (player->is_articulation[neighbour[i]]) {
                    art_list[art_count++] = neighbour[i];
                    continue;
                }
                queue_add(&q, neighbour[i]);
            }
        }
    }

    counts_reset(&max_count);

    for (int ac = 0; ac < art_count; ac++) {
        int current = art_list[ac];
        int neighbour_count = get_empty_neighbours(current, neighbour);
        for (int i = 0; i < neighbour_count; i++) {
            if (visited[neighbour[i]]) continue;
            visited[neighbour[i]] = TRUE;
            if (connected) {
                int va = voronoi_access(neighbour[i]);
                if (va == VORONOI_DRAW_ACCESS || va == player->opp_number) counts_add(main_count, neighbour[i]);
                if (va == player->opp_number) continue;
            }
            count_nodes(player, neighbour[i], &sub_count, connected, visited);
            counts_adjust(&sub_count);
            if (connected && (sub_count.edges * 1000 + sub_count.nodes_adjust) > (max_count.edges * 1000 + max_count.nodes_adjust)) {
                memcpy(&max_count, &sub_count, sizeof(COUNTS));
            }
            if (!connected && sub_count.nodes_adjust > max_count.nodes_adjust) {
                memcpy(&max_count, &sub_count, sizeof(COUNTS));
            }
        }
    }
    counts_combine(main_count, &max_count);
    counts_adjust(main_count);

    if (EVAL_DEBUG) printf("player: %d vertex: %d nodes: %d adjust: %d\n", player->my_number, vertex, main_count->nodes, main_count->nodes_adjust);
}

int count_holes(int player_num) {
	int	count = 0;
	for (int i = 0; i < FIELD_SIZE; i++) {
		if (!is_empty(i)) continue;
		if (!is_initial_area(player_num, i)) continue;
		count += (3 - count_empty_neighbours(i));
	}
	return count;
}

// END