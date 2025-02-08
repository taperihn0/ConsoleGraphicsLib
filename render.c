#include "render.h"
#include "terminal.h"
#include "gmath.h"

#define _plot(x, y, z, c) set(&_terminal.buff, (x), (y), (z), (c))

void clear_terminal(CHAR_T c) {
	clear_buffer_with(&_terminal.buff, c);
}

void flush_terminal() {
	flush_buffer(&_terminal.buff);
	_sync_with_next_frame();
}

extern void _pipeline(vec3*, size_t cnt);

int draw_buffer(buff_idx_t id) {
	size_t size;
	vec3* mem = (vec3*)get_mem_buff(&size, id);

	if (mem == NULL) {
		fprintf(stderr, "Invalid buffer index (null buffer)\n");
		return -1;
	}
	
	size_t cnt = size / sizeof(vec3);
	_pipeline(mem, cnt);
	
	return 0;
}

_FORCE_INLINE void _stage_vertex(vec3* mem, size_t cnt) {
	mat3 m;
	memset(m.rc, 0, sizeof(m.rc));
	static int i = 0;
	i++;
	m.rc[0][0] = cos(i / 120.f);
	m.rc[0][1] = sin(i / 120.f);
	m.rc[1][0] = -sin(i / 120.f);
	m.rc[1][1] = cos(i / 120.f);
	m.rc[2][2] = 1;
	
	/*
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			printf("%f ", m.rc[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
	raise(SIGTERM);
	exit(0);*/
	
	for (UINT i = 0; i < cnt; i++) {
		vec3 v = mult_mv3(&m, &mem[i]);
		//printf("%d %d %d\n",(int)roundl(v.x), (int)roundl(v.y), (int)roundl(v.z));
		_plot((int)roundl(v.x), (int)roundl(v.y), (int)roundl(v.z), '0');
	}
}

_FORCE_INLINE void _pipeline(vec3* mem, size_t cnt) {
	_stage_vertex(mem, cnt);
}