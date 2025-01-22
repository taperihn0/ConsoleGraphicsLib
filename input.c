#include "input.h"
#include <fcntl.h>

#define _INTERFACE_KEY_RELEASED 0
#define _INTERFACE_KEY_PRESSED  1
#define _INTERFACE_KEY_REPEATED 2

int _open_keyboard_input_file(_device_file* kbd) {
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

int _close_keyboard_input_file(_device_file* kbd) {
	int code = close(kbd->fd);
	
	if (code == -1)
		fprintf(stderr, "Cannot close %s: %s", kbd->file, strerror(errno));
	
	fcntl(kbd->fd, F_SETFL, kbd->flags & ~O_NONBLOCK);
	return code; 
}

void _clear_keyboard_events(_keyboard_events* kev) {
	for (UINT i = 0; i < KEY_CNT; i++)
		kev->key_map[i] = KEY_RELEASED;
}

_FORCE_INLINE int _get_key_state(int value) {
	return (bool)value;
}

void _poll_keyboard_events(_keyboard_events* kev, _device_file* kbd) {
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

int _get_key_from_events(_keyboard_events* kev, int key) {
	return kev->key_map[key];
}

void init_keyboard(keyboard* keyboard) {
	_open_keyboard_input_file(&keyboard->device_file);
	_clear_keyboard_events(&keyboard->events);
}

void clear_events(keyboard* keyboard) {
	_clear_keyboard_events(&keyboard->events);
}

void poll_events(keyboard* keyboard) {
	_poll_keyboard_events(&keyboard->events, &keyboard->device_file);
}

int get_key(keyboard* keyboard, int key) {
	return _get_key_from_events(&keyboard->events, key);
}

void close_keyboard(keyboard* keyboard) {
	_close_keyboard_input_file(&keyboard->device_file);
}