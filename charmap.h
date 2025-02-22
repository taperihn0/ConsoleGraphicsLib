#pragma once

#include "common.h"
#include "buffer.h"

int _init_char_map();
CHAR_T _char_by_brightness(float brightness);

#ifdef DEBUG
void print_char_map();
#endif