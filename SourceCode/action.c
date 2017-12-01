/*-------------------------------------------------------------------------------
** Action: execute moves
**-----------------------------------------------------------------------------*/

#include "definitions.h"

extern GAME_SETTINGS	game_settings;

void do_moves(int time) {
	char move;

	move = find_move(time, game_settings.my_bot_id, MAX_DEPTH);

	if (move == MOVE_UP) fprintf(stdout, "up\n");
	if (move == MOVE_DOWN) fprintf(stdout, "down\n");
	if (move == MOVE_LEFT) fprintf(stdout, "left\n");
	if (move == MOVE_RIGHT) fprintf(stdout, "right\n");
}

void action(char *type, char *time) {
	assert(type != NULL);
	assert(time != NULL);

	if (!strcmp(type, "move"))
		do_moves(atoi(time));
	else
		fprintf(stderr, "action: unknown type: [%s]\n", type);
}


//END
