#pragma once

#include "common.h"
#include <wchar.h>

// UNDERLAYING BUFFER TYPE
#define _BUFF_ELEM_TYPE wchar_t

// for external usage - terminal and buffer interface
#define CHAR_T _BUFF_ELEM_TYPE

typedef struct _core_buffer {
	_BUFF_ELEM_TYPE* mem;
	size_t   		 width,
			 		 height;
} _core_buffer;

int init_buffer(_core_buffer* buff, size_t width, size_t height);
int resize_buffer(_core_buffer* buff, size_t width, size_t height);
void clear_buffer(_core_buffer* buff);
void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c);
void close_buffer(_core_buffer* buff);

void set(_core_buffer* buff, size_t x, size_t y, _BUFF_ELEM_TYPE c);
void flush_buffer(_core_buffer* buff);