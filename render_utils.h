#pragma once

#include "common.h"
#include "terminal.h"

void _draw_line(int x1, int y1, int x2, int y2);
void _draw_triangle_edges(int x1, int y1, int x2, int y2, int x3, int y3);
void _draw_triangle_solid(int x1, int y1, int x2, int y2, int x3, int y3, 
	float b1, float b2, float b3,
	float d1, float d2, float d3);