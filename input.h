#pragma once

#include "common.h"
#include <linux/input.h>

#define KEY_INVALID  -1
#define KEY_RELEASED  0
#define KEY_PRESSED   1

typedef struct _device_file {
	char*  file;
	int    fd;
	int    flags;
} _device_file;

int _open_keyboard_input_file(_device_file* kbd);
int _close_keyboard_input_file(_device_file* kbd);

typedef struct _keyboard_events {
	int key_map[KEY_CNT];
} _keyboard_events;

void _clear_keyboard_events(_keyboard_events* kev);
void _poll_keyboard_events(_keyboard_events* kev, _device_file* kbd);

int _get_key_from_events(_keyboard_events* kev, int key);

typedef struct keyboard {
	_device_file device_file;
	_keyboard_events events;
} keyboard;

void init_keyboard(keyboard* keyboard);
void clear_events(keyboard* keyboard);
void poll_events(keyboard* keyboard);
int  get_key(keyboard* keyboard, int key);
void close_keyboard(keyboard* keyboard);