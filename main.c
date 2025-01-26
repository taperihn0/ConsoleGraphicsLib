#include "input.h"
#include "cursor.h"
#include "terminal.h"

void mouse_callback(int dx, int dy) {
	printf("(%d, %d)\n", dx, dy);
}

void button_callback(unsigned short btn_action, int btn) {
	if (btn_action == KEY_PRESSED && btn == BTN_LEFT)
		printf("PRESSED LEFT MOUSE BUTTON\n");
	else if (btn_action == KEY_PRESSED && btn == BTN_RIGHT)
		printf("PRESSED RIGHT MOUSE BUTTON\n");
}

int main() {
	enable_raw_mode();
	enable_focus_events();
	//make_terminal_fullscreen();
	set_terminal_title("ASCIIGRAPHICS");
	int code = hide_cursor();

	if (code == -1)
		exit(EXIT_FAILURE);

	keyboard* kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);

	mouse* mice = malloc(sizeof(mouse));
	init_mouse(mice);
	set_pos_callback(mice, &mouse_callback);
	set_button_callback(mice, &button_callback);
	
	unsigned long long cnt = 0;

	while (true) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);
		
		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

		//printf("RUNNING... %llu\n", cnt++);
		usleep(100);
	}

	close_keyboard(kbd);
	free(kbd);
	free(mice);
	
	unmake_terminal_fullscreen();
	disable_raw_mode();
	disable_focus_events();
	show_cursor();
}
