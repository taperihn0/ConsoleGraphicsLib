#include "input.h"
#include "cursor.h"
#include "terminal.h"
#include "render.h"
#include <locale.h>

void mouse_callback(int dx, int dy) {
	/*printf("(%d, %d)\n", dx, dy);*/
}

void button_callback(unsigned short btn_action, int btn) {
	/*
	if (btn_action == KEY_PRESSED && btn == BTN_LEFT)
		printf("PRESSED LEFT MOUSE BUTTON\n");
	else if (btn_action == KEY_PRESSED && btn == BTN_RIGHT)
		printf("PRESSED RIGHT MOUSE BUTTON\n");
	*/
}

int main() {
	init_terminal_state();

	enable_raw_mode();
	enable_focus_events();
	make_terminal_fullscreen();
	set_terminal_title("ASCIIGRAPHICS");
	hide_cursor();

	keyboard* kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);

	mouse* mice = malloc(sizeof(mouse));
	init_mouse(mice);
	set_pos_callback(mice, &mouse_callback);
	set_button_callback(mice, &button_callback);

	set_framerate_limit(10);
	
	int i = 0;
	setlocale(LC_ALL, "");
	
	while (!should_quit()) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);

		clear_terminal((CHAR_T)'.');//0x00002588);
		_terminal.buff.mem[0] = 'X';

		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

		flush_terminal();
		/*
		wchar_t* mem = malloc(sizeof(wchar_t) * 10);
		
		for (int i = 0; i < 10; i++)
			mem[i] = 0x00002588;

		mem[9] = 0;

		printf("%S\n", mem);

		free(mem);*/
	}

	close_keyboard(kbd);
	free(kbd);
	free(mice);
	
	unmake_terminal_fullscreen();
	disable_raw_mode();
	disable_focus_events();
	show_cursor();

	close_terminal_state();
}
