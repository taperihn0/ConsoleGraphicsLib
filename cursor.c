#include "cursor.h"
#include <sys/wait.h>
#include <signal.h>

#include <X11/X.h>
#include <X11/Xlib.h>

Display* dpy;
Cursor empty_cursor;

_INLINE Cursor nullCursor(Display *dpy, Drawable dw) {
    XColor color  = { 0 };
    const char data[] = { 0 };

    Pixmap pixmap = XCreateBitmapFromData(dpy, dw, data, 1, 1);
    Cursor cursor = XCreatePixmapCursor(dpy, pixmap, pixmap, &color, &color, 0, 0);

    XFreePixmap(dpy, pixmap);

    return cursor;
}

_main_cursor _cursor;

int hide_cursor() {
	dpy = XOpenDisplay(NULL);

	if (!dpy) {
		fprintf(stderr, "Failed to open display\n");
		return -1;
	}

	int scr = DefaultScreen(dpy);
   	Window win = RootWindow(dpy, scr);

    const unsigned int mask = PointerMotionMask | ButtonPressMask;
    empty_cursor = nullCursor(dpy, win);

	XGrabPointer(dpy, win, True, mask, GrabModeSync, GrabModeAsync, None, empty_cursor, CurrentTime);

	_cursor.visible = false;
	return 0;
}

void show_cursor() {
	XUngrabPointer(dpy, CurrentTime);
    XFreeCursor(dpy, empty_cursor);
    XCloseDisplay(dpy);

    _cursor.visible = true;
}