#pragma once

#include "common.h"
#include <sys/types.h>

typedef struct _main_cursor {
	bool  visible;
} _main_cursor;

extern _main_cursor _cursor;

// Returns -1 when error occured, else 0 
int hide_cursor();

// Calling it before hide_cursor() is just unexpected.
void show_cursor();