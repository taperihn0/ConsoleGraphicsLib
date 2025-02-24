#pragma once

#include "common.h"
#include "terminal.h"

void _draw_line(int x1, int y1, int x2, int y2);
void _draw_triangle_edges(int x1, int y1, int x2, int y2, int x3, int y3);
void _draw_triangle_solid(
	float x1, float y1, float z1, 
	float x2, float y2, float z2, 
	float x3, float y3, float z3,
	float b1, float b2, float b3);