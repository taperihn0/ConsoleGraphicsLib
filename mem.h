#pragma once

#include "common.h"

typedef UINT buff_idx_t;

int gen_mem_buff(void* mem, size_t size, buff_idx_t* idx);
int delete_mem_buff(buff_idx_t idx);

int   set_mem_buff(void* mem, size_t size, buff_idx_t idx);
void* get_mem_buff(size_t* size, buff_idx_t idx);