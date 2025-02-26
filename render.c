#include "render.h"
#include "terminal.h"
#include "timeman.h"
#include "render_utils.h"
#include "thread.h"

#define _TERMINAL_WIDTH  	  get_terminal_width()
#define _TERMINAL_HEIGHT 	  get_terminal_height()
#define _HALF_TERMINAL_WIDTH  (_TERMINAL_WIDTH / 2)
#define _HALF_TERMINAL_HEIGHT (_TERMINAL_HEIGHT / 2)

_double_buffer _dbl_buff;

_FORCE_INLINE _core_buffer* _get_current_buffer(_double_buffer* dbl) {
	return &dbl->buff[dbl->curr_buff];
}

_FORCE_INLINE void _flip_buffer_index(_double_buffer* dbl) {
	dbl->curr_buff ^= 1;
}

void _resize_buffers(_double_buffer* dbl, size_t width, size_t height) {
	resize_buffer(&dbl->buff[0], width, height);
	resize_buffer(&dbl->buff[1], width, height);
}

void _close_buffers(_double_buffer* dbl) {
	close_buffer(&dbl->buff[0]);
	close_buffer(&dbl->buff[1]);
}

render_mode_t _mode;

void set_render_mode(render_mode_t mode) {
	ASSERT(_RENDER_MODE_MIN <= mode && mode <= _RENDER_MODE_MAX, "Invalid render mode");
	_mode = mode;
}

void clear_terminal(CHAR_T c) {
	clear_buffer_with(_get_current_buffer(&_dbl_buff), c);
}

typedef struct _flush_thread_ctx {
	pthread_t* thread;
	_mutex_t to_flush;
	_mutex_t swap;
} _flush_thread_ctx;

_flush_thread_ctx _flush_ctx = {
	.thread = NULL
};

bool byte_false = false;
bool byte_true = true;

void* _flush_thread_loop(void* args) {
	bool ready = false;

	while (true) {
		_read_mutex_data(&_flush_ctx.to_flush, &ready);
		
		if (ready) {
			_write_mutex_data(&_flush_ctx.swap, &byte_false);
			_core_buffer* buff = _get_current_buffer(&_dbl_buff);
			_write_mutex_data(&_flush_ctx.swap, &byte_true);

			flush_buffer(buff);
			_write_mutex_data(&_flush_ctx.to_flush, &byte_false);
		}
	}

	return NULL;
}

void _init_flush_ctx() {
	_init_mutex(&_flush_ctx.to_flush, &byte_false, sizeof(bool));
	_init_mutex(&_flush_ctx.swap, &byte_false, sizeof(bool));
	_flush_ctx.thread = _get_thread();
	pthread_create(_flush_ctx.thread, NULL, _flush_thread_loop, NULL);
}

void _close_flush_ctx() {
	_close_mutex(&_flush_ctx.to_flush);
	_close_mutex(&_flush_ctx.swap);
}

void swap_terminal_buffers() {
	bool flushing;

	do {
		_read_mutex_data(&_flush_ctx.to_flush, &flushing);
	} while (flushing);

	_write_mutex_data(&_flush_ctx.to_flush, &byte_true);
	
	bool ready2swap;

	do {
		_read_mutex_data(&_flush_ctx.swap, &ready2swap);
	} while (!ready2swap);

	_flip_buffer_index(&_dbl_buff);
	_sync_with_next_frame();
}

int set_elem(int x, int y, CHAR_T c, PREC_T d) {
	set(_get_current_buffer(&_dbl_buff), x, y, d, c);
	return 0;
}

extern void _triangle_pipeline(
	shader_t* shader, byte* mem, size_t entry_size, void* attrib);
extern void _stage_rasterization_triangle(
	shader_t* shader, _entry_t* entry0, _entry_t* entry1, _entry_t* entry2, void* attrib);
extern void _stage_assembly_triangle(vec4* v1, vec4* v2, vec4* v3);

int draw_buffer(shader_t* shader, buff_idx_t id, void* attrib) {
	// ISVALIDSHADER() ...
	size_t size;
	size_t entry_size;
	byte* mem = (byte*)get_mem_buff(&size, &entry_size, id);

	if (mem == NULL) {
		fprintf(stderr, "Invalid buffer index (null buffer)\n");
		return -1;
	}

	for (UINT i = 0; i < size; i += 3 * entry_size) {
		_triangle_pipeline(shader, &mem[i], entry_size, attrib);
	}
	
	return 0;
}

_FORCE_INLINE void _triangle_pipeline(
	shader_t* shader, byte* mem, size_t entry_size, void* attrib) 
{
	_entry_t entry0 = _get_entry(mem, entry_size);
	_entry_t entry1 = _get_entry(mem + entry_size, entry_size);
	_entry_t entry2 = _get_entry(mem + 2 * entry_size, entry_size);

	vec4* v0 = _ENTRY_POS4(&entry0);
	vec4* v1 = _ENTRY_POS4(&entry1);
	vec4* v2 = _ENTRY_POS4(&entry2);

	shader->stage_vertex(&entry0, &entry1, &entry2, attrib);

	v0->x = v0->x / v0->w * _HALF_TERMINAL_WIDTH;
	v0->y = v0->y / v0->w * _HALF_TERMINAL_HEIGHT;
	v0->z = v0->z / v0->w;

	v1->x = v1->x / v1->w * _HALF_TERMINAL_WIDTH;
	v1->y = v1->y / v1->w * _HALF_TERMINAL_HEIGHT;
	v1->z = v1->z / v1->w;
	
	v2->x = v2->x / v2->w * _HALF_TERMINAL_WIDTH;
	v2->y = v2->y / v2->w * _HALF_TERMINAL_HEIGHT;
	v2->z = v2->z / v2->w;

	if (_mode == RENDER_MODE_EDGES) {
		_stage_assembly_triangle(v0, v1, v2);
		return;
	}

	_stage_rasterization_triangle(shader, &entry0, &entry1, &entry2, attrib);
}

// TODO: IMPLEMENT PROPER COLOR INTERPRETATION, NOT ONLY "BRIGHTNESS"
_FORCE_INLINE void _stage_rasterization_triangle(
	shader_t* shader, _entry_t* entry0, _entry_t* entry1, _entry_t* entry2, void* attrib) 
{
	_draw_triangle_solid(
		_ENTRY_POS3(entry0), _ENTRY_POS3(entry1), _ENTRY_POS3(entry2),
		_ENTRY_COL(entry0), _ENTRY_COL(entry1), _ENTRY_COL(entry2),
		_ENTRY_NORM(entry0), _ENTRY_NORM(entry1), _ENTRY_NORM(entry2),
		shader->stage_fragment,
		attrib);
}

_FORCE_INLINE void _stage_assembly_triangle(vec4* v1, vec4* v2, vec4* v3) {
	_draw_triangle_edges(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y);
}