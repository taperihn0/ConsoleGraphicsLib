#pragma once

#include "common.h"

void make_terminal_fullscreen();

// termios manipulations.
// Sets STD_FILENO file descriptor to be
// non canonical. Disables echo and setting 
// input non blocking.
void enable_raw_mode();

// disables raw input mode
void disable_raw_mode();