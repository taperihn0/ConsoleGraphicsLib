#include "common.h"

#include <termios.h>
#include <fcntl.h>

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);	
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disableRawMode() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
	// go back to canonical mode, VMIN and VTIME are ignored since then
    original.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

int main() {
    enableRawMode();

    char c;
    while (1) {
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == 'q') 
            	break;
            printf("You pressed: %c\n", c);
        }
    }

    disableRawMode();
}