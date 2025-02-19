#include "render.h"
#include "terminal.h"
#include "timeman.h"
#include "render_core.h"
#include <pthread.h>

#define _TERMINAL_WIDTH  	  get_terminal_width()
#define _TERMINAL_HEIGHT 	  get_terminal_height()
#define _HALF_TERMINAL_WIDTH  (_TERMINAL_WIDTH / 2)
#define _HALF_TERMINAL_HEIGHT (_TERMINAL_HEIGHT / 2)

struct _double_buffer _dbl_buff;

void clear_terminal(CHAR_T c) {
	clear_buffer_with(_get_current_buffer(&_dbl_buff), c);
}

// TODO: IMPLEMENT MORE EFFICIENT MULTITHREADING FOR FLUSHING BUFFER
pthread_t _thread_flush;
bool _is_thread_valid = false;

void swap_terminal_buffers() {
	if (_is_thread_valid) {
		pthread_join(_thread_flush, NULL);
	} 

	pthread_attr_t _attrs;
	pthread_attr_init(&_attrs);
	pthread_attr_setdetachstate(&_attrs, PTHREAD_CREATE_JOINABLE);

	pthread_create(&_thread_flush, &_attrs, flush_buffer, (void*)_get_current_buffer(&_dbl_buff));
	_is_thread_valid = true;
	
	//flush_buffer((void*)_get_current_buffer(&_dbl_buff));
	_flip_buffer_index(&_dbl_buff);
	_sync_with_next_frame();
}

int set_elem(int x, int y, CHAR_T c, PREC_T d) {
	set(_get_current_buffer(&_dbl_buff), x, y, d, c);
	return 0;
}

extern void _triangle_pipeline(vec3* mem, mat4* vt);

int draw_buffer(buff_idx_t id, mat4* vt) {
	size_t size;
	vec3* mem = (vec3*)get_mem_buff(&size, id);

	if (mem == NULL) {
		fprintf(stderr, "Invalid buffer index (null buffer)\n");
		return -1;
	}
	
	size_t cnt = size / sizeof(vec3);

	for (UINT i = 0; i < cnt; i += 3) {
		_triangle_pipeline(&mem[i], vt);
	}
	
	return 0;
}

extern void _stage_vertex_triangle(vec4* v1, vec4* v2, vec4* v3, mat4* vt);
extern void _stage_rasterization_triangle(vec4* v1, vec4* v2, vec4* v3);

_FORCE_INLINE void _triangle_pipeline(vec3* mem, mat4* vt) {
	vec4 v1 = vec4f(mem[0].x, mem[0].y, mem[0].z, 1.f), 
		 v2 = vec4f(mem[1].x, mem[1].y, mem[1].z, 1.f), 
		 v3 = vec4f(mem[2].x, mem[2].y, mem[2].z, 1.f);

	_stage_vertex_triangle(&v1, &v2, &v3, vt);
	_stage_rasterization_triangle(&v1, &v2, &v3);
}

_FORCE_INLINE void _stage_vertex_triangle(vec4* v1, vec4* v2, vec4* v3, mat4* vt) {
	*v1 = mult_mv4(vt, v1);
	*v2 = mult_mv4(vt, v2);
	*v3 = mult_mv4(vt, v3);

	v1->x = (v1->x / v1->w) * _HALF_TERMINAL_WIDTH;
	v1->y = (v1->y / v1->w) * _HALF_TERMINAL_HEIGHT;
	v1->z = (v1->z / v1->w);

	v2->x = (v2->x / v2->w) * _HALF_TERMINAL_WIDTH;
	v2->y = (v2->y / v2->w) * _HALF_TERMINAL_HEIGHT;
	v2->z = (v2->z / v2->w);
	
	v3->x = (v3->x / v3->w) * _HALF_TERMINAL_WIDTH;
	v3->y = (v3->y / v3->w) * _HALF_TERMINAL_HEIGHT;
	v3->z = (v3->z / v3->w);
}

// TODO: implement complete rasterizer, so far only edges are drawn
_FORCE_INLINE void _stage_rasterization_triangle(vec4* v1, vec4* v2, vec4* v3) {
	_draw_triangle_edges(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y);
}