#include "buffer.h"

#define _DEPTH_MAX 3.402823466E38F

#define _MAX_ELEM_CNT 256 * 256

#define _get_buff_elem_cnt(buff)  (buff->width * buff->height)
#define _get_buff_depth_cnt(buff) (buff->width * buff->height)
#define _get_buff_color_cnt(buff) (buff->width * buff->height)

int init_buffer(_core_buffer* buff, size_t width, size_t height) {
	ASSERT(buff != NULL, "Operation init on invalid null pointer");
	ASSERT(sizeof(_BUFF_ELEM_TYPE) == sizeof(wchar_t), "Only underlaying wchar_t type supported.");
	ASSERT(width * height < _MAX_ELEM_CNT, "Buffer size overflow");

	if (sizeof(_BUFF_ELEM_TYPE) == sizeof(wchar_t))
		setlocale(LC_ALL, "");

	buff->width = width;
	buff->height = height;
	buff->xcenter = width / 2;
	buff->ycenter = height / 2;
	buff->mem = malloc(_get_buff_elem_cnt(buff) * sizeof(_BUFF_ELEM_TYPE));
	buff->depth = malloc(_get_buff_depth_cnt(buff) * sizeof(_BUFF_DEPTH_PREC_TYPE));
	buff->col_pair_num = malloc(_get_buff_color_cnt(buff) * sizeof(_ncurses_pair_id));

	if (buff->mem == NULL || buff->depth == NULL || buff->col_pair_num == NULL) {
		fprintf(stderr, "Error while getting heap memory for buffer (%s\n)", strerror(errno));
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

_FORCE_INLINE void _clear_color_buff(_core_buffer* buff) {
	for (UINT i = 0; i < _get_buff_color_cnt(buff); i++)
		buff->col_pair_num[i] = (_ncurses_pair_id)0;
}

void clear_buffer(_core_buffer* buff) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operation clear on null buffer"); 
	memset(buff->mem, 0, sizeof(_BUFF_ELEM_TYPE) * (_get_buff_elem_cnt(buff)));
	_clear_depth_buff(buff);
	_clear_color_buff(buff);
}

void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operator clear_with on null buffer");
	wmemset(buff->mem, c, _get_buff_elem_cnt(buff));
	_clear_depth_buff(buff);
	_clear_color_buff(buff);
}

void close_buffer(_core_buffer* buff) {
	if (buff != NULL) {
		free(buff->mem);
		free(buff->depth);
		free(buff->col_pair_num);
		memset(buff, '\0', sizeof(_core_buffer));
	}
}

void set(
	_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, 
	_BUFF_ELEM_TYPE c, _ncurses_pair_id col) 
{
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;
	
	if (!(y < (int)buff->height && x < (int)buff->width && y >= 0 && x >= 0))
		return;

	UINT idx = buff->width * y + x;
	if (buff->depth[idx] > d) {
		buff->depth[idx] = d;
		buff->mem[idx] = c;
		buff->col_pair_num[idx] = col;
	}
}

void set_force(
	_core_buffer* buff, int x, int y, _BUFF_DEPTH_PREC_TYPE d, 
	_BUFF_ELEM_TYPE c, _ncurses_pair_id col) 
{
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;

	if (!(y < (int)buff->height && x < (int)buff->width && y >= 0 && x >= 0))
		return;
	
	UINT idx = buff->width * y + x;
	buff->depth[idx] = d;
	buff->mem[idx] = c;
	buff->col_pair_num[idx] = col;
}

_BUFF_DEPTH_PREC_TYPE get_depth(_core_buffer* buff, int x, int y) {
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	x += buff->xcenter;
	y = buff->ycenter - y;

	if (!(y < (int)buff->height && x < (int)buff->width && y >= 0 && x >= 0))
		return (_BUFF_DEPTH_PREC_TYPE)(-1.);
	
	UINT idx = buff->width * y + x;
	return buff->depth[idx];
}

void flush_buffer(_core_buffer* buff) {
	ASSERT(buff->mem != NULL, "Operation flush on null buffer");
	
	// ncurses-friendly format for character screen flushing
	static cchar_t scr_cchar[_MAX_ELEM_CNT];

	// wide character string must end with the 0 character, as 
	// stated in documentation: https://linux.die.net/man/3/setcchar
	#pragma omp parallel for
	for (UINT yx = 0; yx < _get_buff_elem_cnt(buff); yx++) {
		wchar_t wstr[2] = { buff->mem[yx], L'\0' };
		setcchar(&scr_cchar[yx], wstr, A_NORMAL, buff->col_pair_num[yx], NULL);
	}
	
	// mvadd_wchnstr is not thread-safe (!)
	#pragma omp parallel for
	for (UINT y = 0; y < buff->height; y++) {
		mvadd_wchnstr(y, 0, scr_cchar + y * buff->width, buff->width);
	}

	refresh();
}