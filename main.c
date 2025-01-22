#include "input.h"
#include "cursor.h"

int main() {
	enable_raw_mode();
	
	device_file kbd;
	open_keyboard_input_file(&kbd);

	keyboard_events kbd_ev;
	clear_keyboard_events(&kbd_ev);

	int code = hide_cursor();

	if (code == -1)
		exit(EXIT_FAILURE);
	
	while (true) {
		poll_keyboard_events(&kbd_ev, &kbd);
		
		if (get_key(&kbd_ev, KEY_Q) == KEY_PRESSED)
			break;

		printf("RUNNING...\n");
		usleep(10000);
	}	

	show_cursor();

	close_keyboard_input_file(&kbd);

	disable_raw_mode();
}
