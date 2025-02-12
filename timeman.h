#pragma once

#include "common.h"
#include <time.h>

typedef long utime_t;

_FORCE_INLINE utime_t gettime_ns(clockid_t clockid) {
	struct timespec t;
	int c = clock_gettime(clockid, &t);
	ASSERT(c == 0, "Cannot get time with clock_gettime");
	return t.tv_nsec + t.tv_sec * 1000000000;
}

_FORCE_INLINE utime_t gettime_mcs(clockid_t clockid) {
	struct timespec t;
	int c = clock_gettime(clockid, &t);
	ASSERT(c == 0, "Cannot get time with clock_gettime");
	return t.tv_nsec / 1000 + t.tv_sec * 1000000;
}

_FORCE_INLINE utime_t gettime_mls(clockid_t clockid) {
	struct timespec t;
	int c = clock_gettime(clockid, &t);
	ASSERT(c == 0, "Cannot get time with clock_gettime");
	return t.tv_nsec / 1000000 + t.tv_sec * 1000;
}

_FORCE_INLINE utime_t gettime_s(clockid_t clockid) {
	struct timespec t;
	int c = clock_gettime(clockid, &t);
	ASSERT(c == 0, "Cannot get time with clock_gettime");
	return t.tv_nsec / 1000000000 + t.tv_sec;
}