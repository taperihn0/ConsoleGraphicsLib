#include "thread.h"

int _init_mutex(_mutex_t* mutex, void* data, size_t n) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_init(&mutex->pthread_mutex_lock, NULL);
	mutex->data = malloc(n);
	memcpy(mutex->data, data, n);
	pthread_mutex_unlock(&mutex->pthread_mutex_lock);
	return 0;
}

int _read_mutex_data(_mutex_t* mutex, void* data, size_t n) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_lock(&mutex->pthread_mutex_lock);
	memcpy(data, mutex->data, n);
	pthread_mutex_unlock(&mutex->pthread_mutex_lock);
	return 0;
}

int _write_mutex_data(_mutex_t* mutex, void* data, size_t n) {
	if (!mutex) {
		fprintf(stderr, "Null mutex");
		return -1;
	}
	pthread_mutex_lock(&mutex->pthread_mutex_lock);
	memcpy((void*)mutex->data, data, n);
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