#pragma once

#include "common.h"

typedef unsigned short _ncurses_pair_id;
typedef short _color_t;

void _init_colors();

_color_t _color_by_rgb(float r, float g, float b);

_STATIC _FORCE_INLINE _ncurses_pair_id color_pair_id(float r, float g, float b)  {
	return _color_by_rgb(r, g, b);
}
