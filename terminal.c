#include "terminal.h"
#include "timeman.h"
#include "render.h"
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termcap.h>

#define _MAX_FRAMES (UINT)(-1)

#define _get_terminal_elem_cnt() (_terminal.width * _terminal.height + 1)

_main_terminal _terminal;

void _init_terminal_state() {
	memset(&_terminal, 0, sizeof(_main_terminal));
	_update_terminal_size();

	_terminal.is_focus = true;
	_terminal.console_cursor = true;

	initscr();
}

void _close_terminal_state() {
	_close_buffers(&_dbl_buff);
	endwin();
}

void make_terminal_fullscreen() {
	system("wmctrl -r :ACTIVE: -b add,fullscreen");
	_terminal.fullscreen = true;
}

void unmake_terminal_fullscreen() {
	system("wmctrl -r :ACTIVE: -b remove,fullscreen");
	_terminal.fullscreen = false;
}

void set_terminal_title(const char* const s) {
	static size_t buff_size = 128 * sizeof(char);
	ASSERT(sizeof(s) + sizeof("wmctrl -r :ACTIVE: -T ") <= buff_size, "Title too long.");

	char* comm = malloc(buff_size);
	strcpy(comm, "wmctrl -r :ACTIVE: -T ");
	strcat(comm, s);
	
	system(comm);
	free(comm);
}

void enable_raw_mode() {
	struct termios raw;
	tcgetattr(STDIN_FILENO, &raw);

	raw.c_lflag &= ~(ECHO | ICANON);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &raw);	
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	_terminal.raw_mode = true;
}

void disable_raw_mode() {
	struct termios original;
	tcgetattr(STDIN_FILENO, &original);
	// go back to canonical mode, VMIN and VTIME are ignored since then
	original.c_lflag |= (ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &original);
	_terminal.raw_mode = false;
}

void enable_focus_events() {
	system("echo -ne '\e[?1004h'");
	_terminal.focus_events = true;
}

void disable_focus_events() {
	system("echo -ne '\e[?1004l'");
	_terminal.focus_events = false;
}

void _update_terminal_size() {
	static char termbuf[2048];
	char *termtype = getenv("TERM");

	if (tgetent(termbuf, termtype) < 0) {
		fprintf(stderr, "Could not access the termcap database.\n");
		return;
	}

	_terminal.height = tgetnum("li");
	_terminal.width = tgetnum("co");
	
	_resize_buffers(&_dbl_buff, _terminal.width, _terminal.height);
	resizeterm(_terminal.height, _terminal.width);
}

void enable_console_cursor() {
	printf("\e[?25h");
	fflush(stdout);
	_terminal.console_cursor = true;
}

void disable_console_cursor() {
	printf("\e[?25l");
	fflush(stdout);
	_terminal.console_cursor = false;
}

bool _check_focus() {
	static char c[3];
	static bool focus = true;

	ssize_t n = read(STDIN_FILENO, c, sizeof(c));
	
	if (n == sizeof(c)) {
		if (strncmp(c, "\e[O", 3) == 0)
			focus = false;
		else if (strncmp(c, "\e[I", 3) == 0)
			focus = true;
	}

	return focus;
}

void set_framerate_limit(UINT cnt) {
	UINT frame_rate = cnt > 0 ? cnt : _MAX_FRAMES;
	_terminal.microsec_delay = 1000000 / frame_rate;
}

void _sync_with_next_frame() {
	static utime_t last_tp = 0;
	utime_t curr_tp = gettime_mcs(CLOCK_MONOTONIC_RAW);

	utime_t microsec_diff = curr_tp - last_tp;

	useconds_t microsec_delay = _terminal.microsec_delay > microsec_diff ? 
						  		_terminal.microsec_delay - microsec_diff : 0u;
	
	usleep(microsec_delay);

	last_tp = curr_tp;
}

UINT get_terminal_width() {
	return _terminal.width;
}

UINT get_terminal_height() {
	return _terminal.height;
}

bool should_quit() {
	return _terminal.over;
}