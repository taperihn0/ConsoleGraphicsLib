#pragma once

#include "common.h"
#include "mem.h"
#include "coremath.h"
#include "buffer.h"
#include "light.h"

typedef struct _double_buffer {
	_core_buffer buff[2];
	short curr_buff;
} _double_buffer;

void _resize_buffers(_double_buffer* dbl, size_t width, size_t height);
void _close_buffers(_double_buffer* dbl);

extern _double_buffer _dbl_buff;

void _init_flush_ctx();
void _close_flush_ctx();

#define RENDER_MODE_SOLID 0
#define RENDER_MODE_EDGES 1
#define _RENDER_MODE_MIN 0
#define _RENDER_MODE_MAX 1
#define _RENDER_MODE_CNT _RENDER_MODE_MAX + 1

typedef UINT render_mode_t;

void set_render_mode(render_mode_t mode);

void clear_terminal(CHAR_T c);
void swap_terminal_buffers();
int set_elem(int x, int y, CHAR_T c, PREC_T d);

typedef void (*func_stage_vertex)(
	_entry_t* entry0, _entry_t* entry1, _entry_t* entry2, void* attrib);

typedef void (*func_stage_fragment)(
	_entry_t* normalized, void* attrib);

typedef struct shader_t {
	func_stage_vertex stage_vertex;
	func_stage_fragment stage_fragment;
} shader_t;

typedef UINT element_t;

int draw_buffer(shader_t* shader, buff_idx_t id, void* attrib);
int draw_order_buffer(
	shader_t* shader, buff_idx_t id, 
	element_t* elem, void* attrib);