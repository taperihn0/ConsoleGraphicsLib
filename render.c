#include "render.h"
#include "terminal.h"
#include "timeman.h"
#include "render_utils.h"
#include "thread.h"

#define _TERMINAL_WIDTH  	   get_terminal_width()
#define _TERMINAL_HEIGHT 	   get_terminal_height()
#define _HALF_TERMINAL_WIDTH  (_TERMINAL_WIDTH  / 2)
#define _HALF_TERMINAL_HEIGHT (_TERMINAL_HEIGHT / 2)

_double_buffer _dbl_buff;

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
	_mutex_t flushed_buff;
} _flush_thread_ctx;

_flush_thread_ctx _flush_ctx = {
	.thread = NULL
};

bool byte_false = false;
bool byte_true = true;
_core_buffer* null_buffer = NULL;

void* _flush_thread_loop(_UNUSED void* args) {
	bool ready = false;

	while (true) {
		_read_mutex_data(&_flush_ctx.to_flush, &ready);
		
		if (ready) {
			_core_buffer* buff;
			_read_mutex_data(&_flush_ctx.flushed_buff, &buff);

			flush_buffer(buff);
			_write_mutex_data(&_flush_ctx.to_flush, &byte_false);
		}
	}

	return NULL;
}

void _init_flush_ctx() {
	_init_mutex(&_flush_ctx.to_flush, &byte_false, sizeof(bool));
	_init_mutex(&_flush_ctx.flushed_buff, &null_buffer, sizeof(_core_buffer*));
	_flush_ctx.thread = _get_thread();
	pthread_create(_flush_ctx.thread, NULL, _flush_thread_loop, NULL);
}

void _close_flush_ctx() {
	_close_mutex(&_flush_ctx.to_flush);
	_close_mutex(&_flush_ctx.flushed_buff);
}

#ifdef DEBUG

typedef struct _lateness_data {
	ULONGLONG flush_late_cnt;
	ULONGLONG writer_late_cnt;
} _lateness_data;

static _lateness_data lateness = { 
	.flush_late_cnt = 0, 
	.writer_late_cnt = 0 
};

void get_late_data(ULONGLONG* flush_late_cnt, ULONGLONG* render_late_cnt) {
	*flush_late_cnt = lateness.flush_late_cnt;
	*render_late_cnt = lateness.writer_late_cnt;
}

#endif // DEBUG

void swap_terminal_buffers() {
	bool flushing;
	
#ifdef DEBUG
	_read_mutex_data(&_flush_ctx.to_flush, &flushing);
	if (flushing) lateness.flush_late_cnt++;
	else lateness.writer_late_cnt++;
#endif // DEBUG

	do {
		_read_mutex_data(&_flush_ctx.to_flush, &flushing);
	} while (flushing);
	
	_core_buffer* to_flush_buff = _get_current_buffer(&_dbl_buff);
	_write_mutex_data(&_flush_ctx.flushed_buff, &to_flush_buff);
	_write_mutex_data(&_flush_ctx.to_flush, &byte_true);

	_flip_buffer_index(&_dbl_buff);
	_sync_with_next_frame();
}

void set_elem(int x, int y, CHAR_T c, PREC_T d, _ncurses_pair_id col) {
	set(_get_current_buffer(&_dbl_buff), x, y, d, c, col);
}

void set_elem_force(int x, int y, CHAR_T c, PREC_T d, _ncurses_pair_id col) {
	set_force(_get_current_buffer(&_dbl_buff), x, y, d, c, col);
}

// Forward function declarations
extern void _triangle_pipeline(
	shader_t* shader, byte* mem0, byte* mem1, 
	byte* mem2, size_t entry_size, void* attrib);
extern void _stage_rasterization_triangle(
	shader_t* shader, _entry_t* entries, size_t triangles_cnt, void* attrib);
extern void _stage_assembly_triangle(
	shader_t* shader, _entry_t* entries, size_t triangles_cnt, void* attrib);

int draw_buffer(shader_t* shader, buff_idx_t id, void* attrib) {
	ASSERT(shader->stage_vertex && shader->stage_fragment, "Invalid shader");

	size_t size;
	size_t entry_size;
	byte* mem = (byte*)get_mem_buff(&size, &entry_size, id);

	if (mem == NULL) {
		fprintf(stderr, "Invalid buffer index (null buffer)\n");
		return -1;
	}

	for (UINT i = 0; i < size; i += 3 * entry_size) {
		_triangle_pipeline(shader, &mem[i], &mem[i + entry_size], 
			&mem[i + 2 * entry_size], entry_size, attrib);
	}
	
	return 0;
}

int draw_order_buffer(
	shader_t* shader, buff_idx_t id, 
	element_t* elem, void* attrib) 
{
	ASSERT(shader->stage_vertex && shader->stage_fragment, "Invalid shader");

	size_t size;
	size_t entry_size;
	byte* mem = (byte*)get_mem_buff(&size, &entry_size, id);

	if (mem == NULL) {
		fprintf(stderr, "Invalid buffer index (null buffer)\n");
		return -1;
	}
	
	for (UINT i = 0; i < size; i += 3) {
		_triangle_pipeline(shader, 
			&mem[elem[i] * entry_size], 
			&mem[elem[i + 1] * entry_size], 
			&mem[elem[i + 2] * entry_size], 
			entry_size, attrib);
	}
	
	return 0;
}

/*
	Interesting article diving into culling policy:
	https://cs418.cs.illinois.edu/website/text/clipping.html
*/

#define _PLANE_LEFT   0
#define _PLANE_RIGHT  1
#define _PLANE_DOWN   2
#define _PLANE_UP 	 3
#define _PLANE_FRONT  4
#define _PLANE_BEHIND 5

void _clip_against(
	_entry_t* entry0, _entry_t* entry1, _entry_t* entry2, 
	_entry_t* triangles, size_t* cnt, UINT plane) 
{
	_entry_t* entries[4] = { entry0, entry1, entry2, entry0 };
	_entry_t edges[4];
	int edge_cnt = 0;

	for (UINT i = 1; i <= 3; i++) {
		vec4* p0 = _ENTRY_POS4(entries[i]);
		vec4* p1 = _ENTRY_POS4(entries[i - 1]);
		float dist0;
		float dist1;
		
		switch (plane) {
		case _PLANE_LEFT:
 			dist0 = p0->x + p0->w;
 			dist1 = p1->x + p1->w;

 			if (p1->x > -p1->w)
 				edges[edge_cnt++] = *entries[i - 1];
 			break;
 		case _PLANE_RIGHT:
 	 		dist0 = p0->x - p0->w;
 			dist1 = p1->x - p1->w;

 			 if (p1->x < p1->w)
 				edges[edge_cnt++] = *entries[i - 1];
 			break;
 		case _PLANE_DOWN:
 			dist0 = p0->y + p0->w;
			dist1 = p1->y + p1->w;
			
			if (p1->y > -p1->w)
				edges[edge_cnt++] = *entries[i - 1];
			break;
		case _PLANE_UP:
			dist0 = p0->y - p0->w;
			dist1 = p1->y - p1->w;
			
			if (p1->y < p1->w)
				edges[edge_cnt++] = *entries[i - 1];
			break;
		case _PLANE_FRONT:
			dist0 = p0->z + p0->w;
			dist1 = p1->z + p1->w;
			
			if (p1->z > -p1->w)
				edges[edge_cnt++] = *entries[i - 1];
			break;
		case _PLANE_BEHIND:
			dist0 = p0->z - p0->w;
			dist1 = p1->z - p1->w;
			
			if (p1->z < p1->w)
				edges[edge_cnt++] = *entries[i - 1];
			break;
 		default: 
			dist0 = 0.f;
			dist1 = 0.f;
 			break;
 		}

		if ((dist0 < 0.f && dist1 > 0.f) || (dist0 > 0.f && dist1 < 0.f)) {
			vec4 tmp_mult1 = mult_av4(dist0, p1);
			vec4 tmp_mult2 = mult_av4(dist1, p0);

			vec4 intersect = sub4f(&tmp_mult1, &tmp_mult2);
			intersect = mult_av4(1.f / (dist0 - dist1), &intersect);
			
			vec3 tmp_mult3 = mult_av3(dist0, _ENTRY_COL(entries[i - 1]));
			vec3 tmp_mult4 = mult_av3(dist1, _ENTRY_COL(entries[i]));

			vec3 rgb = sub3f(&tmp_mult3, &tmp_mult4);
			rgb = mult_av3(1.f / (dist0 - dist1), &rgb);

			// So far, only predefined colors and normals are interpolated
			tmp_mult3 = mult_av3(dist0, _ENTRY_NORM(entries[i - 1]));
			tmp_mult4 = mult_av3(dist1, _ENTRY_NORM(entries[i]));

			vec3 norm = sub3f(&tmp_mult3, &tmp_mult4);
			norm = mult_av3(1.f / (dist0 - dist1), &norm);

			_entry_t new_entry = _entry_from_vec4(&intersect, &rgb, &norm);
			edges[edge_cnt++] = new_entry;
		}
	}

	for (int i = 0; i < edge_cnt - 1; i += 2) {
		triangles[*cnt * 3] = edges[i];
		triangles[*cnt * 3 + 1] = edges[i + 1];
		triangles[*cnt * 3 + 2] = edges[(i + 2) & (edge_cnt - 1)];
		*cnt += 1;
	}
}

void _clip_planes(	
	_entry_t* entry0, _entry_t* entry1, _entry_t* entry2, 
	_entry_t* clip, size_t* cnt) 
{
	size_t clip_cnt = 1;
	size_t new_total_cnt = 0;

	clip[0] = *entry0;
	clip[1] = *entry1;
	clip[2] = *entry2;

	if (_mode == RENDER_MODE_EDGES)
		return;
	
	for (UINT against = _PLANE_LEFT; against <= _PLANE_UP; against++) {
		new_total_cnt = 0;
		_entry_t new_entries[64];

		for (UINT j = 0; j < 3 * clip_cnt; j += 3) {
			size_t new_cnt = 0;

			_clip_against(&clip[j], &clip[j + 1], &clip[j + 2], 
				new_entries + 3 * new_total_cnt, &new_cnt, against);

			new_total_cnt += new_cnt;
		}
		
		for (UINT k = 0; k < 3 * new_total_cnt; k++) {
			clip[k] = new_entries[k];
		}

		clip_cnt = new_total_cnt;
	}

	*cnt = clip_cnt;
}

_FORCE_INLINE void _triangle_pipeline(
	shader_t* shader, byte* mem0, byte* mem1, byte* mem2, 
	size_t entry_size, void* attrib) 
{
	_entry_t entry0 = _get_entry(mem0, entry_size);
	_entry_t entry1 = _get_entry(mem1, entry_size);
	_entry_t entry2 = _get_entry(mem2, entry_size);

	shader->stage_vertex(&entry0, attrib);
	shader->stage_vertex(&entry1, attrib);
	shader->stage_vertex(&entry2, attrib);
	
	_entry_t entries[64];
	size_t triangles_cnt = 1;
	_clip_planes(&entry0, &entry1, &entry2, entries, &triangles_cnt);

	// PERSPECTIVE DIVISION
	for (UINT i = 0; i < triangles_cnt * 3; i += 3) {
		vec3* v0 = (vec3*)_ENTRY_POS4(&entries[i]);
		vec3* v1 = (vec3*)_ENTRY_POS4(&entries[i + 1]);
		vec3* v2 = (vec3*)_ENTRY_POS4(&entries[i + 2]);
	
		*v0 = div_av3(*((float*)v0 + 3), v0); 
		v0->x *= _HALF_TERMINAL_WIDTH;
		v0->y *= _HALF_TERMINAL_HEIGHT;
		
		*v1 = div_av3(*((float*)v1 + 3), v1);
		v1->x *= _HALF_TERMINAL_WIDTH;
		v1->y *= _HALF_TERMINAL_HEIGHT;
		
		*v2 = div_av3(*((float*)v2 + 3), v2);
		v2->x *= _HALF_TERMINAL_WIDTH;
		v2->y *= _HALF_TERMINAL_HEIGHT;
	}

	if (_mode == RENDER_MODE_EDGES) {
		_stage_assembly_triangle(shader, entries, triangles_cnt, attrib);
		return;
	}

	_stage_rasterization_triangle(shader, entries, triangles_cnt, attrib);
}

_FORCE_INLINE void _stage_rasterization_triangle(
	shader_t* shader, _entry_t* entries, size_t triangles_cnt, void* attrib) 
{
	for (UINT i = 0; i < 3 * triangles_cnt; i += 3) {
		_draw_triangle_solid(
			_ENTRY_POS4(&entries[i]), _ENTRY_POS4(&entries[i + 1]), _ENTRY_POS4(&entries[i + 2]),
			_ENTRY_COL(&entries[i]),  _ENTRY_COL(&entries[i + 1]),  _ENTRY_COL(&entries[i + 2]),
			_ENTRY_NORM(&entries[i]), _ENTRY_NORM(&entries[i + 1]), _ENTRY_NORM(&entries[i + 2]),
			shader->stage_fragment,
			attrib);
	}
}

_FORCE_INLINE void _stage_assembly_triangle(
	shader_t* shader, _entry_t* entries, size_t triangles_cnt, void* attrib) 
{
	for (UINT i = 0; i < 3 * triangles_cnt; i += 3) {
		_draw_triangle_edges(
			_ENTRY_POS4(&entries[i]), _ENTRY_POS4(&entries[i + 1]), _ENTRY_POS4(&entries[i + 2]),
			_ENTRY_COL(&entries[i]),  _ENTRY_COL(&entries[i + 1]),  _ENTRY_COL(&entries[i + 2]),
			_ENTRY_NORM(&entries[i]), _ENTRY_NORM(&entries[i + 1]), _ENTRY_NORM(&entries[i + 2]),
			shader->stage_fragment,
			attrib);
	}
}