#include "render_utils.h"
#include "render.h"
#include "charmap.h"
#include <math.h>

#define _LINE_POINT '.'
#define _LINE_DEPTH 0.

#define _plot(x, y, c, d) \
set_elem((x), (y), (c), (d));

/*
	YouTube video by NoBS Code explaining the magics behind 
	Bresenham's algorithm:
	https://youtu.be/CceepU1vIKo?si=amC_5dnMf-8AiGcM
	Also Wikipedia pages helped a little bit:
	https://en.wikipedia.org/wiki/Line_drawing_algorithm
*/

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
		_plot(i, y, _LINE_POINT, _LINE_DEPTH);
		
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
		_plot(x, i, _LINE_POINT, _LINE_DEPTH);
		
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

/*
	The code below is responsible for rasterizing a triangle.
	Read an article for general knowledge about razterizing an image:
	https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer.html
*/

/*
	Triangle consists of 3 points: A1, A2, A3.
	Vectors included here are base information
	about triangle as they are used for calculations
	(rasterization and interpolation)
*/

typedef struct _triangle_data {
	vec2
		a1a2, 
		a1a3,
		a2a3,
		a2a1,
		a3a1,
		a3a2;
} _triangle_data;

/*
	Article: 
	https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer-part-2.html
	Read for further explanations.
*/

_FORCE_INLINE bool _is_inside_triangle(
	vec2* a1p, vec2* a2p, vec2* a3p, 
	_triangle_data*  triangle) 
{
	float det1 = CROSSPROD_2D(*a1p, triangle->a1a2);
	float det2 = CROSSPROD_2D(*a2p, triangle->a2a3);
	float det3 = CROSSPROD_2D(*a3p, triangle->a3a1);
	
	return (det1 >= 0.f && det2 >= 0.f && det3 >= 0.f) || 
		(det1 <= 0.f && det2 <= 0.f && det3 <= 0.f);
}

/*
	Another article, covering interpolation policy:
	https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer-part-3.html
*/

void _interpolate_data(
	vec2* a1p, vec2* a2p, vec2* a3p,
	_triangle_data* triangle, 
	float b1, float b2, float b3, CHAR_T* col,
	float d1, float d2, float d3, float* depth) 
{
	float det1 = CROSSPROD_2D(triangle->a2a1, triangle->a2a3);
	float det2 = CROSSPROD_2D(triangle->a3a1, triangle->a3a2);
	float det3 = CROSSPROD_2D(triangle->a1a2, triangle->a1a3);

	float fac1 = det1 == 0.f ? 0.f : CROSSPROD_2D(*a2p, triangle->a2a3) / det1;
	float fac2 = det2 == 0.f ? 0.f : CROSSPROD_2D(triangle->a3a1, *a3p) / det2;
	float fac3 = det3 == 0.f ? 0.f : CROSSPROD_2D(triangle->a1a2, *a1p) / det3;
	
	float interp_b = fac1 * b1 + fac2 * b2 + fac3 * b3;
	*col = _char_by_brightness(interp_b);
	
	float interp_d = fac1 * d1 + fac2 * d2 + fac3 * d3;
	*depth = interp_d;
}

void _draw_triangle_solid(
	float x1, float y1, float z1, 
	float x2, float y2, float z2, 
	float x3, float y3, float z3,
	float b1, float b2, float b3) 
{
	_triangle_data triangle;

	triangle.a1a2 = vec2f(x2 - x1, y2 - y1);
	triangle.a2a3 = vec2f(x3 - x2, y3 - y2);
	triangle.a3a1 = vec2f(x1 - x3, y1 - y3);
	triangle.a2a1 = vec2f(x1 - x2, y1 - y2);
	triangle.a3a2 = vec2f(x2 - x3, y2 - y3);
	triangle.a1a3 = vec2f(x3 - x1, y3 - y1);
	
	int l = minof3(x1, x2, x3);
	int u = minof3(y1, y2, y3);
	int r = maxof3(x1, x2, x3);
	int d = maxof3(y1, y2, y3);
	
	vec2 a1p, a2p, a3p;

	for (int x = l; x <= r; x++) {
		for (int y = u; y <= d; y++) {
			a1p = vec2f(x - x1, y - y1);
			a2p = vec2f(x - x2, y - y2);
			a3p = vec2f(x - x3, y - y3);

			if (_is_inside_triangle(&a1p, &a2p, &a3p, &triangle)) {
				CHAR_T col;
				float depth;

				_interpolate_data(
					&a1p, &a2p, &a3p,
					&triangle,
					b1, b2, b3, &col,
					z1, z2, z3, &depth);

				_plot(x, y, col, depth);
			}
		}
	}
}