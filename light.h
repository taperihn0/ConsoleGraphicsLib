#pragma once

#include "common.h"
#include "coremath.h"

#define LIGHT_DIRECTIONAL 0x100
#define LIGHT_POINT 0x200

typedef unsigned short light_t;

typedef struct light_directional {
	vec3 dir;
	// ...
	//vec3 ambient;
	//vec3 diffuse;
} light_directional;

typedef struct light_point {
	vec3 pos;
	//...
} light_point;

typedef unsigned short light_id_t;

int add_light_source(light_id_t* id, light_t type);
int get_light_source(light_id_t id, void** light, light_t* type);
void register_light_source(light_id_t* ids, size_t cnt);
void register_light_clear();
void register_light_get(light_id_t** ids, size_t* cnt);