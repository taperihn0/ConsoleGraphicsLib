#include "light.h"

#define _LIGHT_LIMIT_PER_TYPE 32
#define _LIGHT_TYPES_CNT 2

typedef struct _light_map_t {
	light_directional light_dir[_LIGHT_LIMIT_PER_TYPE];
	size_t light_dir_idx;

	light_point light_pnt[_LIGHT_LIMIT_PER_TYPE];
	size_t light_pnt_idx;
} _light_map_t;

_light_map_t _light_map = {
	.light_dir_idx = 0,
};

int add_light_source(light_id_t* id, light_t type) {
	*id = type;

	switch (type) {
	case LIGHT_DIRECTIONAL:
		if (_light_map.light_dir_idx >= _LIGHT_LIMIT_PER_TYPE) 
			return 1;

		*id |= _light_map.light_dir_idx;
		_light_map.light_dir_idx++;
		break;
	case LIGHT_POINT:
		if (_light_map.light_pnt_idx >= _LIGHT_LIMIT_PER_TYPE) 
			return 1;
		
		*id |= _light_map.light_pnt_idx;
		_light_map.light_pnt_idx++;
		break;
	default: return 1;
	}

	return 0;
}

int get_light_source(light_id_t id, void** light, light_t* type) {
	light_t light_type = id & 0xF00;
	size_t idx = id & 0xFF;

	if (idx >= _LIGHT_LIMIT_PER_TYPE)
		return 1;

	switch (light_type) {
	case LIGHT_DIRECTIONAL:
		*light = (void*)&_light_map.light_dir[idx];
		if (type) *type = LIGHT_DIRECTIONAL;
		break;
	case LIGHT_POINT:
		*light = (void*)&_light_map.light_pnt[idx];
		if (type) *type = LIGHT_POINT;
		break;
	default: return 1;
	}

	return 0;
}

#define _LIGHT_REGISTER_LIMIT (_LIGHT_TYPES_CNT * _LIGHT_LIMIT_PER_TYPE)

typedef struct _light_register_t {
	light_id_t* ids;
	size_t cnt;
} _light_register_t;

_light_register_t _light_register = {
	.ids = NULL,
	.cnt = 0
};

void register_light_source(light_id_t* ids, size_t cnt) {
	ASSERT(cnt < _LIGHT_REGISTER_LIMIT, "Light register limit exceeded");
	_light_register.ids = ids;
	_light_register.cnt = cnt;
}

void register_light_clear() {
	_light_register = (_light_register_t){
		.ids = NULL,
		.cnt = 0
	};
}

void register_light_get(light_id_t** ids, size_t* cnt) {
	*ids = _light_register.ids;
	*cnt = _light_register.cnt;
}