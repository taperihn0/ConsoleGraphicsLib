#pragma once

#include "common.h"
#include "buffer.h"
#include "timeman.h"

typedef struct _main_terminal {
	UINT 		 width, 
				 height;
	bool 		 fullscreen;
	bool 		 focus_events;
	bool 		 raw_mode;
	bool 		 console_cursor;
	bool 		 is_focus;
	bool 		 over;
	utime_t		 microsec_delay;
} _main_terminal;

extern _main_terminal _terminal;

void _init_terminal_state();
void _close_terminal_state();

// wmctrl command wrappers.
int make_terminal_fullscreen();
int unmake_terminal_fullscreen();
int set_terminal_title();

// termios manipulations.
// Sets STD_FILENO file descriptor to be
// non canonical. Disables echo and setting 
// input non blocking.
void enable_raw_mode();
// disables raw input mode
void disable_raw_mode();

// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// https://unix.stackexchange.com/questions/480052/how-do-i-detect-whether-my-terminal-has-focus-in-the-gui-from-a-shell-script
int enable_focus_events();
int disable_focus_events();

void enable_console_cursor();
void disable_console_cursor();

// internal function used in polling.
// returns true if console window has focus, else false.
bool _check_focus();

// updates size of this terminal. For internal usage.
void _update_terminal_size();

// sets rate of flush_terminal calls per second.
void set_framerate_limit(UINT cnt);

void _sync_with_next_frame();

// returns cached width and height of main terminal.
UINT get_terminal_width();
UINT get_terminal_height();

bool should_quit();