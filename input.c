#include "input.h"
#include "terminal.h"
#include "dev.h"
#include "thread.h"
#include <fcntl.h>
#include <libusb-1.0/libusb.h>
#include <libudev.h>

#define _INTERFACE_KEY_RELEASED 0
#define _INTERFACE_KEY_PRESSED  1
#define _INTERFACE_KEY_REPEATED 2

#define _HANDLER_PATH_SIZE 32
#define _HANDLER_DIR_PATH  "/dev/input/"

#define _FILE_KEYBOARD    0
#define _FILE_MOUSE       1

#define _MAX_DEVICE_NUM   8
#define _MAX_MOUSE_NUM    8
#define _MAX_KEYBOARD_NUM 8

typedef int _dev_filetype_t;

typedef struct _device {
	_dev_filetype_t type;
	int fd;
} _device;

typedef struct _device_array {
	_device tab[_MAX_DEVICE_NUM];
	size_t cnt;
} _device_array;

static _device_array devs;

int _add_device(_dev_filetype_t type, int fd) {
	if (devs.cnt >= _MAX_DEVICE_NUM)
		return -1;
	devs.tab[devs.cnt++] = (_device){ type, fd };
	return 0;
}

#define _MOUSE_EV      1 			  // event.dev
#define _TOUCHPAD_EV   2 			  // event.dev
#define _KEYBOARD_EV   3 			  // event.dev

#define _BTN_ACT 	 	  KEY_MAX + 1 // event.action
#define _MOVE_X_ACT 	  KEY_MAX + 2 // event.action
#define _MOVE_Y_ACT 	  KEY_MAX + 3 // event.action 

/*
	Wrapper around raw linux struct input_event
*/

typedef struct _event_t {
	int dev;
	int action;
	int value;
} _event;

/*
	MAIN EVENT PROCESSING QUEUE
*/

#define _QUEUE_SIZE_PER_DEVTYPE 512

typedef struct _event_queue {
	_event buff[_QUEUE_SIZE_PER_DEVTYPE];
	uint16_t ind;
	uint16_t end;
} _event_queue;

static _event_queue ev_queue;

int _pop_next_event(_event** ev) {
	if (ev_queue.ind == ev_queue.end) {
		*ev = NULL;
		return 0;
	}
	*ev = &ev_queue.buff[ev_queue.ind];
	ev_queue.ind = (ev_queue.ind + 1) & (_QUEUE_SIZE_PER_DEVTYPE - 1);
	return 1;
}

void _push_next_event(_event* ev) {
	ev_queue.buff[ev_queue.ind] = *ev;
	ev_queue.ind = (ev_queue.ind + 1) & (_QUEUE_SIZE_PER_DEVTYPE - 1);
}

typedef struct _event_thread_ctx {
	pthread_t* thread;
	_mutex_t wait;
	_mutex_t suspended;
} _event_thread_ctx;

_event_thread_ctx _event_writer_ctx = {
	.thread = NULL
};

#define _process_msg_from_main_thread(msg) 							 	\
	byte rest;															   	 	\
	_write_mutex_data(&_event_writer_ctx.suspended, &byte_true); 	\
	do { 																	   	 	\
		_read_mutex_data(&_event_writer_ctx.wait, &rest); 			 	\
	} while (rest); 													   	 	\
	_write_mutex_data(&_event_writer_ctx.suspended, &byte_false);  \

extern int _read_event_mouse(int fd, _event* ev);
extern int _read_event_keyboard(int fd, _event* ev);

void* _event_write_loop(_UNUSED void* args) {
	byte msg;
	_event ev = { 0 };

	while (true) {
		for (size_t i = 0; i < devs.cnt; i++) {
			_read_mutex_data(&_event_writer_ctx.wait, &msg);
			
			if (msg) {
				_process_msg_from_main_thread(msg);
			}

			int ret;

			if (devs.tab[i].type == _FILE_MOUSE)
				ret = _read_event_mouse(devs.tab[i].fd, &ev);
			else if (devs.tab[i].type == _FILE_KEYBOARD)
				ret = _read_event_keyboard(devs.tab[i].fd, &ev);

			if (!ret)
				_push_next_event(&ev);
		}
	}

	return NULL;
}

void _init_writer_ctx() {
	_init_mutex(&_event_writer_ctx.wait, &byte_false, sizeof(byte));
	_init_mutex(&_event_writer_ctx.suspended, &byte_false, sizeof(byte));
	_event_writer_ctx.thread = _get_thread();
	pthread_create(_event_writer_ctx.thread, NULL, _event_write_loop, NULL);
}

void _close_writer_ctx() {
	_close_mutex(&_event_writer_ctx.wait);
	_close_mutex(&_event_writer_ctx.suspended);
}

#define _suspend_event_writer() 										\
	_write_mutex_data(&_event_writer_ctx.wait, &byte_true);  \
	byte msg; 																\
	do { 																		\
		_read_mutex_data(&_event_writer_ctx.suspended, &msg); \
	} while (!msg); 														\


#define _run_event_writer() 											\
	_write_mutex_data(&_event_writer_ctx.wait, &byte_false); \

void poll_events() {
	_suspend_event_writer();

	_event* ev;

	while (_pop_next_event(&ev)) {
		// if not null then process *mouse_ev and *kbd_ev
	}
	
	_run_event_writer();
}

int _get_input_file_of(_dev_filetype_t devtype, char* path) {
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

	_get_input_file_of(_FILE_KEYBOARD, path);
	if (_open_device_input_file(&keyboard->device_file, path)) {
		fprintf(stderr, "Cannot open device input file: %s\n", path);
		return;
	}

	_add_device(_FILE_KEYBOARD, keyboard->device_file.fd);

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

int _read_event_keyboard(int fd, _event* dst) {
	*dst = (_event) { 0 };

	struct input_event ev;
	ssize_t n = read(fd, &ev, sizeof(ev));
	
	if (n == (ssize_t)-1) {
		if (errno == EINTR) return -1;
      return -2;
   } else if (n == 0)
		return -2;
	else if (n != sizeof(ev))
		return -1;

	if (ev.type == EV_KEY && 
	    ev.value >= _INTERFACE_KEY_RELEASED && 
	    ev.value <= _INTERFACE_KEY_REPEATED)
	{
		dst->dev = _KEYBOARD_EV;
		dst->action = ev.code;
		dst->value = _get_key_state(ev.value);
		return 0;
	}

	return -3;
}

int get_key(keyboard* keyboard, int key) {
	return _get_key_from_events(&keyboard->events, key);
}

void close_keyboard(keyboard* keyboard) {
	_close_keyboard_input_file(&keyboard->device_file);
}

void init_mouse(mouse* mouse) {
	char path[_HANDLER_PATH_SIZE];

	_get_input_file_of(_FILE_MOUSE, path);
	if (_open_device_input_file(&mouse->device_file, path)) {
		fprintf(stderr, "Cannot open device file: %s\n", path);
		return;
	}

	_add_device(_FILE_MOUSE, mouse->device_file.fd);

	mouse->pos_callback_func = NULL;
	mouse->btn_callback_func = NULL;
}

#define _TOUCHPAD_CONTINUE_DIST 500

void poll_events_mouse(mouse* mouse) {
	ssize_t n;
	struct input_event ev;

	ASSERT(mouse->pos_callback_func != NULL && mouse->btn_callback_func != NULL, 
	       "Polling mouse events with null callback");

	const bool focus = _check_focus();
	
	// touchpad specific
	static int touchpad_px = -_TOUCHPAD_CONTINUE_DIST;
	static int touchpad_py = -_TOUCHPAD_CONTINUE_DIST;
	static bool touchpad_first = true;
	// general for every mouse-like stering device
	int dx, dy;

	while (true) {
		n = read(mouse->device_file.fd, &ev, sizeof(ev));

		if (n == (ssize_t)-1) {
			if (errno == EINTR) continue;
			else break;
		} else if (n == 0)
			break;
		else if (n != sizeof(ev))
			continue;

		if (!focus) continue;
		
		if (ev.type == EV_ABS && mouse->pos_callback_func != NULL) {
			dx = 0;
			dy = 0;

			if (ev.code == ABS_X) {
				touchpad_first = abs(touchpad_px - ev.value) > _TOUCHPAD_CONTINUE_DIST;
				dx = touchpad_first ? 0 : ev.value - touchpad_px;
				touchpad_px = ev.value;
			}
			else if (ev.code == ABS_Y) {
				touchpad_first = abs(touchpad_py - ev.value) > _TOUCHPAD_CONTINUE_DIST;
				dy = touchpad_first ? 0 : ev.value - touchpad_py;
				touchpad_py = ev.value;
			}

			touchpad_first = false;
			(*mouse->pos_callback_func)(dx, dy);
		} else if (ev.type == EV_REL && mouse->btn_callback_func != NULL) {
			dx = 0;
			dy = 0;
			
			if (ev.code == REL_X)
				dx = ev.value;
			else if (ev.code == REL_Y)
				dy = ev.value;

			(*mouse->pos_callback_func)(dx, dy);
		} else if (ev.type == EV_KEY) {
			(*mouse->btn_callback_func)(_get_btn_state(ev.value), ev.code);
		}
	}
}

int _read_event_mouse(int fd, _event* dst) {
	*dst = (_event) { 0 };

	int touchpad_px = -_TOUCHPAD_CONTINUE_DIST;
	int touchpad_py = -_TOUCHPAD_CONTINUE_DIST;
	static bool touchpad_first = true;

	struct input_event ev;
	ssize_t n = read(fd, &ev, sizeof(ev));

	if (n == (ssize_t)-1) {
		if (errno == EINTR) return -2;
		return -1;
	} else if (n == 0)
		return -1;
	else if (n != sizeof(ev))
		return -2;

	if (ev.type == EV_ABS) {
		dst->dev = _KEYBOARD_EV;

		if (ev.code == ABS_X) {
			touchpad_first = abs(touchpad_px - ev.value) > _TOUCHPAD_CONTINUE_DIST;
			dst->value = touchpad_first ? 0 : ev.value - touchpad_px;
			touchpad_px = ev.value;
			dst->action = _MOVE_X_ACT;
		}
		else if (ev.code == ABS_Y) {
			touchpad_first = abs(touchpad_py - ev.value) > _TOUCHPAD_CONTINUE_DIST;
			dst->value = touchpad_first ? 0 : ev.value - touchpad_py;
			touchpad_py = ev.value;
			dst->action = _MOVE_Y_ACT;
		}

		touchpad_first = false;
		return 0;
	} else if (ev.type == EV_REL) {
		dst->dev = _KEYBOARD_EV;

		if (ev.code == REL_X)
			dst->action = _MOVE_X_ACT;
		else if (ev.code == REL_Y)
			dst->action = _MOVE_Y_ACT;
		
		dst->value = ev.value;
		return 0;
	} else if (ev.type == EV_KEY) {
		dst->dev = _KEYBOARD_EV;
		dst->action = _BTN_ACT;
		dst->value = _get_btn_state(ev.value);
		return 0;
	}

	return -3;
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