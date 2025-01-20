#include "input.h"

#include <termios.h>
#include <fcntl.h>

#define _INTERFACE_KEY_RELEASED 0
#define _INTERFACE_KEY_PRESSED  1
#define _INTERFACE_KEY_REPEATED 2

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);	
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disable_raw_mode() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
	// go back to canonical mode, VMIN and VTIME are ignored since then
    original.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

int open_keyboard_input_file(device_file* kbd) {
	kbd->file = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    kbd->fd = open(kbd->file, O_RDONLY);
    kbd->flags = fcntl(kbd->fd, F_GETFL);

    if (kbd->fd == -1) {
    	fprintf(stderr, "Cannot open %s: %s", kbd->file, strerror(errno));
    	return -1;
    }

    fcntl(kbd->fd, F_SETFL, kbd->flags | O_NONBLOCK);
    return 0;
}

int close_keyboard_input_file(device_file* kbd) {
	int code = close(kbd->fd);
	
	if (code == -1)
		fprintf(stderr, "Cannot close %s: %s", kbd->file, strerror(errno));
	
	fcntl(kbd->fd, F_SETFL, kbd->flags & ~O_NONBLOCK);

	return code; 
}

void clear_keyboard_events(keyboard_events* kev) {
	for (UINT i = 0; i < KEY_CNT; i++)
		kev->key_map[i] = KEY_RELEASED;
}

_FORCE_INLINE int _get_key_state(int value) {
	return (bool)value;
}

void poll_keyboard_events(keyboard_events* kev, device_file* kbd) {
	ssize_t n;
	struct input_event ev;
	
	while (true) {
		n = read(kbd->fd, &ev, sizeof(ev));

		if (n <= 0)
			break;
		else if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
            else break;
		} else if (n != sizeof(ev))
			continue;

		if (ev.type == EV_KEY && 
			ev.value >= _INTERFACE_KEY_RELEASED && 
			ev.value <= _INTERFACE_KEY_REPEATED)
		{
			kev->key_map[ev.code] = _get_key_state(ev.value);
		}
	}	
}

int get_key(keyboard_events* kev, int key) {
	return kev->key_map[key];
}

#ifdef DEBUG
void test_input(keyboard_events* kev) {
	static const char* const ev_to_str[3] = {
		"RELEASED",
		"PRESSED",
		"REPEATED"
	};

	printf("KEYBOARD_EVENTS_BUFFER: ");

	for (int k = 0; k <= KEY_MAX; k++) {
		if (kev->key_map[k] != KEY_INVALID) {
			printf("%s_%d, ", ev_to_str[kev->key_map[k]], (int)k);
		}
	}

	putchar('\n');
}
#endif