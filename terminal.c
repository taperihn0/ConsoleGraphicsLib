#include "terminal.h"
#include <termios.h>
#include <fcntl.h>

void make_terminal_fullscreen() {
    system("wmctrl -r :ACTIVE: -b add,fullscreen");
}

void unmake_terminal_fullscreen() {
	system("wmctrl -r :ACTIVE: -b remove,fullscreen");
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
}

void disable_raw_mode() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
	// go back to canonical mode, VMIN and VTIME are ignored since then
    original.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

void enable_focus_events() {
	system("echo -ne '\e[?1004h'");
}

void disable_focus_events() {
	system("echo -ne '\e[?1004l'");
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