#include "ctx.h"
#include "thread.h"
#include "terminal.h"
#include "cursor.h"
#include "sig.h"
#include "render.h"
#include "charmap.h"

void _terminate() {
	_terminal.over = true;

	if (_terminal.fullscreen)
		unmake_terminal_fullscreen();
	
	if (_terminal.raw_mode)
		disable_raw_mode();

	if (_terminal.focus_events)
		disable_focus_events();

	if (!_terminal.console_cursor)
		enable_console_cursor();

	if (!_cursor.visible)
		show_cursor();

	close_mode();

	exit(EXIT_FAILURE);
}

void init_mode() {
	_init_terminal_state();
	
	_init_char_map();

	_init_flush_ctx();
	_close_flush_ctx();

	set_render_mode(RENDER_MODE_SOLID);

	_setup_killers_signal(_terminate);
	_setup_resize_signal(_update_terminal_size);
}

void close_mode() {
	_close_terminal_state();
	_close_all_threads();
}