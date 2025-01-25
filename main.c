#include "input.h"
#include "cursor.h"
#include "terminal.h"

void mouse_callback(int dx, int dy) {
	printf("(%d, %d)\n", dx, dy);
}

int main() {
	enable_raw_mode();
	make_terminal_fullscreen();
	set_terminal_title("TEST");
	int code = hide_cursor();

	if (code == -1)
		exit(EXIT_FAILURE);

	keyboard* kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);

	mouse* mice = malloc(sizeof(mouse));
	init_mouse(mice);
	set_pos_callback(mice, &mouse_callback);

	while (true) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);
		
		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

		//printf("RUNNING...\n");
		usleep(10000);
	}	


	close_keyboard(kbd);
	free(kbd);
	free(mice);
	
	unmake_terminal_fullscreen();
	disable_raw_mode();
	show_cursor(); 
}
