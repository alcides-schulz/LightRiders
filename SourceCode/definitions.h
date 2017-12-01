#pragma once

/*-------------------------------------------------------------------------------
** variables definition
**-----------------------------------------------------------------------------*/

#define NDEBUG

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#if defined(_MSC_VER)

typedef   signed __int8    int8_t;
typedef unsigned __int8   uint8_t;
typedef   signed __int16  int16_t;
typedef unsigned __int16 uint16_t;
typedef   signed __int32  int32_t;
typedef unsigned __int32 uint32_t;
typedef   signed __int64  int64_t;
typedef unsigned __int64 uint64_t;

#else

#include <inttypes.h>

#endif

#define TRUE	1
#define FALSE	0

#define MOVE_NONE	'-'
#define MOVE_UP		'u'
#define MOVE_DOWN	'd'
#define MOVE_RIGHT	'r'
#define MOVE_LEFT	'l'

#define MOVE_LIST	"udrl"

#define EMPTY	'.'
#define WALL	'x'
#define PLAYER0	'0'
#define PLAYER1	'1'

#define FIELD_ROWS	16
#define FIELD_COLS	16
#define FIELD_SIZE	256

#define MAX_DEPTH	128

#define END_POS_DRAW	0
#define END_POS_WIN0	1
#define END_POS_WIN1	2
#define END_POS_UNDEF	3

#define INF		2000000000
#define WIN		1000000000
#define EVAL	(WIN - 1000)

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#define VORONOI_PLAYER0		 0
#define VORONOI_PLAYER1		 1
#define VORONOI_NO_ACCESS	-1
#define VORONOI_DRAW_ACCESS	-2

typedef struct s_game_settings {
	int     timebank;
	int     time_per_move;
	char    player_names[512];
	char    my_bot_name[512];
	int		my_bot_id;
	int     field_height;
	int     field_width;
}   GAME_SETTINGS;

typedef struct s_player {
	char	id;
    int     my_number;
    int     opp_number;
	int		index;
	//int		count;
    char    is_articulation[FIELD_SIZE];
    int     voronoi_dist[FIELD_SIZE];
}   PLAYER;

typedef struct s_game_state {
	int     round;
	char	field[FIELD_SIZE];
	PLAYER	player[2];
    int     turn;
    uint64_t key;
}   GAME_STATE;

typedef struct s_counts {
	int		start_node;
	int		nodes;
	int		edges;
	int		reds;
	int		blacks;
	int		nodes_adjust;
}	COUNTS;


void	init_state(void);
void	update(char *player, char *type, char *value);
void	update_field(char *value);

void	init_settings(void);
void	settings(char *type, char *value);

int		is_empty(int index);
int		get_row(int index);
int		get_col(int index);
int		to_index(int row, int col);
int		distance(int index1, int index2);

int 	get_empty_neighbours(int index, int neighbour[]);
int		count_empty_neighbours(int index);
void	get_neighbours(int index, int *list);
void	get_moves(PLAYER *player, char *list);
int		can_move(PLAYER *player, char move);
void	print_state(void);
void	develop_console(void);
void	action(char *type, char *time);
char	find_move(int time, int turn, int max_depth);
void	make_move(PLAYER *player, char move);
int		eval_end_position(void);
int 	evaluation(int turn);
int		players_are_connected(void);
int		flood_fill(PLAYER *player);
int		get_node_distance(int player_num, int index);

void    counts_reset(COUNTS *counts);
void	counts_init(COUNTS *count, int index);
int		counts_is_red(int index);
void	counts_add(COUNTS *counts, int index);
void	counts_combine(COUNTS *counts, COUNTS *other);
void	counts_adjust(COUNTS *counts);
void	counts_print_board(void);

unsigned int get_time(void);
void	my_pause(char *str);

void    art_calc(PLAYER *player, int connected);

void	print_art_separated(int player_num);

void	voronoi_calculate(int player_number);
int		voronoi_access(int index);
void	prepare_initial_area(void);
int		is_initial_area(int player_num, int index);

uint64_t calc_zkey(int turn);
uint64_t zkey_value(int index);
uint64_t zkey_turn(int turn);

void    print_eval_table_stats(void);
void    init_eval(void);

//END
