#pragma once

#include "common.h"
#include "terminal.h"
#include "render.h"

void _draw_line(int x1, int y1, int x2, int y2);
void _draw_triangle_edges(int x1, int y1, int x2, int y2, int x3, int y3);

void _draw_triangle_solid(
	vec4* v1, vec4* v2, vec4* v3,
	vec3* col1, vec3* col2, vec3* col3,
	vec3* norm1, vec3* norm2, vec3* norm3,
	func_stage_fragment stage_fragment, 
	void* attrib);