#pragma once

#include "simd.h"
#include <math.h>

/*
	MAIN MINIMALISTIC MATH HEADER FOR GRAPHICS PURPOSES (MATH CORE)
*/

#ifndef MATH_EXTENSIONS
#	ifdef _SIMD_SEE
#		define MATH_EXTENSIONS
#	endif
#endif

typedef float _prec0_t;
typedef double _prec1_t;

#define MATH_PREC_T _prec0_t
#define MATH_INT_T  int32_t

#define PI 	3.14159265358979
#define PIf ((float)(PI))

#define RADIANS(deg)  deg / 180. * PI
#define RADIANSf(def) deg / 180.f * PIf

typedef _ALIGN(8) union _vec_2fl_t {
	struct {
		MATH_PREC_T x, 
						y;
	};
	MATH_PREC_T col[2];
} vec2;

typedef _ALIGN(8) union _vec_2i_t {
	struct {
		MATH_INT_T x, 
					  y;
	};
	MATH_INT_T col[2];
} vec2_i;

typedef union _vec_3fl_t {
	struct {
		MATH_PREC_T x, 
						y, 
						z;
	};
	struct {
		MATH_PREC_T r,
						g,
						b;
	};
	MATH_PREC_T col[3];
} vec3;

typedef union _vec_3i_t {
	struct {
		MATH_INT_T x, 
					  y, 
					  z;
	};
	struct {
		MATH_INT_T r,
					  g,
					  b;
	};
	MATH_INT_T col[3];
} vec3_i;

typedef _ALIGN(16) union _vec_4fl_t {
	struct {
		MATH_PREC_T x, 
						y, 
						z, 
						w;
	};
	struct {
		MATH_PREC_T r,
						g,
						b,
						a;
	};
	MATH_PREC_T col[4];
} vec4;

typedef _ALIGN(16) union _vec_4i_t {
	struct {
		MATH_INT_T x, 
					  y, 
					  z, 
					  w;
	};
	struct {
		MATH_INT_T r,
					  g,
					  b,
					  a;
	};
	MATH_INT_T col[4];
} vec4_i;

typedef _ALIGN(8) struct mat_2fl {
	struct {
		vec2 r0,
			  r1;
	};
	MATH_PREC_T rc[2][2];
} mat2;

typedef union mat_3fl {
	struct {
		vec3 r0,
			  r1,
			  r2;
	};
	MATH_PREC_T rc[3][3];
} mat3;

typedef _ALIGN(16) union mat_4fl {
	struct {
		vec4 r0,
			  r1,
			  r2,
			  r3;
	};
	MATH_PREC_T rc[4][4];
} mat4;

vec2_i vec2i(MATH_INT_T x, MATH_INT_T y);
vec2   vec2f(MATH_PREC_T x, MATH_PREC_T y);
vec3   vec3f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z);
vec4   vec4f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z, MATH_PREC_T w);

mat2 mat2f(MATH_PREC_T* elems);
mat3 mat3f(MATH_PREC_T* elems);
mat4 mat4f(MATH_PREC_T* elems);

#define LENGTHSQ2F(v2) dot2f(v2, v2)
#define LENGTH2F(v2)   sqrt(LENGTHSQ2F(v2))
#define LENGTHSQ3F(v3) dot3f(v3, v3)
#define LENGTH3F(v3)   sqrt(LENGTHSQ3F(v3))
#define LENGTHSQ4F(v4) dot4f(v4, v4)
#define LENGTH4F(v4)   sqrt(LENGTHSQ4F(v4))

int equal2f(vec2* a, vec2* b);
int equal3f(vec3* a, vec3* b);
int equal4f(vec4* a, vec4* b);

void normalize2f(vec2* v);
void normalize3f(vec3* v);
void normalize4f(vec4* v);

vec2 add2f(vec2* a, vec2* b);
vec3 add3f(vec3* a, vec3* b);
vec4 add4f(vec4* a, vec4* b);

vec2 sub2f(vec2* a, vec2* b);
vec3 sub3f(vec3* a, vec3* b);
vec4 sub4f(vec4* a, vec4* b);

vec2 mult_av2(MATH_PREC_T alpha, vec2* a);
vec3 mult_av3(MATH_PREC_T alpha, vec3* a);
vec4 mult_av4(MATH_PREC_T alpha, vec4* a);

vec2 div_av2(MATH_PREC_T alpha, vec2* a);
vec3 div_av3(MATH_PREC_T alpha, vec3* a);
vec4 div_av4(MATH_PREC_T alpha, vec4* a);

vec2 mult_v2(vec2* a, vec2* b);
vec3 mult_v3(vec3* a, vec3* b);
vec4 mult_v4(vec4* a, vec4* b);

vec2 div_v2(vec2* a, vec2* b);
vec3 div_v3(vec3* a, vec3* b);
vec4 div_v4(vec4* a, vec4* b);

MATH_PREC_T dot2f(vec2* a, vec2* b);
MATH_PREC_T dot3f(vec3* a, vec3* b);
MATH_PREC_T dot4f(vec4* a, vec4* b);

// frequent use of this expansion make it necessary 
// to make it a macro, which is *ALWAYS* inlined
#define CROSS2F(v1, v2) \
	((v1)->x * (v2)->y - (v1)->y * (v2)->x)

vec3 cross3f(vec3* a, vec3* b);

#ifdef MATH_EXTENSIONS
// returns vector of fields [x, y, z], where:
//  x - CROSS2F(a, b),
//  y - CROSS2F(c, d)
//  z - CROSS2F(e, f)
vec3 mext_cross2fx3(vec2* a, vec2* b, 
                    vec2* c, vec2* d,
                    vec2* e, vec2* f);
#endif

mat2 diagmat2f(MATH_PREC_T a);
mat3 diagmat3f(MATH_PREC_T a);
mat4 diagmat4f(MATH_PREC_T a);

mat4 projmat4f(float fov, float aspect_ratio, float n, float f);
mat4 viewmat4f(vec3* pos, vec3* dir, vec3* up, vec3* right);

mat3 m3_from_m4(mat4* a);

mat3 transpose_m3(mat3* a);

//mat4 inverse_m4(mat4* a);

mat2 mult_m2(mat2* a, mat2* b);
mat3 mult_m3(mat3* a, mat3* b);
mat4 mult_m4(mat4* a, mat4* b);

vec2 mult_mv2(mat2* a, vec2* v);
vec3 mult_mv3(mat3* a, vec3* v);
vec4 mult_mv4(mat4* a, vec4* v);