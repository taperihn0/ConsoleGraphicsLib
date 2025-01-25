#include "input.h"
#include <fcntl.h>

#define _INTERFACE_KEY_RELEASED 0
#define _INTERFACE_KEY_PRESSED  1
#define _INTERFACE_KEY_REPEATED 2

int _open_keyboard_input_file(_device_file* kbd, const char* path) {
	kbd->file = path;
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

int _get_key_from_events(_keyboard_events* kev, int key) {
	return kev->key_map[key];
}

void init_keyboard(keyboard* keyboard) {
	_open_keyboard_input_file(&keyboard->device_file, "/dev/input/event3");
	_clear_keyboard_events(&keyboard->events);
}

void clear_events_keyboard(keyboard* keyboard) {
	_clear_keyboard_events(&keyboard->events);
}

void poll_events_keyboard(keyboard* keyboard) {
	ssize_t n;
	struct input_event ev;
	
	while (true) {
		n = read(keyboard->device_file.fd, &ev, sizeof(ev));

		if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
            else break;
        } else if (n == 0) {
			break;
		} else if (n != sizeof(ev))
			continue;

		if (ev.type == EV_KEY && 
			ev.value >= _INTERFACE_KEY_RELEASED && 
			ev.value <= _INTERFACE_KEY_REPEATED)
		{
			keyboard->events.key_map[ev.code] = _get_key_state(ev.value);
		}
	}	
}

int get_key(keyboard* keyboard, int key) {
	return _get_key_from_events(&keyboard->events, key);
}

void close_keyboard(keyboard* keyboard) {
	_close_keyboard_input_file(&keyboard->device_file);
}

void init_mouse(mouse* mouse) {
	_open_keyboard_input_file(&mouse->device_file, "/dev/input/event4");
	mouse->callback_func = NULL;
}

void poll_events_mouse(mouse* mouse) {
	ssize_t n;
	struct input_event ev;

	if (mouse->callback_func == NULL)
		return;

	while (true) {
		n = read(mouse->device_file.fd, &ev, sizeof(ev));

		if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
            else break;
		} else if (n == 0) {
			break;
		} else if (n != sizeof(ev))
			continue;
		
		if (ev.type == EV_REL) 
		{
			if (ev.code == REL_X) 
				(*mouse->callback_func)(ev.value, 0u);
			else if (ev.code == REL_Y)
				(*mouse->callback_func)(0u, ev.value);
		} //else if (ev.type == EV_KEY) {
			//if (ev.value == _INTERFACE_KEY_PRESSED)
				// get pressed button code
			//else if (ev.value == _INTERFACE_KEY_RELEASED)
				// get released button code
		//}
	}
}

void set_pos_callback(mouse* mouse, mouse_callback_t callback_func) {
	mouse->callback_func = callback_func;
}

void close_mouse(mouse* mouse) {
	_close_keyboard_input_file(&mouse->device_file);
}