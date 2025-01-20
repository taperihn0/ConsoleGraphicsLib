#include "input.h"

int main()
{
	enable_raw_mode();
	
	device_file kbd;
	open_keyboard_input_file(&kbd);

	keyboard_events kbd_ev;
	clear_keyboard_events(&kbd_ev);

    while (true) {
		poll_keyboard_events(&kbd_ev, &kbd);
		
		if (get_key(&kbd_ev, KEY_Q) == KEY_PRESSED)
			break;
		if (get_key(&kbd_ev, KEY_A) == KEY_PRESSED)
			printf("PRESSED_A__");
		if (get_key(&kbd_ev, KEY_A) == KEY_RELEASED)
			printf("RELEASED_A");
		if (get_key(&kbd_ev, KEY_D) == KEY_PRESSED) 
			printf(", PRESSED_D__");
		if (get_key(&kbd_ev, KEY_D) == KEY_RELEASED) 
			printf(", RELEASED_D");
	
		putchar('\n');
		usleep(1000);
    }	

    close_keyboard_input_file(&kbd);

    disable_raw_mode();
}