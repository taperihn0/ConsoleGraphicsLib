#pragma once

#include "common.h"
#include <pthread.h>

typedef struct _mutex_t {
	pthread_mutex_t pthread_mutex_lock;
	void* data;
} _mutex_t;

int _init_mutex(_mutex_t* mutex, void* data, size_t n);
int _read_mutex_data(_mutex_t* mutex, void* data, size_t n);
int _write_mutex_data(_mutex_t* mutex, void* data, size_t n);
int _close_mutex(_mutex_t* mutex);