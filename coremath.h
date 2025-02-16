#pragma once

#include "common.h"
#include <math.h>

/*
	MAIN MINIMALISTIC MATH HEADER FOR GRAPHICS PURPOSES (MATH CORE)
*/

typedef float   	_prec0_t;
typedef double  	_prec1_t;
typedef long double _prec2_t;

#ifndef MATH_PREC_T
#	define MATH_PREC_T _prec0_t
#endif

#define PI 3.14159265358979

#define RADIANS(deg) deg / 180. * PI

typedef struct vec_2fl {
	MATH_PREC_T x, y;
} vec2;

typedef struct vec_3fl {
	MATH_PREC_T x, y, z;
} vec3;

typedef struct vec_4fl {
	MATH_PREC_T x, y, z, w;
} vec4;

typedef struct mat_2fl {
	MATH_PREC_T rc[2][2];
} mat2;

typedef struct mat_3fl {
	MATH_PREC_T rc[3][3];
} mat3;

typedef struct mat_4fl {
	MATH_PREC_T rc[4][4];
} mat4;

vec2 vec2f(MATH_PREC_T x, MATH_PREC_T y);
vec3 vec3f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z);
vec4 vec4f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z, MATH_PREC_T w);

mat2 mat2f(MATH_PREC_T* elems);
mat3 mat3f(MATH_PREC_T* elems);
mat4 mat4f(MATH_PREC_T* elems);

#define LENGTH2F(v2) sqrt(dot2f(v2, v2))
#define LENGTHSQ2F(v2) dot2f(v2, v2)
#define LENGTH3F(v3) sqrt(dot3f(v3, v3))
#define LENGTHSQ3F(v3) dot3f(v3, v3)
#define LENGTH4F(v4) sqrt(dot4f(v4, v4))
#define LENGTHSQ4F(v4) dot4f(v4, v4)

void normalize2f(vec2* v);
void normalize3f(vec3* v);
void normalize4f(vec4* v);

MATH_PREC_T dot2f(vec2* a, vec2* b);
MATH_PREC_T dot3f(vec3* a, vec3* b);
MATH_PREC_T dot4f(vec4* a, vec4* b);

vec3 cross3f(vec3* a, vec3* b);

mat2 diagmat2f(MATH_PREC_T a);
mat3 diagmat3f(MATH_PREC_T a);
mat4 diagmat4f(MATH_PREC_T a);

mat2 mult_m2(mat2* a, mat2* b);
mat3 mult_m3(mat3* a, mat3* b);
mat4 mult_m4(mat4* a, mat4* b);

vec2 mult_mv2(mat2* a, vec2* v);
vec3 mult_mv3(mat3* a, vec3* v);
vec4 mult_mv4(mat4* a, vec4* v);