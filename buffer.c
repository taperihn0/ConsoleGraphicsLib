#include "buffer.h"
#include <locale.h>

#define _get_buff_elem_cnt(buff) (buff->width * buff->height + 1) 

int init_buffer(_core_buffer* buff, size_t width, size_t height) {
	ASSERT(buff != NULL, "Operation init on invalid null pointer");
	ASSERT(sizeof(_BUFF_ELEM_TYPE) == sizeof(wchar_t), "Only underlaying wchar_t type supported.");

	setlocale(LC_ALL, "");

	buff->width = width;
	buff->height = height;
	buff->mem = malloc(_get_buff_elem_cnt(buff) * sizeof(_BUFF_ELEM_TYPE));

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

void clear_buffer(_core_buffer* buff) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operation clear on null buffer"); 
	memset(buff->mem, 0, sizeof(_BUFF_ELEM_TYPE) * (_get_buff_elem_cnt(buff) - 1));
}

void clear_buffer_with(_core_buffer* buff, _BUFF_ELEM_TYPE c) {
	ASSERT(buff != NULL && buff->mem != NULL, "Operator clear_with on null buffer");
	wmemset(buff->mem, c, _get_buff_elem_cnt(buff) - 1);
}

void close_buffer(_core_buffer* buff) {
	if (buff != NULL && buff->mem != NULL) {
		free(buff->mem);
		memset(buff, '\0', sizeof(_core_buffer));
	}
}

void set(_core_buffer* buff, size_t x, size_t y, _BUFF_ELEM_TYPE c) {
	ASSERT(y < buff->height && x < buff->width, "Invalid buffer element position.");
	ASSERT(buff != NULL, "Trying to fetch from null buffer");
	buff->mem[buff->width * y + x] = c;
}

void flush_buffer(_core_buffer* buff) {
	ASSERT(buff->mem != NULL, "Operation flush on null buffer");
	system("clear");
	printf("%S", buff->mem);
	fflush(stdout);
}