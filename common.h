#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <ncurses.h>
#include <signal.h>

#ifndef _FORCE_INLINE
#	define _FORCE_INLINE inline __attribute__((always_inline))
#endif

#define _INLINE inline

#ifndef UINT
#	define UINT unsigned int
#endif

static _INLINE bool _assert_error_terminate(const char* err, const char* file, UINT line) {
	// allow terminal to close its state properly by handling a signal
	raise(SIGTERM);
	fprintf(stderr, "Assertion failed: %s\nfile: %s, line: %u\n", err, file, line);
	exit(EXIT_FAILURE);
	return true;
}

#define ASSERT(val, err) (void)((val) || _assert_error_terminate((err), __FILE__, __LINE__))

static _FORCE_INLINE int min(int a, int b) {
	return a < b ? a : b;
}

static _FORCE_INLINE int max(int a, int b) {
	return a < b ? b : a;
}

static _FORCE_INLINE void swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}