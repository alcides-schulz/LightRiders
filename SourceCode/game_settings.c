/*-------------------------------------------------------------------------------
** Game settings update
**-----------------------------------------------------------------------------*/

#include "definitions.h"

GAME_SETTINGS	game_settings;


void init_settings(void) {
	memset(&game_settings, 0, sizeof(GAME_SETTINGS));
}

void settings(char *type, char *value) {

	assert(type != NULL);
	assert(value != NULL);

	if (!strcmp(type, "timebank"))
		game_settings.timebank = atoi(value);
	else
		if (!strcmp(type, "time_per_move"))
			game_settings.time_per_move = atoi(value);
		else
			if (!strcmp(type, "player_names"))
				strcpy(game_settings.player_names, value);
			else
				if (!strcmp(type, "your_bot"))
					strcpy(game_settings.my_bot_name, value);
				else
					if (!strcmp(type, "your_botid"))
						game_settings.my_bot_id = atoi(value);
					else
						if (!strcmp(type, "field_height"))
							game_settings.field_height = atoi(value);
						else
							if (!strcmp(type, "field_width"))
								game_settings.field_width = atoi(value);
							else
								fprintf(stderr, "settings: unknown type: [%s]\n", type);
}

int get_row(int index) {
	assert(index >= 0 && index < field_size());
	return index / FIELD_ROWS;
}

int get_col(int index) {
	assert(index >= 0 && index < field_size());
	return index % FIELD_COLS;
}

int to_index(int row, int col) {
	return row * FIELD_ROWS + col;
}

int distance(int index1, int index2) {
	int row1 = get_row(index1);
	int col1 = get_col(index1);
	int row2 = get_row(index2);
	int col2 = get_col(index2);
	return abs(row1 - row2) + abs(col1 - col2);
}
//END
