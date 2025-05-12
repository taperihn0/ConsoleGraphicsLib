#include "render_utils.h"
#include "charmap.h"
#include "terminal.h"
#include "render.h"
#include "color.h"
#include <math.h>

#define _LINE_POINT '.'
#define _LINE_DEPTH 0.

#define _plot(x, y, c, d) \
set_elem((x), (y), (c), (d), COLOR_WHITE + 1);

#define _plot_with_col(x, y, d, c, col) \
set_elem_force((x), (y), (c), (d), (col));

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

_STATIC _FORCE_INLINE void _draw_line(
	vec4* v1, vec4* v2,
	_UNUSED vec3* col1, _UNUSED vec3* col2,
	_UNUSED vec3* norm1, _UNUSED vec3* norm2,
	_UNUSED func_stage_fragment stage_fragment,
	_UNUSED void* attrib) 
{
	vec2 rv1xy = vec2f(roundf(v1->x), roundf(v1->y));
	vec2 rv2xy = vec2f(roundf(v2->x), roundf(v2->y));

	if (fabs(rv1xy.x - rv2xy.x) > fabs(rv1xy.y - rv2xy.y))
		_draw_line_horizontal(rv1xy.x, rv1xy.y, rv2xy.x, rv2xy.y);
	else 
		_draw_line_vertical(rv1xy.x, rv1xy.y, rv2xy.x, rv2xy.y);
}

void _draw_triangle_edges(
	vec4* v1, vec4* v2, vec4* v3,
	vec3* col1, vec3* col2, vec3* col3,
	vec3* norm1, vec3* norm2, vec3* norm3,
	func_stage_fragment stage_fragment,
	void* attrib) 
{
	_draw_line(v1, v2, col1, col2, norm1, norm2, stage_fragment, attrib);
	_draw_line(v2, v3, col2, col3, norm2, norm3, stage_fragment, attrib);
	_draw_line(v3, v1, col3, col1, norm3, norm1, stage_fragment, attrib);
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
	/*
		Determinant vector [x, y, z], where:
		  x: CROSS2F(a2a1, a2a3);
		  y: CROSS2F(a3a1, a3a2);
		  x: CROSS2F(a1a2, a1a3);
		Used for calculating barycentric coordinates.
	*/
	vec3 det;

	vec2 a1a2, 
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

_STATIC _FORCE_INLINE bool _is_inside_triangle(
	vec2* a1p, vec2* a2p, vec2* a3p, _triangle_data* triangle) 
{
	vec3 detp;
#if !defined(MATH_EXTENSIONS) || !defined(_SIMD_SEE)
	detp = vec3f(CROSS2F(a1p, &triangle->a1a2),
	             CROSS2F(a2p, &triangle->a2a3),
	             CROSS2F(a3p, &triangle->a3a1));
#else
	detp = mext_cross2fx3(a1p, &triangle->a1a2,
	                      a2p, &triangle->a2a3,
	                      a3p, &triangle->a3a1);
#endif
	return (detp.x > 0.f && detp.y > 0.f && detp.z > 0.f) || 
			 (detp.x < 0.f && detp.y < 0.f && detp.z < 0.f);
}

/*
	Another article, covering interpolation policy:
	https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer-part-3.html
*/

_STATIC _FORCE_INLINE void _barycentric_coords(
	vec2* a1p, vec2* a2p, vec2* a3p,
	_triangle_data* triangle, vec3* cords) 
{
#if !defined(MATH_EXTENSIONS) || !defined(_SIMD_SEE)
	cords->x = CROSS2F(a2p, &triangle->a2a3) / triangle->det.x;
	cords->y = CROSS2F(&triangle->a3a1, a3p) / triangle->det.y;
	cords->z = CROSS2F(&triangle->a1a2, a1p) / triangle->det.z;	
#else
	*cords = mext_cross2fx3(a2p, &triangle->a2a3,
	                        &triangle->a3a1, a3p,
	                        &triangle->a1a2, a1p);
	*cords = div_v3(cords, &triangle->det);
#endif
}

void _draw_triangle_solid(
	vec4* v1, vec4* v2, vec4* v3,
	vec3* col1, vec3* col2, vec3* col3,
	vec3* norm1, vec3* norm2, vec3* norm3,
	func_stage_fragment stage_fragment,
	void* attrib)
{
	_triangle_data triangle;

	triangle.a1a2 = vec2f(v2->x - v1->x, v2->y - v1->y);
	
	// Triangle is a straight from a1 to a3
	if (LENGTHSQ2F(&triangle.a1a2) == 0.f) {
		_draw_line(v1, v3, NULL, NULL, NULL, NULL, NULL, NULL);
		return;
	}

	triangle.a2a3 = vec2f(v3->x - v2->x, v3->y - v2->y);

	// straight from a1 to a2
	if (LENGTHSQ2F(&triangle.a2a3) == 0.f) {
		_draw_line(v1, v2, NULL, NULL, NULL, NULL, NULL, NULL);
		return;
	}

	triangle.a3a1 = vec2f(v1->x - v3->x, v1->y - v3->y);

	// straight from a1 to a2
	if (LENGTHSQ2F(&triangle.a3a1) == 0.f) {
		_draw_line(v1, v2, NULL, NULL, NULL, NULL, NULL, NULL);
		return;
	}

	triangle.a2a1 = vec2f(v1->x - v2->x, v1->y - v2->y);
	triangle.a3a2 = vec2f(v2->x - v3->x, v2->y - v3->y);
	triangle.a1a3 = vec2f(v3->x - v1->x, v3->y - v1->y);

#if !defined(MATH_EXTENSIONS) || !defined(_SIMD_SEE)
	triangle.det.x = CROSS2F(&triangle.a2a1, &triangle.a2a3);
	triangle.det.y = CROSS2F(&triangle.a3a1, &triangle.a3a2);
	triangle.det.z = CROSS2F(&triangle.a1a2, &triangle.a1a3);
#else
	triangle.det = mext_cross2fx3(&triangle.a2a1, &triangle.a2a3,
	                              &triangle.a3a1, &triangle.a3a2,
	                              &triangle.a1a2, &triangle.a1a3);
#endif

	const int half_width = get_terminal_width() / 2;
	const int half_height = get_terminal_height() / 2;

	const int l = max(minof3(v1->x, v2->x, v3->x), -half_width);
	const int u = max(minof3(v1->y, v2->y, v3->y), -half_height);
	const int r = min(maxof3(v1->x, v2->x, v3->x), half_width);
	const int d = max(maxof3(v1->y, v2->y, v3->y), half_height);
	
	vec3 v1wv2wv3w = vec3f(v1->w, v2->w, v3->w);
	vec3 v1zv2zv3z = vec3f(v1->z, v2->z, v3->z);
	vec3 c1xc2xc3x = vec3f(col1->x, col2->x, col3->x);
	vec3 c1yc2yc3y = vec3f(col1->y, col2->y, col3->y);
	vec3 c1zc2zc3z = vec3f(col1->z, col2->z, col3->z);
	vec3 n1xn2xn3x = vec3f(norm1->x, norm2->x, norm3->x);
	vec3 n1yn2yn3y = vec3f(norm1->y, norm2->y, norm3->y);
	vec3 n1zn2zn3z = vec3f(norm1->z, norm2->z, norm3->z);

	vec2 a1p, a2p, a3p;
	vec3 cords, norm, rgb;
	
	// Optimization:
	// while interating through row and getting out of the 
	// triangle, there is no sense no iterate more, since 
	// I'm guaranteed to not getting inside the triangle again.
	bool found, fill;

	for (int y = u; y <= d; y++) {
		found =  false;
		fill = false;

		for (int x = l; x <= r && (!found || fill); x++) {
			a1p = vec2f(x - v1->x, y - v1->y);
			a2p = vec2f(x - v2->x, y - v2->y);
			a3p = vec2f(x - v3->x, y - v3->y);

			fill = false;
			if (_is_inside_triangle(&a1p, &a2p, &a3p, &triangle)) {
				found = true;
				fill = true;
				_barycentric_coords(&a1p, &a2p, &a3p, &triangle, &cords);
				
				// Interpolation done as stated in OpenGL specification, page 427:
				// https://registry.khronos.org/OpenGL/specs/gl/glspec44.core.pdf

				// interpolating depth without w division
				const float z = dot3f(&cords, &v1zv2zv3z);
				const float curr_z = get_depth(_get_current_buffer(&_dbl_buff), x, y);

				if (curr_z < z)
					continue;

				// Division by homogeneus coordinate, as stated on page 427
				cords = div_v3(&cords, &v1wv2wv3w);
				const float div = cords.x + cords.y + cords.z;
				
				rgb = vec3f(dot3f(&cords, &c1xc2xc3x),
				            dot3f(&cords, &c1yc2yc3y),
				            dot3f(&cords, &c1zc2zc3z));
				
				rgb = div_av3(div, &rgb);

				norm = vec3f(dot3f(&cords, &n1xn2xn3x),
				             dot3f(&cords, &n1yn2yn3y),
				             dot3f(&cords, &n1zn2zn3z));

				norm = div_av3(div, &norm);
				
				// passing interpolated data in a form of entry
				_entry_t normalized = _entry_from(x, y, z, &rgb, &norm);
				stage_fragment(&normalized, attrib);

				const float brightness = min(_COL_BRIGHTNESS(_ENTRY_COL(&normalized)), 
				                             1.f);
				_plot_with_col(x, y, z, 
				               _char_by_brightness(brightness), 
				               _color_by_rgb(&rgb));
			}
		}
	}
}