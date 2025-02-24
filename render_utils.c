#include "render_utils.h"
#include "render.h"
#include "charmap.h"
#include <math.h>

// TEMPORARY
#define _UNICODE_POINT '.'
#define TEMP_LINE_DEPTH 0.
#define _plot(x, y, c, d) \
set_elem((x), (y), (c), (d));

// YouTube video by NoBS Code explaining the magics behind 
// Bresenham's algorithm:
// https://youtu.be/CceepU1vIKo?si=amC_5dnMf-8AiGcM
//
// Also Wikipedia pages helped a little bit:
// https://en.wikipedia.org/wiki/Line_drawing_algorithm

_STATIC _FORCE_INLINE void _draw_line_horizontal(int x1, int y1, int x2, int y2) {
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
		_plot(i, y, _UNICODE_POINT, TEMP_LINE_DEPTH);
		
		if (d > 0) {
			y += iy;
			d = d - 2 * dx;
		}
		d = d + 2 * dy;
	}
}

_STATIC _FORCE_INLINE void _draw_line_vertical(int x1, int y1, int x2, int y2) {
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
		_plot(x, i, _UNICODE_POINT, TEMP_LINE_DEPTH);
		
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

_FORCE_INLINE bool _is_inside_triangle(vec2_i* a1a2, vec2_i* a2a3, vec2_i* a3a1, 
	vec2_i* a1p, vec2_i* a2p, vec2_i* a3p) 
{
	int det1 = _VECTOR_MATRIX_DET(a1p, a1a2);
	int det2 = _VECTOR_MATRIX_DET(a2p, a2a3);
	int det3 = _VECTOR_MATRIX_DET(a3p, a3a1);
	
	return (det1 >= 0 && det2 >= 0 && det3 >= 0) || 
		(det1 <= 0 && det2 <= 0 && det3 <= 0);
}

#define _MIN3(a1, a2, a3) min(min(a1, a2), a3)
#define _MAX3(a1, a2, a3) max(max(a1, a2), a3)

void _interpolate(vec2_i* a2a1, vec2_i* a2a3, vec2_i* a2p, 
	vec2_i* a3a1, vec2_i* a3a2, vec2_i* a3p,
	vec2_i* a1a2, vec2_i* a1a3, vec2_i* a1p, 
	float b1, float b2, float b3,
	float d1, float d2, float d3,
	CHAR_T* col, _BUFF_DEPTH_PREC_TYPE* depth) 
{
	int det1 = _VECTOR_MATRIX_DET(a2a1, a2a3);
	int det2 = _VECTOR_MATRIX_DET(a3a1, a3a2);
	int det3 = _VECTOR_MATRIX_DET(a1a2, a1a3);

	float fac1 = det1 == 0.f ? 0.f : (float)_VECTOR_MATRIX_DET(a2p, a2a3) / det1;
	float fac2 = det2 == 0.f ? 0.f : (float)_VECTOR_MATRIX_DET(a3a1, a3p) / det2;
	float fac3 = det3 == 0.f ? 0.f : (float)_VECTOR_MATRIX_DET(a1a2, a1p) / det3;

	float interp_b = fac1 * b1 + fac2 * b2 + fac3 * b3;
	*col = _char_by_brightness(interp_b);
	
	float interp_d = fac1 * d1 + fac2 * d2 + fac3 * d3;
	*depth = interp_d;
}

void _draw_triangle_solid(int x1, int y1, int x2, int y2, int x3, int y3, 
	float b1, float b2, float b3,
	float d1, float d2, float d3) 
{
	vec2_i a1a2 = vec2i(x2 - x1, y2 - y1);
	vec2_i a2a3 = vec2i(x3 - x2, y3 - y2);
	vec2_i a3a1 = vec2i(x1 - x3, y1 - y3);
	vec2_i a2a1 = vec2i(x1 - x2, y1 - y2);
	vec2_i a3a2 = vec2i(x2 - x3, y2 - y3);
	vec2_i a1a3 = vec2i(x3 - x1, y3 - y1);

	vec2_i a1p, a2p, a3p;
	
	int l = _MIN3(x1, x2, x3);
	int u = _MIN3(y1, y2, y3);
	int r = _MAX3(x1, x2, x3);
	int d = _MAX3(y1, y2, y3);

	for (int x = l; x <= r; x++) {
		for (int y = u; y <= d; y++) {
			a1p = vec2i(x - x1, y - y1);
			a2p = vec2i(x - x2, y - y2);
			a3p = vec2i(x - x3, y - y3);

			if (_is_inside_triangle(&a1a2, &a2a3, &a3a1, &a1p, &a2p, &a3p)) {
				CHAR_T col;
				_BUFF_DEPTH_PREC_TYPE depth;

				_interpolate(
					&a2a1, &a2a3, &a2p, 
					&a3a1, &a3a2, &a3p,
					&a1a2, &a1a3, &a1p,
					b1, b2, b3,
					d1, d2, d3,
					&col, &depth);

				_plot(x, y, col, depth);
			}
		}
	}
}