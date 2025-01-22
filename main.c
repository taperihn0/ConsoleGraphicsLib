#include "input.h"
#include "cursor.h"
#include "terminal.h"

int main() {
	enable_raw_mode();

	make_terminal_fullscreen();
	
	keyboard* const kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);

	int code = hide_cursor();

	if (code == -1)
		exit(EXIT_FAILURE);
	
	while (true) {
		poll_events(kbd);
		
		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

		printf("RUNNING...\n");
		usleep(10000);
	}	

	show_cursor();

	close_keyboard(kbd);
	free(kbd);

	disable_raw_mode();
}
