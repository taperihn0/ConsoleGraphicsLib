#include "thread.h"

_thread_man _thr_man;

pthread_t* _get_thread() {
	if (_thr_man.idx >= _THREAD_LIMIT)
		return NULL;
	return &_thr_man.threads[_thr_man.idx++];
}

void _close_all_threads() {
	for (UINT i = 0; i < _thr_man.idx; i++) {
		pthread_cancel(_thr_man.threads[i]);
	}
}

int _init_mutex(_mutex_t* mutex, void* data, size_t n) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_init(&mutex->pthread_mutex_lock, NULL);
	mutex->data = malloc(n);
	memcpy(mutex->data, data, n);
	mutex->size = n;
	pthread_mutex_unlock(&mutex->pthread_mutex_lock);
	return 0;
}

int _read_mutex_data(_mutex_t* mutex, void* data) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_lock(&mutex->pthread_mutex_lock);
	memcpy(data, mutex->data, mutex->size);
	pthread_mutex_unlock(&mutex->pthread_mutex_lock);
	return 0;
}

int _write_mutex_data(_mutex_t* mutex, void* data) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_lock(&mutex->pthread_mutex_lock);
	memcpy((void*)mutex->data, data, mutex->size);
	pthread_mutex_unlock(&mutex->pthread_mutex_lock);
	return 0;
}

int _close_mutex(_mutex_t* mutex) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_destroy(&mutex->pthread_mutex_lock);
	return 0;	
}