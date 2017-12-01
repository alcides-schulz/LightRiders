//-------------------------------------------------------------------------
//	TheAiGames's Light Riders bot by Alcides Schulz (zluchs)
//-------------------------------------------------------------------------

#include "definitions.h"

extern GAME_STATE      game_state;
extern GAME_SETTINGS   game_settings;

char    line[16384];
char    part[3][1024];

int main(void) {
	init_state();
	init_settings();
    init_eval();

#ifdef DEVELOP
	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player0,player1");
	settings("your_bot", "player0");
	settings("your_botid", "0");
	settings("field_width", "16");
	settings("field_height", "16");
	update("game", "round", "0");
	update("game", "field", ".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,0,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.");
#endif

	while (fgets(line, 16384, stdin) != NULL) {
		if (!strncmp(line, "settings ", 9)) {
			sscanf(&line[9], "%s %s", part[0], part[1]);
			settings(part[0], part[1]);
			continue;
		}
		if (!strncmp(line, "update ", 7)) {
			sscanf(&line[7], "%s %s %s", part[0], part[1], part[2]);
			update(part[0], part[1], part[2]);
			continue;
		}
		if (!strncmp(line, "action ", 7)) {
			sscanf(&line[7], "%s %s", part[0], part[1]);
			action(part[0], part[1]);
			fflush(stdout);
            //print_eval_table_stats();
			continue;
		}

#ifdef DEVELOP
		if (!strncmp(line, "stderr ", 7)) {
			sscanf(&line[7], "%s", part[0]);
			freopen(part[0], "w", stderr);
			continue;
		}
		if (!strncmp(line, "q", 1))
			exit(0);
		if (!strncmp(line, "d", 1))
			print_state();
		if (!strncmp(line, "test", 4))
			develop_console();
#endif

	}

    return 0;

}

// END
