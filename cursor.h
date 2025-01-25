#pragma once

#include "common.h"
#include <sys/types.h>

// Calls underlaying fork() function
// to create another child process taking care of invisible mouse cursor.
// Returns -1 when error occured, else 0 
int hide_cursor();

// Shows previously hidden cursor.
// Uses PID of a previously process.
// Calling it before hide_cursor() is just unexpected.
void show_cursor();