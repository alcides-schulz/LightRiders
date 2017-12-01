/*-------------------------------------------------------------------------------
** Light Rider - Articulation Points
**-----------------------------------------------------------------------------*/

#include "definitions.h"
#include "queue.h"

typedef struct {
    int16_t	num;
    int16_t	low;
    int16_t	parent;
}	NODE_INFO;

static int         dfs_count;
static NODE_INFO   node_info[FIELD_SIZE];
static int8_t      visited[FIELD_SIZE];

void art_dfs(PLAYER *player, int vertex, int connected);

void art_calc(PLAYER *player, int connected) {
    dfs_count = 0;
    memset(&node_info, 0, sizeof(node_info));
    memset(player->is_articulation, 0, sizeof(player->is_articulation));
    memset(visited, 0, sizeof(visited));
    art_dfs(player, player->index, connected);
}

void art_dfs(PLAYER *player, int vertex, int connected) {
    int	children = 0;
    
    dfs_count++;
    node_info[vertex].low = dfs_count;
    node_info[vertex].num = dfs_count;
    visited[vertex] = TRUE;

    int neighbour[4];
    int neighbour_count = get_empty_neighbours(vertex, neighbour);

    for (int i = 0; i < neighbour_count; i++) {
        if (connected && voronoi_access(neighbour[i]) == player->opp_number) continue;
        if (visited[neighbour[i]] == FALSE) {
            children++;
            node_info[neighbour[i]].parent = node_info[vertex].num;
            art_dfs(player, neighbour[i], connected);
            if (node_info[neighbour[i]].low < node_info[vertex].low)
                node_info[vertex].low = node_info[neighbour[i]].low;
            if (node_info[vertex].parent == 0 && children > 1)
                player->is_articulation[vertex] = TRUE;
            if (node_info[vertex].parent != 0 && node_info[neighbour[i]].low > node_info[vertex].num)
                player->is_articulation[vertex] = TRUE;
        }
        else {
            if (node_info[vertex].parent != node_info[neighbour[i]].num) {
                if (node_info[neighbour[i]].num < node_info[vertex].low)
                    node_info[vertex].low = node_info[neighbour[i]].num;
            }
        }
    }
}

void print_art_separated(int player_num) {
	int     row;
	int     col;
	char	item = ' ';

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
			if (item == EMPTY) item = ' ';
		}

		fprintf(stderr, " %3d\n", row * FIELD_COLS + 15);
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

}

// END
