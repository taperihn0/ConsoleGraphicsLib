#pragma once

#include "common.h"

#define DEFAULT_MEM_ENTRY_PREC
#define DEFAULT_MEM_ENTRY_PREC_T float

typedef struct _entry_t {
	// w component for matrix transformation, not 
	// included in memory buffer entry
	float pos_x, pos_y, pos_z, pos_w;
	float col_r, col_g, col_b;
	float norm_x, norm_y, norm_z;
} _entry_t;

#define _POS_ENTRY(entry) (vec4*)(&entry)
#define _COLOR_ENTRY(entry) (vec3*)((float*)&entry + 4)
#define _NORM_ENTRY(entry) (vec3*)((float*)&entry + 7)

_entry_t _get_entry(void* mem, size_t entry_size);

typedef UINT buff_idx_t;

int gen_mem_buff(void* mem, size_t mem_size, size_t entry_size, buff_idx_t* idx);
int delete_mem_buff(buff_idx_t idx);

int   set_mem_buff(void* mem, size_t mem_size, buff_idx_t idx);
void* get_mem_buff(size_t* mem_size, size_t* entry_size, buff_idx_t idx);