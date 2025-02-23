#pragma once

#include "common.h"

#define DEFAULT_MEM_ENTRY_PREC
#define DEFAULT_MEM_ENTRY_PREC_T float

typedef struct _entry_t {
	float pos_x, pos_y, pos_z, pos_w;
	float brightness;
	float norm_x, norm_y, norm_z;
} _entry_t;

#define _POS_ENTRY(entry) (vec4*)(&entry)
#define _BRIGHTNESS_ENTRY(entry) (float*)((float*)&entry + 4)
#define _NORM_ENTRY(entry) (vec3*)((float*)&entry + 5)

// TODO: fix this
_FORCE_INLINE _entry_t _get_entry(void* mem, size_t entry_size) {
	_entry_t r;
	memset(&r, 0, sizeof(r));
	
	_entry_t* entry = (_entry_t*)mem;

	r.pos_x = entry->pos_x;
	r.pos_y = entry->pos_y;
	r.pos_z = entry->pos_z;
	r.pos_w = 1.f;

	return r;
}

typedef UINT buff_idx_t;

int gen_mem_buff(void* mem, size_t mem_size, size_t entry_size, buff_idx_t* idx);
int delete_mem_buff(buff_idx_t idx);

int   set_mem_buff(void* mem, size_t mem_size, buff_idx_t idx);
void* get_mem_buff(size_t* mem_size, size_t* entry_size, buff_idx_t idx);