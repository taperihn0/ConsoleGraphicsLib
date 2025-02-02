#include "cursor.h"
#include <sys/wait.h>
#include <signal.h>

_main_cursor _cursor;

int hide_cursor() {
	_cursor.pid = fork();
	
	if (_cursor.pid == -1) {
		fprintf(stderr, "Cannot fork: %s", strerror(errno));
		return -1;
	}

	if (_cursor.pid == 0) {
		int res = execlp("./hhpc/hhpc", "./hhpc/hhpc", "-i", "500", NULL);

		if (res == -1) {
			fprintf(stderr, "execlp for hhpc failed: %s", strerror(errno));
			return -1;
		}
	}
	
	_cursor.visible = false;
	return _cursor.pid;
}

void show_cursor() {
	if (_cursor.pid > 0) {
		kill(_cursor.pid, SIGTERM);
		waitpid(_cursor.pid, NULL, 0);
		_cursor.visible = true;
	}
}