#pragma once

#include "common.h"
#include <linux/input.h>

#define KEY_INVALID  -1
#define KEY_RELEASED  0
#define KEY_PRESSED   1

/*
	GENERAL DEVICE FILE HANDLER.
	Implemented for internal usage.
	Contains path to device file in /dev/input/eventX
	For futher reading on Linux event handling:
	https://www.kernel.org/doc/html/latest/input/input.html#detailed-description
*/

typedef struct _device_file {
	const char*  file;
	int    		 fd;
	int    		 flags;
} _device_file;

int _open_keyboard_input_file(_device_file* kbd, const char* path);
int _close_keyboard_input_file(_device_file* kbd);


/*
	Internal structure mapping every supported key into
	KEY_RELEASED or KEY_PRESSED state.
	Part of keyboard user-interface.
*/

typedef struct _keyboard_events {
	int key_map[KEY_CNT];
} _keyboard_events;

void _clear_keyboard_events(_keyboard_events* kev);
int _get_key_from_events(_keyboard_events* kev, int key);

/*
	GENERAL PURPOSE KEYBOARD STRUCTURE.
	Supports polling from device file, clearing polled events.
	Allows getting state of every key after polling the events.
*/

typedef struct keyboard {
	_device_file device_file;
	_keyboard_events events;
} keyboard;

void init_keyboard(keyboard* keyboard);
void clear_events_keyboard(keyboard* keyboard);
void poll_events_keyboard(keyboard* keyboard);
int  get_key(keyboard* keyboard, int key);
void close_keyboard(keyboard* keyboard);


/*
	MOUSE STRUCTURE.
	So far, only supports simple mouse events polling, same as
	keyboard device structure.
	Real-time event sampling may be added in the future.
*/

typedef void (*mouse_callback_t)(int, int);

typedef struct mouse {
	_device_file device_file;
	mouse_callback_t callback_func;
} mouse;

void init_mouse(mouse* mouse);
void poll_events_mouse(mouse* mouse);
void set_pos_callback(mouse* mouse, mouse_callback_t callback_func);
void close_mouse();