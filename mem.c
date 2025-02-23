#include "mem.h"

#define _BUFFER_LIMIT 		   8u
#define _INVALID_BUFFER_INDEX -1

_entry_t _get_entry(void* mem, size_t entry_size) {
	_entry_t r;
	memset(&r, 0, sizeof(r));

	size_t float_num = entry_size / sizeof(float);
	float* p = (float*)&r;

	for (UINT i = 0; i < 3 && i < float_num; i++) {
		*(p + i) = *((float*)mem + i);
	}

	r.pos_w = 1.f;

	for (UINT i = 3; i < float_num; i++) {
		*(p + i + 1) = *((float*)mem + i);
	}

	return r;
}

typedef struct _mem_buff {
	void*  mem;
	size_t mem_size;
	size_t entry_size;
} _mem_buff;

typedef struct _buff_array_t {
	_mem_buff arr[_BUFFER_LIMIT];
} _buff_array_t;

_buff_array_t* _buff_array = NULL;

_STATIC _FORCE_INLINE void _init_buff_array(_buff_array_t* ba) {
	memset(ba->arr, 0, sizeof(ba->arr));
}

buff_idx_t _get_next_idx(_buff_array_t* ba) {
	for (UINT i = 0; i < _BUFFER_LIMIT; i++) {
		if (ba->arr[i].mem == NULL)
			return i;
	}

	return (buff_idx_t)_INVALID_BUFFER_INDEX;
}

bool _is_empty(_buff_array_t* ba) {
	for (UINT i = 0; i < _BUFFER_LIMIT; i++) {
		if (ba->arr[i].mem != NULL)
			return false;
	}

	return true;
} 

int gen_mem_buff(void* mem, size_t mem_size, size_t entry_size, buff_idx_t* idx) {
	if (_buff_array == NULL) {
		_buff_array = malloc(sizeof(_buff_array_t));

		if (_buff_array == NULL) {
			fprintf(stderr, "Cannot allocate buffer array");
			return -1;
		}

		_init_buff_array(_buff_array);
	} else if (mem_size == 0) {
		fprintf(stderr, "Invalid zero size");
		return -1;
	}

	buff_idx_t next = _get_next_idx(_buff_array);

	if (next == (buff_idx_t)_INVALID_BUFFER_INDEX) {
		fprintf(stderr, "Invalid buffer index (probably excedeed buffer limit)\n");
		return -1;
	}

	_mem_buff* entry = &_buff_array->arr[next];
	entry->mem = malloc(mem_size);

	if (entry->mem == NULL) {
		fprintf(stderr, "Cannot malloc %lu bytes\n", mem_size);
		return -1;
	}

	memcpy(entry->mem, mem, mem_size);
	entry->mem_size = mem_size;
	entry->entry_size = entry_size;

	*idx = next; 
	return 0;
}

int delete_mem_buff(buff_idx_t idx) {
	if (idx >= _BUFFER_LIMIT) {
		fprintf(stderr, "Invalid buffer index\n");
		return -1;
	} else if (_buff_array == NULL) {
		return 0;
	}

	_mem_buff* entry = &_buff_array->arr[idx];

	free(entry->mem);
	entry->mem = NULL;
	entry->entry_size = 0;

	if (_is_empty(_buff_array)) {
		free(_buff_array);
		_buff_array = NULL;
	}

	return 0;
}

int set_mem_buff(void* mem, size_t mem_size, buff_idx_t idx) {
	if (idx >= _BUFFER_LIMIT || _buff_array == NULL) {
		fprintf(stderr, "Invalid buffer index\n");
		return -1;
	} else if (mem_size == 0) {
		fprintf(stderr, "Invalid zero size");
		return -1;
	}

	_mem_buff* entry = &_buff_array->arr[idx];

	entry->mem = realloc(entry->mem, mem_size);

	if (entry->mem == NULL) {
		fprintf(stderr, "Cannot realloc %lu bytes\n", mem_size);
		return -1;
	}
	
	memcpy(entry->mem, mem, mem_size);
	entry->mem_size = mem_size;
	
	return 0;
}

void* get_mem_buff(size_t* mem_size, size_t* entry_size, buff_idx_t idx) {
	if (idx >= _BUFFER_LIMIT) {
		fprintf(stderr, "Invalid buffer index\n");
		return NULL;
	}

	_mem_buff* entry = &_buff_array->arr[idx];
	*mem_size = entry->mem_size;
	*entry_size = entry->entry_size;
	return entry->mem;
}