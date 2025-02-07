#pragma once

#include "common.h"

int gen_mem_buff(void* mem, size_t size, UINT* idx);
int delete_mem_buff(UINT idx);

int   set_mem_buff(void* mem, size_t size, UINT idx);
void* get_mem_buff(size_t* size, UINT idx);