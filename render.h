#pragma once

#include "common.h"
#include "terminal.h"
#include "mem.h"
#include "coremath.h"

void clear_terminal(CHAR_T c);
void flush_terminal();

int draw_buffer(buff_idx_t id, mat4* vt); 