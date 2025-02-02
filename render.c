#include "render.h"
#include "terminal.h"

void clear_terminal(CHAR_T c) {
	clear_buffer_with(&_terminal.buff, c);
}

void flush_terminal() {
	flush_buffer(&_terminal.buff);
	_sync_with_next_frame();
}