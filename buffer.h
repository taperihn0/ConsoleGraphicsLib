#pragma once

#include "common.h"
#include <wchar.h>

// UNDERLAYING BUFFER TYPES
typedef float   	_prec0_t;
typedef double  	_prec1_t;
typedef long double _prec2_t;

#ifndef _BUFF_ELEM_TYPE
#	define _BUFF_ELEM_TYPE wchar_t
#endif

#ifndef _BUFF_DEPTH_PREC_TYPE
#	define _BUFF_DEPTH_PREC_TYPE _prec1_t
#endif

// for external usage - terminal and buffer interface
typedef _BUFF_ELEM_TYPE  	  CHAR_T;
typedef _BUFF_DEPTH_PREC_TYPE PREC_T;

typedef struct _core_buffer {
	_BUFF_ELEM_TYPE* 	   mem;
	_BUFF_DEPTH_PREC_TYPE* depth;
	size_t   		 	   width,
			 		 	   height,
			 		 	   xcenter,
			 		 	   ycenter;
} _core_buffer;

int  init_buffer(_core_buffer* buff, size_t width, size_t height);
int  resize_buffer(_core_buffer* buff, size_t width, size_t height);
void clear_buffer(_core_buffer* buff);
void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c);
void close_buffer(_core_buffer* buff);

void set(_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, _BUFF_ELEM_TYPE c);
void flush_buffer(_core_buffer* buff);