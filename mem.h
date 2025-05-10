#pragma once

#include "common.h"
#include "coremath.h"
#include <stddef.h>

#define DEFAULT_MEM_ENTRY_PREC
#define DEFAULT_MEM_ENTRY_PREC_T float

typedef struct _entry_t {
	// w component for matrix transformation, not 
	// included in memory buffer entry
	float pos_x, pos_y, pos_z, pos_w;
	float col_r, col_g, col_b;
	float norm_x, norm_y, norm_z;
} _entry_t;

_entry_t _entry_from(float x, float y, float z, vec3* rgb, vec3* norm);
_entry_t _entry_from_vec4(vec4* pos, vec3* rgb, vec3* norm);

_UNUSED static size_t _entry_col_offset = offsetof(_entry_t, col_r);
_UNUSED static size_t _entry_norm_offset = offsetof(_entry_t, norm_x);

#define _ENTRY_POS3(entry) ((vec3*)entry)
#define _ENTRY_POS4(entry) ((vec4*)entry)
#define _ENTRY_COL(entry)  ((vec3*)((byte*)entry + _entry_col_offset))
#define _ENTRY_NORM(entry) ((vec3*)((byte*)entry + _entry_norm_offset))

_entry_t _get_entry(void* mem, size_t entry_size);

typedef UINT buff_idx_t;

int gen_mem_buff(void* mem, size_t mem_size, size_t entry_size, buff_idx_t* idx);
int delete_mem_buff(buff_idx_t idx);

int   set_mem_buff(void* mem, size_t mem_size, buff_idx_t idx);
void* get_mem_buff(size_t* mem_size, size_t* entry_size, buff_idx_t idx);