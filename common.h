#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef _FORCE_INLINE
#	ifdef _MSC_VER
#		define _FORCE_INLINE __declspec(forceinline)
#	else
#		define _FORCE_INLINE inline __attribute__((always_inline))
#	endif
#endif

#define _INLINE inline

#ifndef UINT
#	define UINT unsigned int
#endif

static _INLINE int _assert_error_terminate(const char* err) {
	printf("%s\n", err);
	exit(1);
	return 1;
}

#define ASSERT(val, err) (void)((val) || _assert_error_terminate((err)))

static _FORCE_INLINE int min(int a, int b) {
	return a < b ? a : b;
}

static _FORCE_INLINE int max(int a, int b) {
	return a < b ? b : a;
}