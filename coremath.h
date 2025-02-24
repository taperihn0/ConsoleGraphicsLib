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

#ifndef MATH_INT_T
#	define MATH_INT_T int
#endif

#define PI 3.14159265358979

#define RADIANS(deg) deg / 180. * PI

typedef struct vec_2fl {
	MATH_PREC_T x, y;
} vec2;

typedef struct vec_2i {
	MATH_INT_T x, y;
} vec2_i;

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

vec2_i vec2i(MATH_INT_T x, MATH_INT_T y);

vec2 vec2f(MATH_PREC_T x, MATH_PREC_T y);
vec3 vec3f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z);
vec4 vec4f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z, MATH_PREC_T w);

mat2 mat2f(MATH_PREC_T* elems);
mat3 mat3f(MATH_PREC_T* elems);
mat4 mat4f(MATH_PREC_T* elems);

#define LENGTHSQ2F(v2) dot2f(v2, v2)
#define LENGTH2F(v2) sqrt(LENGTHSQ2F(v2))
#define LENGTHSQ3F(v3) dot3f(v3, v3)
#define LENGTH3F(v3) sqrt(LENGTHSQ3F(v3))
#define LENGTHSQ4F(v4) dot4f(v4, v4)
#define LENGTH4F(v4) sqrt(LENGTHSQ4F(v4))

void normalize2f(vec2* v);
void normalize3f(vec3* v);
void normalize4f(vec4* v);

vec2 add2f(vec2* a, vec2* b);
vec3 add3f(vec3* a, vec3* b);
vec4 add4f(vec4* a, vec4* b);

MATH_PREC_T dot2f(vec2* a, vec2* b);
MATH_PREC_T dot3f(vec3* a, vec3* b);
MATH_PREC_T dot4f(vec4* a, vec4* b);

// frequent use of this expansion make it necessary 
// to make it a macro, which is *ALWAYS* inline
#define CROSSPROD_2D(v1, v2) ((v1).x * (v2).y - (v1).y * (v2).x)

vec3 cross3f(vec3* a, vec3* b);

mat2 diagmat2f(MATH_PREC_T a);
mat3 diagmat3f(MATH_PREC_T a);
mat4 diagmat4f(MATH_PREC_T a);

mat4 projmat4f(float fov, float aspect_ratio, float n, float f);
mat4 viewmat4f(vec3* pos, vec3* dir, vec3* up, vec3* right);

mat2 mult_m2(mat2* a, mat2* b);
mat3 mult_m3(mat3* a, mat3* b);
mat4 mult_m4(mat4* a, mat4* b);

vec2 mult_mv2(mat2* a, vec2* v);
vec3 mult_mv3(mat3* a, vec3* v);
vec4 mult_mv4(mat4* a, vec4* v);