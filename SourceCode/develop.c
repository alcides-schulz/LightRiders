/*-------------------------------------------------------------------------------
** Light Rider Development utils
**-----------------------------------------------------------------------------*/

#ifdef DEVELOP

#include "definitions.h"
#include "queue.h"

extern GAME_STATE		game_state;
extern GAME_SETTINGS	game_settings;
extern int				node_count;

void	bench(void);
void	perf_test(void);
void	print_voronoi_diagram(void);
void	export_voronoi(int player, char *file);
void	generate_openings(void);
void	positions(void);
void	print_art_separated(int player_num);
int 	max_fill(PLAYER *player);
int		flood_fill(PLAYER *player);
void	analyse_articulations(void);
void	test_articulations(int player_num);

extern int EVAL_DEBUG;

void develop_console(void) {
	char	input[1024];
	char    i;
	char	move;
	int		turn = game_settings.my_bot_id;
	int		valid;
	char	list[5];
	int		quit = FALSE;

	//int		timebank = game_settings.timebank;

	while (!quit) {
		get_moves(&game_state.player[turn], list);
		printf("\n");
		printf("voronoi='v' bench='b' perf_test='p' openings='o' bad pos='p' evaluate='e' articulations='a'\n");
		printf("player %d moves='%s' go='.' quit='q': ", turn, list);
		fgets(input, 1024, stdin);

		for (i = 0; input[i]; i++) {
			if (input[i] == 'q') {
				quit = TRUE;
				break;
			}
			if (input[i] == 'v') {
				//int vs = evaluate_connected();
				//print_voronoi_diagram();
				//printf("voronoi score: %d\n", vs);
				//export_voronoi(0, "C:\\Users\\Alcides Schulz\\Documents\\AI\\LightRider\\files\\voronoi0.csv");
				//export_voronoi(1, "C:\\Users\\Alcides Schulz\\Documents\\AI\\LightRider\\files\\voronoi1.csv");
				continue;
			}
			if (input[i] == 'e') {
                EVAL_DEBUG = TRUE;
				printf("eval=%d end_pos: %d\n", evaluation(0), eval_end_position());
				print_art_separated(0);
				print_art_separated(1);
				if (flood_fill(&game_state.player[0]) < 16 && flood_fill(&game_state.player[1]) < 16) {
					printf("max_fill: %d\n", max_fill(&game_state.player[0]));
					printf("max_fill: %d\n", max_fill(&game_state.player[1]));
				}
				else {
					printf("flood_fill: %d\n", flood_fill(&game_state.player[0]));
					printf("flood_fill: %d\n", flood_fill(&game_state.player[1]));
				}
                test_articulations(0);
                EVAL_DEBUG = FALSE;
				continue;
			}
			if (input[i] == 'a') {
				analyse_articulations();
				continue;
			}
			if (input[i] == 'b') {
				bench();
				continue;
			}
			if (input[i] == 'p') {
				positions();
				continue;
			}
			if (input[i] == 'o') {
				generate_openings();
				continue;
			}
			if (input[i] == 'p') {
				perf_test();
				continue;
			}
			if (input[i] == '.')
				move = find_move(2000, turn, MAX_DEPTH);
			else
				if (strchr(MOVE_LIST, input[i]) == NULL)
					continue;
				else
					move = input[i];
			valid = can_move(&game_state.player[turn], move);
			printf("%d move: %c %d\n", turn, move, valid);
			if (valid) {
				make_move(&game_state.player[turn], move);
				turn ^= 1;
				print_state();
				//printf("min_row: %d max_row: %d\n", min_row, max_row);
			}
			else {
				printf(" not valid\n");
				continue;
			}
		}
	}
}

void positions(void) {
	char	line[1024];
	FILE	*f = fopen("C:\\Users\\Alcides Schulz\\Documents\\AI\\LightRider\\files\\positions-big.txt", "r");
	FILE	*o = fopen("C:\\Users\\Alcides Schulz\\Documents\\AI\\LightRider\\files\\win-eval.csv", "w");
	char	field[256];
	int		pos_count = 0;
	int		total_count = 0;

	if (f == NULL) {
		fprintf(stderr, "cannot open positions file.");
		return;
	}
    if (o == NULL) {
        fprintf(stderr, "cannot create output file.");
        return;
    }

	while (fgets(line, 1024, f) != NULL) {
		if (line[0] != '1') continue;
		
        int i = 0;
		for (char *c = &line[6]; *c; c++) {
			if (*c == ',') continue;
			if (*c == '\n') continue;
			if (i >= FIELD_SIZE) break;
			field[i++] = *c;
		}

		update_field(field);
		if (eval_end_position() != END_POS_UNDEF) continue;
		if (!players_are_connected()) continue;
		if (flood_fill(&game_state.player[0]) > 20) continue;
		if (flood_fill(&game_state.player[1]) > 20) continue;
		
		total_count++;
		int e = evaluation(0);
		if (e <= 0) continue;

		for (i = 0; line[i]; i++)
			if (line[i] == '\n') line[i] = '\0';
		fprintf(o, "%d,%d,\"update game field %s\"\n", e, players_are_connected(), &line[6]);

		pos_count++;
		
	}

	fclose(o);
	fclose(f);

	printf("eval_tune: found %d positions out of %d\n", pos_count, total_count);
}

void print_art_separated(int player_num);

int     arts[FIELD_SIZE];

void analyse_articulations(void) {
	char	line[1024];
	FILE	*f = fopen("C:\\Users\\Alcides Schulz\\Documents\\AI\\LightRider\\files\\positions-big.txt", "r");
	char	field[256];
    int     count = 0;

	if (f == NULL) {
		fprintf(stderr, "cannot open positions file.");
		return;
	}

    for (int i = 0; i < 256; i++) arts[i] = 0;

	while (fgets(line, 1024, f) != NULL) {
		int i = 0;
		for (char *c = &line[6]; *c; c++) {
			if (*c == ',') continue;
			if (*c == '\n') continue;
			if (i >= FIELD_SIZE) break;
			field[i++] = *c;
		}
		update_field(field);
		if (eval_end_position() != END_POS_UNDEF) continue;
		if (players_are_connected()) continue;
		int ff0 = flood_fill(&game_state.player[0]);
		int ff1 = flood_fill(&game_state.player[1]);
		if (ff0 < 0 || ff1 < 0) continue;
		if (ff0 > 20 || ff1 > 20) continue;

        count++;
        printf("positions: %d\r", count);

        evaluation(0);
        //print_art_separated(0);
        //my_pause("art");

		test_articulations(0);
	}

    printf("\n");

    for (int i = 0; i < 256; i++) {
        if (arts[i] == 0) continue;
        //printf("%3d 0x%02X,\n", i, i);
        printf("0x%02X,\n", i);
    }

	fclose(f);
}

void test_articulations(int player_num) {
    QUEUE	q;
    int		index = game_state.player[player_num].index;
    char	visited[FIELD_SIZE];

    memset(visited, 0, sizeof(visited));

    queue_init(&q);

    queue_add(&q, index);
    visited[index] = TRUE;

    while (!queue_empty(&q)) {
        int current = queue_next(&q);
        int neighbour[4];
        int neighbour_count = get_empty_neighbours(current, neighbour);
        for (int i = 0; i < neighbour_count; i++) {
            if (visited[neighbour[i]]) continue;
            visited[neighbour[i]] = TRUE;
            //if (articulation_is_art_node(player_num, neighbour) && square_pattern(neighbour) == 0xE0) {
            //        print_art_separated(player_num);
            //        printf("ART: %d pattern: %d test count: %d 0x%02X\n", neighbour, is_articulation(neighbour), articulation_is_art_node(player_num, neighbour), square_pattern(neighbour));
            //        my_pause("art");
            //    arts[square_pattern(neighbour)]++;
            //}
            //if (is_articulation(neighbour) == TRUE && articulation_is_art_node(player_num, neighbour) == FALSE) {
            //    print_art_separated(player_num);
            //    printf("ART: %d pattern: %d test count: %d 0x%02X\n", neighbour, is_articulation(neighbour), articulation_is_art_node(player_num, neighbour), square_pattern(neighbour));
            //    my_pause("art");
            //    arts[square_pattern(neighbour)]++;
            //}
            queue_add(&q, neighbour[i]);
        }
    }

}

void perf_test(void) {
	char	*fields[] = {
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,1,.,.,.,.,.,x,x,.,.,x,x,x,x,x,x,x,.,.,.,.,.,x,x,.,.,x,x,x,x,x,x,x,.,.,.,.,.,x,x,.,.,x,x,.,.,x,x,x,.,.,.,.,.,x,x,x,x,x,x,.,.,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,0,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,.,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,.,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,.,x,x,.,.,.,.,.,.,.,x,.,x,x,x,x,x,x,x,.,.,.,.,.,1,x,x,x,x,x,x,.,x,x,x,.,.,.,.,.,0,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,x,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,.,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,0,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,1,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,1,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,x,.,.,.,.,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,0,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,1,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,.,.,0,x,.,.,.,.,.,.,.,.,.,.,.,x,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,.,x,x,x,x,.,.,.,.,.,0,x,x,x,x,x,.,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,x,.,.,.,.,.,.,.,x,x,x,x,x,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,.,x,x,x,x,x,x,x,x,x,x,.,x,x,.,x,.,0,.,.,x,x,x,x,x,.,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,1,x,x,x,x,x,x,x,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,0,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,x,x,x,x,.,.,.,x,x,.,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,.,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,1,x,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,x,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,x,.,.,x,x,x,.,x,x,x,.,.,.,x,x,x,x,.,.,x,x,x,x,x,x,x,x,.,.,x,x,x,x,.,.,x,x,x,x,x,x,.,x,.,.,.,x,x,x,.,.,0,x,x,x,x,x,x,x,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,1,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,x,x,x,x,.,.,x,x,x,x,x,x,.,.,.,x,x,.,.,.,.,.,x,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,.,.,x,0,.,.,x,x,x,x,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,x,x,.,.,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		NULL
	};

	int			i;
	int			j;
	unsigned	start;
	int			run_count = 5;
	double		knps[5];
	int			min;
	int			max;
	double      sum = 0;
	long		total_nodes;

	for (i = 0; i < run_count; i++) {
		start = get_time();
		total_nodes = 0;
		for (j = 0; fields[j] != NULL; j++) {
			update_field(fields[j]);
			printf("run %d) pos: %d   \r", i, j + 1);
			find_move(100000, j % 2, 16);
			total_nodes += node_count;
		}
		knps[i] = (double)(total_nodes) / (double)(get_time() - start);
		//printf("run %d) knodes per second: %.2f\n", i, knps[i]);
	}

	min = max = 0;
	for (i = 1; i < run_count; i++) {
		if (knps[i] < knps[min]) min = i;
		if (knps[i] > knps[max]) max = i;
	}
	if (min == max) {
		min = 0;
		max = run_count - 1;
	}
	for (i = 0; i < run_count; i++) {
		if (i == min || i == max) continue;
		sum += knps[i];
	}

	printf("perf_test: knodes per second %.2f\n", sum / (double)(run_count - 2));
}

void bench(void) {
	char	*fields[] = {
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,1,.,.,.,.,.,x,x,.,.,x,x,x,x,x,x,x,.,.,.,.,.,x,x,.,.,x,x,x,x,x,x,x,.,.,.,.,.,x,x,.,.,x,x,.,.,x,x,x,.,.,.,.,.,x,x,x,x,x,x,.,.,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,0,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,.,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,.,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,.,x,x,.,.,.,.,.,.,.,x,.,x,x,x,x,x,x,x,.,.,.,.,.,1,x,x,x,x,x,x,.,x,x,x,.,.,.,.,.,0,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,x,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,.,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,0,x,.,.,.,.,x,.,.,.,.,.,.,.,.,.,1,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,1,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,x,.,.,.,.,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,0,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,1,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,.,.,0,x,.,.,.,.,.,.,.,.,.,.,.,x,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,x,x,x,x,x,.,x,x,x,x,.,.,.,.,.,0,x,x,x,x,x,.,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,x,.,.,.,.,.,.,.,x,x,x,x,x,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,.,x,x,x,x,x,x,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,x,.,x,x,x,x,x,x,x,x,x,x,.,x,x,.,x,.,0,.,.,x,x,x,x,x,.,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,.,.,1,x,x,x,x,x,x,x,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,0,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,x,x,x,x,.,.,.,x,x,.,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,.,x,x,x,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,1,x,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,x,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,.,x,x,.,.,.,.,x,x,x,.,.,.,.,.,.,x,x,x,.,.,.,.,x,x,x,.,.,.,x,x,x,x,x,x,.,.,.,.,x,x,x,x,.,.,x,x,x,.,x,x,x,.,.,.,x,x,x,x,.,.,x,x,x,x,x,x,x,x,.,.,x,x,x,x,.,.,x,x,x,x,x,x,.,x,.,.,.,x,x,x,.,.,0,x,x,x,x,x,x,x,.,.,.,.,.,.,.",
		".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,x,x,x,x,x,x,x,x,x,x,1,.,.,x,x,x,x,x,x,x,x,x,x,x,x,x,.,.,.,x,x,x,x,x,.,.,x,x,x,x,x,x,.,.,.,x,x,.,.,.,.,.,x,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,x,x,.,.,.,x,x,.,.,.,.,.,.,x,.,.,.,.,.,.,.,x,x,.,.,x,0,.,.,x,x,x,x,.,.,.,.,x,x,x,x,x,.,.,.,.,.,.,x,x,.,.,.,x,x,x,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
		NULL
	};

	int			i;
	long		s = 0;
	unsigned	start = get_time();
	unsigned	elapsed;
	//int			run_count = perf_test ? 5 : 1;

	for (i = 0; fields[i] != NULL; i++) {
		update_field(fields[i]);
		printf("%d: %s\n", i + 1, fields[i]);
		find_move(100000, i % 2, 16);
		s += node_count;
	}

	elapsed = get_time() - start;

	printf("\ntotal nodes %ld   elapsed milliseconds %u   knodes per second %.2f\n", s, elapsed, (double)s / (double)elapsed);
}

extern int node_distance[2][256];

char get_voronoi_value(int index) {
	if (node_distance[0][index] == 0 && node_distance[1][index] == 0) return 'x';
	if (node_distance[0][index] != 0 && node_distance[1][index] == 0) return '0';
	if (node_distance[0][index] == 0 && node_distance[1][index] != 0) return '1';
	if (node_distance[0][index] < node_distance[1][index]) return '0';
	if (node_distance[0][index] > node_distance[1][index]) return '1';
	return 'B';
}

void print_voronoi_diagram() {
	int row;
	int col;
	char item;

	int c0 = 0;
	int c1 = 0;

	for (row = 0; row < FIELD_ROWS; row++) {
		fprintf(stderr, "   ");
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
				fprintf(stderr, "%2d %c", row, 179);

			item = get_voronoi_value(to_index(row, col));

			if (item == '0') c0++;
			if (item == '1') c1++;

			fprintf(stderr, " %c %c", item, 179);
		}

		fprintf(stderr, " %2d\n", row);
	}

	fprintf(stderr, "   ");
	for (col = 0; col < FIELD_COLS; col++) {
		if (col == 0)
			fprintf(stderr, "%c", 192);
		else
			fprintf(stderr, "%c", 193);
		fprintf(stderr, "%c%c%c", 196, 196, 196);
	}
	fprintf(stderr, "%c\n", 217);
	fprintf(stderr, "c0=%d c1=%d\n", c0, c1);
}

void export_voronoi(int player, char *file) {
	FILE *fs = fopen(file, "w");
	int row;
	int col;

	for (row = 0; row < FIELD_ROWS; row++) {
		for (col = 0; col < FIELD_COLS; col++) {
			fprintf(fs, "%d", node_distance[player][to_index(row, col)]);
			if (col < FIELD_COLS - 1) fprintf(fs, ",");
		}
		fprintf(fs, "\n");
	}
	fclose(fs);
}

void my_pause(char *str) {
	char	line[100];

	if (str != NULL) {
		fprintf(stdout, "%s: ", str);
		fflush(stdout);
	}
	fgets(line, 100, stdin);
}

void make_move(PLAYER *player, char move);
void undo_move(PLAYER *player, char move);
void max_space(PLAYER *player, int *max, int ply);

void max_space(PLAYER *player, int *max, int ply) {
	char list[5];
	int i;

	if (ply > *max)	*max = ply;
	get_moves(player, list);
	for (i = 0; list[i]; i++) {
		make_move(player, list[i]);
		max_space(player, max, ply + 1);
		undo_move(player, list[i]);
	}
}

int max_fill(PLAYER *player) {
	int max = 0;

	//print_state();
	//printf("space: %d\n", space);
	//pause("start compare_space");

	max_space(player, &max, 0);

	return max;

	//if (max < space) {
	//	print_state();
	//	printf("row: %d col: %d space: %d max: %d\n", get_row(player->index), get_col(player->index), space, max);
	//	pause("compare_space");
	//}
}

int flood_fill(PLAYER *player) {
	int		neighbour[4];
	int		main_count = 0;
	int		queue[512];
	int		queue_count = 0;
	int		queue_index = 0;
	int		i;
    char    visited[FIELD_SIZE];

    memset(visited, 0, sizeof(visited));
	visited[player->index] = TRUE;
	queue[queue_count++] = player->index;

	while (queue_index < queue_count) {
		int current = queue[queue_index++];
		int neighbour_count = get_empty_neighbours(current, neighbour);
		for (i = 0; i < neighbour_count; i++) {
			if (visited[neighbour[i]]) continue;
			visited[neighbour[i]] = TRUE;
			main_count += 1;
			queue[queue_count++] = neighbour[i];
		}
	}

	return main_count;
}

#endif

//END
