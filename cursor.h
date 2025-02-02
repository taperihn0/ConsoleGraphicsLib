#pragma once

#include "common.h"
#include <sys/types.h>

typedef struct _main_cursor {
	bool  visible;
	pid_t pid;
} _main_cursor;

extern _main_cursor _cursor;

// Calls underlaying fork() function
// to create another child process taking care of invisible mouse cursor.
// Returns -1 when error occured, else 0 
int hide_cursor();

// Shows previously hidden cursor.
// Uses PID of a previously process.
// Calling it before hide_cursor() is just unexpected.
void show_cursor();