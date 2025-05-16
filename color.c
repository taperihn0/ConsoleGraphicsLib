#include "color.h"
#include "coremath.h"
#include <limits.h>

typedef short _color_ind;

#define _PREDEF_COLOR_NUM 		256

// HIGH-INTENSITY COLORS
#define _COLOR_BRIGHT_BLACK   8
#define _COLOR_BRIGHT_RED     9
#define _COLOR_BRIGHT_GREEN   10
#define _COLOR_BRIGHT_YELLOW  11
#define _COLOR_BRIGHT_BLUE    12
#define _COLOR_BRIGHT_MAGENTA 13
#define _COLOR_BRIGHT_CYAN    14
#define _COLOR_BRIGHT_WHITE   15
#define _BACKGROUND_COLOR	   COLOR_BLACK

vec3_i rgbof[_PREDEF_COLOR_NUM];

void _add_color(_color_ind code, short r, short g, short b) {
	ASSERT(code < _PREDEF_COLOR_NUM, "Invalid color");
	rgbof[code] = vec3i(r, g, b);
}

_STATIC _FORCE_INLINE _ncurses_pair_id _closest_color(short r, short g, short b) {
	int min_diff = _PREDEF_COLOR_NUM << 2;
	_color_ind code = 0;

	for (UINT i = 0; i < _PREDEF_COLOR_NUM; i++) {
		int diff = abs(rgbof[i].r - r) 
					+ abs(rgbof[i].g - g)
					+ abs(rgbof[i].b - b);

		if (diff < min_diff) {
			min_diff = diff;
			code = i;
		}
	}
	
	// pair number is shifted by one (0 is not used, we are numering from 1)
	return code + 1;
}

_ncurses_pair_id idmap[_PREDEF_COLOR_NUM][_PREDEF_COLOR_NUM][_PREDEF_COLOR_NUM];

void _init_idmap() {
	#pragma omp parallel for
	for (short r = 0; r < _PREDEF_COLOR_NUM; r++) {
		for (short g = 0; g < _PREDEF_COLOR_NUM; g++) {
			for (short b = 0; b < _PREDEF_COLOR_NUM; b++) {
				idmap[r][g][b] = _closest_color(r, g, b);
			}
		}
	}
}

void _init_colors() {
	start_color();
	
	// colors 0-7
	_add_color(COLOR_BLACK, 0, 0, 0);
	_add_color(COLOR_RED, 205, 0, 0);
	_add_color(COLOR_GREEN, 0, 205, 0);
	_add_color(COLOR_YELLOW, 205, 205, 0);
	_add_color(COLOR_BLUE, 0, 0, 238);
	_add_color(COLOR_MAGENTA, 205, 0, 205);
	_add_color(COLOR_CYAN,0, 205, 205);
	_add_color(COLOR_WHITE, 229, 229, 229); 
	
	// colors 8-15
	_add_color(_COLOR_BRIGHT_BLACK, 0, 0, 0);
	_add_color(_COLOR_BRIGHT_RED, 255, 0, 0);
	_add_color(_COLOR_BRIGHT_GREEN, 0, 255, 0);
	_add_color(_COLOR_BRIGHT_YELLOW, 255, 255, 0);
	_add_color(_COLOR_BRIGHT_BLUE, 92, 92, 255);
	_add_color(_COLOR_BRIGHT_MAGENTA, 255, 0, 255);
	_add_color(_COLOR_BRIGHT_CYAN, 0, 255, 255);
	_add_color(_COLOR_BRIGHT_WHITE, 255, 255, 255);
	
	// 16-231 as a 6x6x6 color cube
	for (UINT red = 0; red < 6; red++) {
		for (UINT green = 0; green < 6; green++) {
			for (UINT blue = 0; blue < 6; blue++) {
				_color_ind code = 16 + (red * 36) + (green * 6) + blue;
				short r = (red ? red * 40 + 55 : 0);
				short g = (green ? green * 40 + 55 : 0);
				short b = (blue ? blue * 40 + 55 : 0);
				_add_color(code, r, g, b);
			}
		}
	}
	
	// 232-255 as a grayscale ramp
	for (UINT gray = 0; gray < 24; gray++) {
		short level = 10 * gray + 8;
		_color_ind code = 232 + gray;
		_add_color(code, level, level, level);
	}

	// initialize 256 pairs of colors with a default background coloring
	for (UINT col = 0; col < _PREDEF_COLOR_NUM; col++) {
		init_pair(col + 1, col, _BACKGROUND_COLOR);
	}
	
	// finally, fill the idmap cache with proper pair numbers,
	// so matching the color is just a memory lookup
	_init_idmap();
}

_ncurses_pair_id _color_by_rgb(vec3* col) {
	vec3 ind = mult_av3(255.f, col);
	ind = round3f(&ind);
	return idmap[(size_t)ind.x][(size_t)ind.y][(size_t)ind.z];
}
