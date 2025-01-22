#include "cursor.h"
#include <sys/wait.h>
#include <signal.h>

static pid_t pid;

int hide_cursor() {
	pid = fork();
	
	if (pid == -1) {
		fprintf(stderr, "Cannot fork: %s", strerror(errno));
		return -1;
	}

	if (pid == 0) {
		int res = execlp("unclutter", "unclutter", "-jitter", "1000000", NULL);

		if (res == -1) {
			fprintf(stderr, "execlp for unclutter failed: %s", strerror(errno));
			return -1;
		}
	}

	return pid;
}

void show_cursor() {
	if (pid > 0) {
		kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
	}
}