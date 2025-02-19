#pragma once

#include "common.h"
#include <signal.h>

#define _SIG_STANDARD_MIN 1
#define _SIG_STANDARD_MAX 31

typedef void (*handler_t)();

static int _setup_killers_signal(handler_t handler) {
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handler;
	
	int r = 0;

	// specify original signals
	for (int sig = _SIG_STANDARD_MIN; sig <= _SIG_STANDARD_MAX; sig++) {
		if (sig == SIGWINCH || sig == SIGSTOP || sig == SIGKILL || 
			sig == SIGCHLD || sig == SIGCLD)
			continue;

		if (sigaction(sig, &sa, NULL) == -1) {
			fprintf(stderr, "Could not register signal %d: %s\n", sig, strerror(errno));
			r++;
		}
	}
	
	// specify real time signals
	for (int sig = SIGRTMIN; sig <= SIGRTMAX; sig++) {
		if (sigaction(sig, &sa, NULL) == -1) {
			fprintf(stderr, "Could not register signal %d: %s\n", sig, strerror(errno));
			r++;
		}
	}
	
	return r;
}

static int _setup_resize_signal(handler_t handler) {
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handler;

	int r = sigaction(SIGWINCH, &sa, NULL);

	if (r == -1) {
		fprintf(stderr, "Could not register signal SIGWINCH (%d): %s", SIGWINCH, strerror(errno));
		return -1;
	}

	return 0;
}