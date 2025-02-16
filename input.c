#include "input.h"
#include "terminal.h"
#include "dev.h"
#include <fcntl.h>
#include <libusb-1.0/libusb.h>
#include <libudev.h>

#define _INTERFACE_KEY_RELEASED 0
#define _INTERFACE_KEY_PRESSED  1
#define _INTERFACE_KEY_REPEATED 2

#define _HANDLER_PATH_SIZE 		32
#define _HANDLER_DIR_PATH 		"/dev/input/"

#define _FILE_KEYBOARD			0
#define _FILE_MOUSE				1

typedef int _dev_filetype_t;

int _get_input_file_of(_dev_filetype_t devtype, char* path, size_t n) {
	_devices_file* f = fopen(_DEVICES_FILEPATH, "r");
	_dev_simple dev, kbd;
	bool found = false;

	strcpy(path, _HANDLER_DIR_PATH);
	size_t ncat = _HANDLER_PATH_SIZE - strlen(_HANDLER_DIR_PATH);
	
	while (_next_device(f, &dev)) {
		if (dev.input_num != _PRIMARY_INPUT_NUM) 
			continue;
		else if (devtype == _FILE_KEYBOARD && !_is_keyboard_device(&dev))
			continue;
		else if (devtype == _FILE_MOUSE && !_is_mouse_device(&dev))
			continue;

		if (dev.usb) {
			strncat(path, dev.handler, ncat);
			return 0;
		}

		kbd = dev;
		found = true;
	}

	fclose(f);

	if (!found)
		return -1;

	strncat(path, kbd.handler, ncat);
	return 0;
}

int _open_device_input_file(_device_file* kbd, char* path) {
	kbd->file = path;
    kbd->fd = open(kbd->file, O_RDONLY);

    if (kbd->fd == -1) {
    	fprintf(stderr, "Cannot open %s: %s\n", kbd->file, strerror(errno));
    	return -1;
    }

    kbd->flags = fcntl(kbd->fd, F_GETFL);

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

_FORCE_INLINE int _get_btn_state(int value) {
	return (bool)value;
}

int _get_key_from_events(_keyboard_events* kev, int key) {
	return kev->key_map[key];
}

void init_keyboard(keyboard* keyboard) {
	char path[_HANDLER_PATH_SIZE];

	_get_input_file_of(_FILE_KEYBOARD, path, _HANDLER_PATH_SIZE);
	_open_device_input_file(&keyboard->device_file, path);
	_clear_keyboard_events(&keyboard->events);
}

void clear_events_keyboard(keyboard* keyboard) {
	_clear_keyboard_events(&keyboard->events);
}

void poll_events_keyboard(keyboard* keyboard) {
	ssize_t n;
	struct input_event ev;

	const bool focus = _check_focus();

	while (true) {
		n = read(keyboard->device_file.fd, &ev, sizeof(ev));
				
		if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
            else break;
        } else if (n == 0) {
			break;
		} else if (n != sizeof(ev))
			continue;

		if (!focus) continue;

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
	char path[_HANDLER_PATH_SIZE];

	_get_input_file_of(_FILE_MOUSE, path, _HANDLER_PATH_SIZE);
	_open_device_input_file(&mouse->device_file, path);

	mouse->pos_callback_func = NULL;
	mouse->btn_callback_func = NULL;
}

void poll_events_mouse(mouse* mouse) {
	ssize_t n;
	struct input_event ev;

	static const char* const null_pol_err = "Polling mouse events with null callback";

	ASSERT(mouse->pos_callback_func != NULL, null_pol_err);
	ASSERT(mouse->btn_callback_func != NULL, null_pol_err);

	const bool focus = _check_focus();

	while (true) {
		n = read(mouse->device_file.fd, &ev, sizeof(ev));

		if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
            else break;
		} else if (n == 0) {
			break;
		} else if (n != sizeof(ev))
			continue;

		if (!focus) continue;
		
		if (ev.type == EV_REL) 
		{
			if (ev.code == REL_X) 
				(*mouse->pos_callback_func)(ev.value, 0u);
			else if (ev.code == REL_Y)
				(*mouse->pos_callback_func)(0u, ev.value);
		} else if (ev.type == EV_KEY) {
			(*mouse->btn_callback_func)(_get_btn_state(ev.value), ev.code);
		}
	}
}

void set_pos_callback(mouse* mouse, mouse_pos_callback_t pos_callback_func) {
	mouse->pos_callback_func = pos_callback_func;
}

void set_button_callback(mouse* mouse, mouse_button_callback_t callback_func) {
	mouse->btn_callback_func = callback_func;
}

void close_mouse(mouse* mouse) {
	_close_keyboard_input_file(&mouse->device_file);
}