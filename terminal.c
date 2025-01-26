#include "terminal.h"
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termcap.h>
#include <signal.h>

void init_terminal_state() {
	memset(&_terminal, 0, sizeof(_main_terminal));
	_update_terminal_size(&_terminal.width, &_terminal.height);
	_terminal.is_focus = true;

	struct sigaction sa;
    sa.sa_handler = _update_terminal_size;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGWINCH, &sa, NULL);
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
	char* comm = malloc(23 * sizeof(char) + sizeof(s));
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
        fprintf(stderr, "Could not access the termcap data base.\n");
        return;
    }

    _terminal.height = tgetnum("li");
    _terminal.width = tgetnum("co");
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

UINT get_terminal_width() {
	return _terminal.width;
}

UINT get_terminal_height() {
	return _terminal.height;
}