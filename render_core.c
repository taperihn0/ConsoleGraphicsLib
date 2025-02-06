#include "render_core.h"
#include <math.h>

#define _UNICODE_SOLID (CHAR_T)(0x00002588)

#define _plot(x, y, c) set(&_terminal.buff, (x), (y), (c))


// YouTube video by NoBS Code explaining the magics behind 
// Bresenham's algorithm:
// https://youtu.be/CceepU1vIKo?si=amC_5dnMf-8AiGcM
//
// Also Wikipedia pages helped a little bit:
// https://en.wikipedia.org/wiki/Line_drawing_algorithm

static _FORCE_INLINE void _draw_line_horizontal(UINT x1, UINT y1, UINT x2, UINT y2) {
	if (x2 < x1) {
		swap(&x2, &x1);
		swap(&y2, &y1);
	}

	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = 2 * dy - dx;

	int iy = dy < 0 ? -1 : 1;
	dy *= iy;

	UINT y = y1;
	for (UINT i = x1; i <= x2; i++) {
		_plot(i, y, _UNICODE_SOLID);
		
		if (d > 0) {
			y += iy;
			d = d - 2 * dx;
		}
		d = d + 2 * dy;
	}
}

static _FORCE_INLINE void _draw_line_vertical(UINT x1, UINT y1, UINT x2, UINT y2) {
	if (y2 < y1) {
		swap(&x2, &x1);
		swap(&y2, &y1);
	}

	int dy = y2 - y1;
	int dx = x2 - x1;
	int d = 2 * dx - dy;

	int ix = dx < 0 ? -1 : 1;
	dx *= ix;

	UINT x = x1;
	for (UINT i = y1; i <= y2; i++) {
		_plot(x, i, _UNICODE_SOLID);
		
		if (d > 0) {
			x += ix;
			d = d - 2 * dy;
		}
		d = d + 2 * dx;
	}
}

void _draw_line(UINT x1, UINT y1, UINT x2, UINT y2) {
	if (abs(x1 - x2) > abs(y1 - y2))
		_draw_line_horizontal(x1, y1, x2, y2);
	else 
		_draw_line_vertical(x1, y1, x2, y2);
}

void _draw_triangle_edges(UINT x1, UINT y1, UINT x2, UINT y2, UINT x3, UINT y3) {
	_draw_line(x1, y1, x2, y2);
	_draw_line(x2, y2, x3, y3);
	_draw_line(x3, y3, x1, y1);
}