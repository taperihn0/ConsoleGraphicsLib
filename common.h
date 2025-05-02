#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>

#define NCURSES_WIDECHAR 1
#include <ncurses.h>

#ifndef _FORCE_INLINE
#	define _FORCE_INLINE inline __attribute__((always_inline))
#endif
#define _INLINE inline

#ifndef _UNUSED
#	define _UNUSED __attribute__((unused))
#endif

// Got problems with static functions while trying to compile the code under 
// different optimization flags with GCC.
// For instance, -Og requires every function defined here needs to be explicitly
// static, while, say, -Ofast do not care.
#ifdef DEBUG
#	define _STATIC static
#else
#	define _STATIC
#endif

#ifndef UINT
#	define UINT unsigned int
#endif

#ifndef ULL
#	define ULONGLONG unsigned long long
#endif

typedef unsigned char byte;

_STATIC _INLINE bool _assert_error_terminate(const char* err, const char* file, UINT line) {
	if (stdscr)
		endwin();

	fprintf(stderr, "Assertion failed: %s\nfile: %s, line: %u\n", err, file, line);

	if (errno != 0) 
		fprintf(stderr, "Error: %s\n", strerror(errno));
	
	raise(SIGTERM);
	exit(EXIT_FAILURE);
	return true;
}

#define ASSERT(val, err) (void)((val) || _assert_error_terminate((err), __FILE__, __LINE__))

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a < b) ? b : a)

#define minof3(a1, a2, a3) min(min(a1, a2), a3)
#define maxof3(a1, a2, a3) max(max(a1, a2), a3)

_STATIC _FORCE_INLINE void swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}