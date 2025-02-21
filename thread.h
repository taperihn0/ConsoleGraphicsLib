#pragma once

#include "common.h"
#include <pthread.h>

/*
	THREAD MANAGEMENT SYSTEM -
	direct usage of thread leads to a need to 
	store all of the threads globally inside context
	to handle all threads termination properly and globally.
*/

#define _THREAD_LIMIT 8

typedef struct _thread_man {
	pthread_t threads[_THREAD_LIMIT];
	size_t idx;
} _thread_man;

extern _thread_man _thr_man;

pthread_t* _get_thread();
void _close_all_threads();


typedef struct _mutex_t {
	pthread_mutex_t pthread_mutex_lock;
	void* data;
	size_t size;
} _mutex_t;

int _init_mutex(_mutex_t* mutex, void* data, size_t n);
int _read_mutex_data(_mutex_t* mutex, void* data);
int _write_mutex_data(_mutex_t* mutex, void* data);
int _close_mutex(_mutex_t* mutex);