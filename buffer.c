#include "buffer.h"

#define _DEPTH_MAX 3.402823466E38F

#define _get_buff_elem_cnt(buff) (buff->width * buff->height + 1) 
#define _get_buff_depth_cnt(buff) (buff->width * buff->height)

int init_buffer(_core_buffer* buff, size_t width, size_t height) {
	ASSERT(buff != NULL, "Operation init on invalid null pointer");
	ASSERT(sizeof(_BUFF_ELEM_TYPE) == sizeof(wchar_t), "Only underlaying wchar_t type supported.");

	if (sizeof(_BUFF_ELEM_TYPE) == sizeof(wchar_t))
		setlocale(LC_ALL, "");

	buff->width = width;
	buff->height = height;
	buff->xcenter = width / 2;
	buff->ycenter = height / 2;
	buff->mem = malloc(_get_buff_elem_cnt(buff) * sizeof(_BUFF_ELEM_TYPE));
	buff->depth = malloc(_get_buff_depth_cnt(buff) * sizeof(_BUFF_DEPTH_PREC_TYPE));

	if (buff->mem == NULL) {
		fprintf(stderr, "Error while getting heap memory for buffer");
		return -1;
	}

	clear_buffer(buff);

	return 0;
}

int resize_buffer(_core_buffer* buff, size_t width, size_t height) {
	close_buffer(buff);
	int code = init_buffer(buff, width, height);
	return code;
}

_FORCE_INLINE void _clear_depth_buff(_core_buffer* buff) {
	for (UINT i = 0; i < _get_buff_depth_cnt(buff); i++)
		buff->depth[i] = (_BUFF_DEPTH_PREC_TYPE)_DEPTH_MAX;
}

void clear_buffer(_core_buffer* buff) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operation clear on null buffer"); 
	memset(buff->mem, 0, sizeof(_BUFF_ELEM_TYPE) * (_get_buff_elem_cnt(buff) - 1));
	_clear_depth_buff(buff);
}

void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operator clear_with on null buffer");
	wmemset(buff->mem, c, _get_buff_elem_cnt(buff) - 1);
	_clear_depth_buff(buff);
}

void close_buffer(_core_buffer* buff) {
	if (buff != NULL && buff->mem != NULL) {
		free(buff->mem);
		memset(buff, '\0', sizeof(_core_buffer));
	}
}

void set(_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, _BUFF_ELEM_TYPE c) {
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;
	
	if (!(y < buff->height && x < buff->width && y >= 0 && x >= 0))
		return;

	UINT idx = buff->width * y + x;
	if (buff->depth[idx] > d) {
		buff->depth[idx] = d;
		buff->mem[idx] = c;
	}
}

void set_force(_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, _BUFF_ELEM_TYPE c) {
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;

	if (!(y < buff->height && x < buff->width && y >= 0 && x >= 0))
		return;
	
	UINT idx = buff->width * y + x;
	buff->depth[idx] = d;
	buff->mem[idx] = c;
}

_BUFF_DEPTH_PREC_TYPE get_depth(_core_buffer* buff, int x, int y) {
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;

	if (!(y < buff->height && x < buff->width && y >= 0 && x >= 0))
		return;
	
	UINT idx = buff->width * y + x;
	return buff->depth[idx];
}

void flush_buffer(_core_buffer* buff) {
	ASSERT(buff->mem != NULL, "Operation flush on null buffer");
	//attron(COLOR_PAIR(1));
	mvaddwstr(0, 0, buff->mem);
	//attroff(COLOR_PAIR(1));
	refresh(); 
}