#pragma once

#include "common.h"
#include <sys/types.h>

// Calls underlaying fork() function
// to create another child process
// for uncutter program.
// Returns -1 when error occured, else 0 
int hide_cursor();

// Shows previously hidden cursor.
// Uses pid of a uncutter process.
// Calling it before hide_cursor() 
// is unexpected behaviour.
void show_cursor();