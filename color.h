#pragma once

#include "common.h"
#include "coremath.h"

typedef uint8_t _ncurses_pair_id;

#define _COL_BRIGHTNESS(col) (((col)->r + (col)->g + (col)->b) / 3.f)

void _init_colors();

_ncurses_pair_id _color_by_rgb(vec3* col);
