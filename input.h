#pragma once

#include "common.h"
#include <errno.h>
#include <linux/input.h>

#define KEY_NO_EVENT -1
#define KEY_RELEASED  0
#define KEY_PRESSED   1
#define KEY_REPEATED  2

// termios manipulations.
// Sets STD_FILENO file descriptor to be
// non canonical. Disables echo and setting 
// input non blocking.
void enable_raw_mode();

// disables raw input mode
void disable_raw_mode();

typedef struct device_file {
	char*  file;
	int    fd;
	int    flags;
} device_file;

int open_keyboard_input_file(device_file* kbd);
int close_keyboard_input_file(device_file* kbd);

typedef struct keyboard_events {
	int key_map[KEY_CNT];
} keyboard_events;

void clear_keyboard_events(keyboard_events* kev);
void poll_keyboard_events(keyboard_events* kev, device_file* kbd);

int get_key(keyboard_events* kev, int key);

#ifdef DEBUG
void test_input(keyboard_events* kev);
#endif