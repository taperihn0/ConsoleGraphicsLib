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

#define _FORCE_INLINE inline __attribute__((always_inline))
#define _INLINE inline

#ifndef _UNUSED
#	define _UNUSED __attribute__((unused))
#endif

#ifndef _MAYBE_UNUSED
#	define _MAYBE_UNUSED _UNUSED
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

#ifndef ULONGLONG
#	define ULONGLONG unsigned long long
#endif

// for compatibility
typedef uint8_t byte;

#ifndef BYTE
#	define BYTE byte
#endif

#define _LIKELY(x)   __builtin_expect(!!(x), 1)
#define _UNLIKELY(x) __builtin_expect(!!(x), 0)


_STATIC _FORCE_INLINE bool _assert_error_terminate(const char* err, const char* file, UINT line) {
	if (stdscr)
		endwin();

	fprintf(stderr, "Assertion failed: %s\nfile: %s, line: %u\n", err, file, line);

	if (errno != 0) 
		fprintf(stderr, "Error: %s\n", strerror(errno));
	
	raise(SIGTERM);
	exit(EXIT_FAILURE);
	return true;
}

#ifdef DEBUG
#	define ASSERT(val, err) (void)((val) || _assert_error_terminate((err), __FILE__, __LINE__))
#else
#	define ASSERT(val, err) (void)(val), (void)(err)
#endif

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a < b) ? b : a)

#define minof3(a1, a2, a3) min(min(a1, a2), a3)
#define maxof3(a1, a2, a3) max(max(a1, a2), a3)

_STATIC _FORCE_INLINE void swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

#define static_strlen(str) (sizeof(str) / sizeof(char) - 1)

_MAYBE_UNUSED static byte byte_false = 0;
_MAYBE_UNUSED static byte byte_true  = 1;
