#include "render_utils.h"
#include "render.h"
#include <math.h>

// TEMPORARY
#define _UNICODE_POINT '.'
#define _UNICODE_SOLID '#'/*0x00002588*/
#define TEMP_LINE_DEPTH 0.
#define _plot(x, y, c) \
set_elem((x), (y), (c), 0.f);

// YouTube video by NoBS Code explaining the magics behind 
// Bresenham's algorithm:
// https://youtu.be/CceepU1vIKo?si=amC_5dnMf-8AiGcM
//
// Also Wikipedia pages helped a little bit:
// https://en.wikipedia.org/wiki/Line_drawing_algorithm

_FORCE_INLINE void _draw_line_horizontal(int x1, int y1, int x2, int y2) {
	if (x2 < x1) {
		swap(&x2, &x1);
		swap(&y2, &y1);
	}

	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = 2 * dy - dx;

	int iy = dy < 0 ? -1 : 1;
	dy *= iy;

	int y = y1;
	for (int i = x1; i <= x2; i++) {
		_plot(i, y, _UNICODE_POINT);
		
		if (d > 0) {
			y += iy;
			d = d - 2 * dx;
		}
		d = d + 2 * dy;
	}
}

_FORCE_INLINE void _draw_line_vertical(int x1, int y1, int x2, int y2) {
	if (y2 < y1) {
		swap(&x2, &x1);
		swap(&y2, &y1);
	}

	int dy = y2 - y1;
	int dx = x2 - x1;
	int d = 2 * dx - dy;

	int ix = dx < 0 ? -1 : 1;
	dx *= ix;

	int x = x1;
	for (int i = y1; i <= y2; i++) {
		_plot(x, i, _UNICODE_POINT);
		
		if (d > 0) {
			x += ix;
			d = d - 2 * dy;
		}
		d = d + 2 * dx;
	}
}

void _draw_line(int x1, int y1, int x2, int y2) {
	if (abs(x1 - x2) > abs(y1 - y2))
		_draw_line_horizontal(x1, y1, x2, y2);
	else 
		_draw_line_vertical(x1, y1, x2, y2);
}

void _draw_triangle_edges(int x1, int y1, int x2, int y2, int x3, int y3) {
	_draw_line(x1, y1, x2, y2);
	_draw_line(x2, y2, x3, y3);
	_draw_line(x3, y3, x1, y1);
}

// expands into an expression representing a sinus of an angle between
// given vectors in 2D space
#define _VECTOR_MATRIX_DET(v1, v2) (v1->x * v2->y - v1->y * v2->x)

_FORCE_INLINE bool _is_inside_triangle(vec2* a1a2, vec2* a2a3, vec2* a3a1, 
	vec2* a1p, vec2* a2p, vec2* a3p) 
{
	float sin1 = _VECTOR_MATRIX_DET(a1p, a1a2);
	float sin2 = _VECTOR_MATRIX_DET(a2p, a2a3);
	float sin3 = _VECTOR_MATRIX_DET(a3p, a3a1);

	return (sin1 > 0.f && sin2 > 0.f && sin3 > 0.f) || 
		(sin1 < 0.f && sin2 < 0.f && sin3 < 0.f);
}

#define _MIN3(a1, a2, a3) min(min(a1, a2), a3)
#define _MAX3(a1, a2, a3) max(max(a1, a2), a3)

void _draw_triangle_solid(int x1, int y1, int x2, int y2, int x3, int y3) {
	vec2 a1a2 = vec2f(x2 - x1, y2 - y1);
	vec2 a2a3 = vec2f(x3 - x2, y3 - y2);
	vec2 a3a1 = vec2f(x1 - x3, y1 - y3);

	vec2 a1p, a2p, a3p;
	
	int l = _MIN3(x1, x2, x3);
	int u = _MIN3(y1, y2, y3);
	int r = _MAX3(x1, x2, x3);
	int d = _MAX3(y1, y2, y3);

	for (int x = l; x <= r; x++) {
		for (int y = u; y <= d; y++) {
			a1p = vec2f(x - x1, y - y1);
			a2p = vec2f(x - x2, y - y2);
			a3p = vec2f(x - x3, y - y3);

			if (_is_inside_triangle(&a1a2, &a2a3, &a3a1, &a1p, &a2p, &a3p)) {
				_plot(x, y, _UNICODE_SOLID);
			}
		}
	}
}