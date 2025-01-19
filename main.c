#include "input.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>

int main(void)
{
	enable_raw_mode();
	
	device_file kbd;
	open_keyboard_input_file(&kbd);

	keyboard_events kbd_ev;
	clear_keyboard_events(&kbd_ev);

    while (true) {
		poll_keyboard_events(&kbd_ev, &kbd);

		if (get_key(&kbd_ev, KEY_Q) == KEY_REPEATED)
			break;

		usleep(1000);
    }

    fflush(stdout);
    fprintf(stderr, "%s.\n", strerror(errno));	
    disable_raw_mode();

    close_keyboard_input_file(&kbd);

    return EXIT_FAILURE;
}