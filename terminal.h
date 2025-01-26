#pragma once

#include "common.h"

// wmctrl command wrappers.
void make_terminal_fullscreen();
void unmake_terminal_fullscreen();
void set_terminal_title();

// termios manipulations.
// Sets STD_FILENO file descriptor to be
// non canonical. Disables echo and setting 
// input non blocking.
void enable_raw_mode();

// disables raw input mode
void disable_raw_mode();

// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// https://unix.stackexchange.com/questions/480052/how-do-i-detect-whether-my-terminal-has-focus-in-the-gui-from-a-shell-script
void enable_focus_events();
void disable_focus_events();

// internal function used in polling.
// returns true if console window has focus, else false.
bool _check_focus();