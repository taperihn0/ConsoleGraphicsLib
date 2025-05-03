#pragma once

#include "common.h"
#include "coremath.h"
#include "color.h"
#include <wchar.h>

// UNDERLAYING BUFFER TYPES
#ifndef _BUFF_ELEM_TYPE
#	define _BUFF_ELEM_TYPE wchar_t
#endif

#ifndef _BUFF_DEPTH_PREC_TYPE
#	define _BUFF_DEPTH_PREC_TYPE _prec1_t
#endif

// for external usage - terminal and buffer interface
typedef _BUFF_ELEM_TYPE			CHAR_T;
typedef _BUFF_DEPTH_PREC_TYPE PREC_T;

typedef struct _core_buffer {
	_BUFF_ELEM_TYPE*		  mem;
	_BUFF_DEPTH_PREC_TYPE* depth;
	_ncurses_pair_id*		  col_pair_num;

	size_t   		 		  width,
			 		 	   	  height,
			 		 	   	  xcenter,
			 		 	   	  ycenter;
} _core_buffer;

int  init_buffer(_core_buffer* buff, size_t width, size_t height);
int  resize_buffer(_core_buffer* buff, size_t width, size_t height);
void clear_buffer(_core_buffer* buff);
void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c);
void close_buffer(_core_buffer* buff);

void set(_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, _BUFF_ELEM_TYPE c, _ncurses_pair_id col);
// same as set, but do not perform depth checking
void set_force(_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, _BUFF_ELEM_TYPE c, _ncurses_pair_id col);
_BUFF_DEPTH_PREC_TYPE get_depth(_core_buffer* buff, int x, int y);
void flush_buffer(_core_buffer* buff);