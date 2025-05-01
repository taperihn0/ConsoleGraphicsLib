#include "color.h"
#include "coremath.h"
#include <limits.h>

typedef struct _predef_color {
	_color_t id;
	float r, g, b;
} _predef_color;

#define _PREDEF_COLOR_NUM 8

typedef struct _predef_color_list {
	_predef_color list[_PREDEF_COLOR_NUM];
} _predef_color_list;

_predef_color_list predef;

void _add_color_at(size_t idx, _color_t col, short r, short g, short b) {
	ASSERT(idx < _PREDEF_COLOR_NUM, "Invalid index while"
"adding a new color");
	predef.list[idx] = (_predef_color){
		.id = col,
		.r = r / 1000.f,
		.g = g / 1000.f,
		.b = b / 1000.f,
	};
}

#define _BACKGROUND_COLOR COLOR_BLACK

void _init_colors() {
	start_color();

	_add_color_at(0, COLOR_BLACK, 0, 0, 0);
	_add_color_at(1, COLOR_RED, 1000, 0, 0);
	_add_color_at(2, COLOR_GREEN, 0, 1000, 0);
	_add_color_at(3,	COLOR_YELLOW, 1000, 1000, 0);
	_add_color_at(4, COLOR_BLUE, 0, 0, 1000);
	_add_color_at(5, COLOR_MAGENTA, 1000, 0, 1000);
	_add_color_at(6, COLOR_CYAN,0, 1000, 1000);
	_add_color_at(7, COLOR_WHITE, 1000, 1000, 1000);

	init_pair(1, COLOR_BLACK, _BACKGROUND_COLOR);
	init_pair(2, COLOR_RED, _BACKGROUND_COLOR);
	init_pair(3, COLOR_GREEN, _BACKGROUND_COLOR);
	init_pair(4,	COLOR_YELLOW, _BACKGROUND_COLOR);
	init_pair(5, COLOR_BLUE, _BACKGROUND_COLOR);
	init_pair(6, COLOR_MAGENTA, _BACKGROUND_COLOR);
	init_pair(7, COLOR_CYAN, _BACKGROUND_COLOR);
	init_pair(8, COLOR_WHITE, _BACKGROUND_COLOR);
}

_color_t _color_by_rgb(float r, float g, float b) {
	float min_diff = 10.f;
	_color_t best = 0;

	for (UINT i = 0; i < _PREDEF_COLOR_NUM; i++) {
		float diff = fabs(predef.list[i].r - r) 
			+ fabs(predef.list[i].g - g)
			+ fabs(predef.list[i].b - b);

		if (diff < min_diff) {
			min_diff = diff;
			best = predef.list[i].id;
		}
	}

	return best + 1;
}