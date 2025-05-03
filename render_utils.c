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

#define _plot_with_col(x, y, c, d, col) \
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

_STATIC _FORCE_INLINE bool _is_inside_triangle(
	vec2* a1p, vec2* a2p, vec2* a3p, _triangle_data*  triangle) 
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

_STATIC _FORCE_INLINE void _barycentric_coords(
	vec2* a1p, vec2* a2p, vec2* a3p,
	_triangle_data* triangle, 
	vec3* cords) 
{
	float det1 = CROSSPROD_2D(triangle->a2a1, triangle->a2a3);
	float det2 = CROSSPROD_2D(triangle->a3a1, triangle->a3a2);
	float det3 = CROSSPROD_2D(triangle->a1a2, triangle->a1a3);

	cords->x = det1 == 0.f ? 0.f : CROSSPROD_2D(*a2p, triangle->a2a3) / det1;
	cords->y = det2 == 0.f ? 0.f : CROSSPROD_2D(triangle->a3a1, *a3p) / det2;
	cords->z = det3 == 0.f ? 0.f : CROSSPROD_2D(triangle->a1a2, *a1p) / det3;	
}

#define _COL_BRIGHTNESS(col) (((col)->x + (col)->y + (col)->z) / 3.f)

void _draw_triangle_solid(
	vec4* v1, vec4* v2, vec4* v3,
	vec3* col1, vec3* col2, vec3* col3,
	vec3* norm1, vec3* norm2, vec3* norm3,
	func_stage_fragment stage_fragment,
	void* attrib)
{
	_triangle_data triangle;

	triangle.a1a2 = vec2f(v2->x - v1->x, v2->y - v1->y);
	triangle.a2a3 = vec2f(v3->x - v2->x, v3->y - v2->y);
	triangle.a3a1 = vec2f(v1->x - v3->x, v1->y - v3->y);
	triangle.a2a1 = vec2f(v1->x - v2->x, v1->y - v2->y);
	triangle.a3a2 = vec2f(v2->x - v3->x, v2->y - v3->y);
	triangle.a1a3 = vec2f(v3->x - v1->x, v3->y - v1->y);

	int half_width = get_terminal_width() / 2;
	int half_height = get_terminal_height() / 2;

	int l = max(minof3(v1->x, v2->x, v3->x), -half_width);
	int u = max(minof3(v1->y, v2->y, v3->y), -half_height);
	int r = min(maxof3(v1->x, v2->x, v3->x), half_width);
	int d = max(maxof3(v1->y, v2->y, v3->y), half_height);
	
	vec2 a1p, a2p, a3p;

	float z, divisor, curr_z;
	CHAR_T ch;
	vec3 cords, norm, rgb;
	_entry_t normalized;
	
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
				z =  v1->z * cords.x + v2->z * cords.y + v3->z * cords.z;
				
				curr_z = get_depth(_get_current_buffer(&_dbl_buff), x, y);

				if (curr_z < z)
					continue;

				// Division by homogeneus coordinate, as stated on page 427
				cords.x /= v1->w;
				cords.y /= v2->w;
				cords.z /= v3->w;
				divisor = 1.f / (cords.x + cords.y + cords.z);
				
				rgb = vec3f(
					cords.x * col1->x + cords.y * col2->x + cords.z * col3->x,
					cords.x * col1->y + cords.y * col2->y + cords.z * col3->y,
					cords.x * col1->z + cords.y * col2->z + cords.z * col3->z);
				
				rgb = mult_av3(divisor, &rgb);

				norm = vec3f(
					cords.x * norm1->x + cords.y * norm2->x + cords.z * norm3->x,
					cords.x * norm1->y + cords.y * norm2->y + cords.z * norm3->y,
					cords.x * norm1->z + cords.y * norm2->z + cords.z * norm3->z);

				norm = mult_av3(divisor, &norm);
				
				// passing interpolated data in a form of entry
				normalized = _entry_from(x, y, z, &rgb, &norm);
				stage_fragment(&normalized, attrib);

				float brightness = min(_COL_BRIGHTNESS(_ENTRY_COL(&normalized)), 1.f);

				_ncurses_pair_id col_num = color_pair_id(rgb.x, rgb.y, rgb.z);

				ch = _char_by_brightness(brightness);
				_plot_with_col(x, y, ch, z, col_num);
			}
		}
	}
}