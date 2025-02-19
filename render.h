#pragma once

#include "common.h"
#include "mem.h"
#include "coremath.h"
#include "buffer.h"

struct _double_buffer {
	_core_buffer buff[2];
	short curr_buff;
};

_FORCE_INLINE _core_buffer* _get_current_buffer(struct _double_buffer* dbl) {
	return &dbl->buff[dbl->curr_buff];
}

_FORCE_INLINE void _flip_buffer_index(struct _double_buffer* dbl) {
	dbl->curr_buff ^= 1;
}

_FORCE_INLINE void _resize_buffers(struct _double_buffer* dbl, size_t width, size_t height) {
	resize_buffer(&dbl->buff[0], width, height);
	resize_buffer(&dbl->buff[1], width, height);
}

_FORCE_INLINE void _close_buffers(struct _double_buffer* dbl) {
	close_buffer(&dbl->buff[0]);
	close_buffer(&dbl->buff[1]);
}

extern struct _double_buffer _dbl_buff;

void clear_terminal(CHAR_T c);
void swap_terminal_buffers();

int set_elem(int x, int y, CHAR_T c, PREC_T d);
int draw_buffer(buff_idx_t id, mat4* vt); 