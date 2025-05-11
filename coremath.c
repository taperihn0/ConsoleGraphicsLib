#include "coremath.h"

vec2_i vec2i(MATH_INT_T x, MATH_INT_T y) {
	return (vec2_i) {
		.x = x,
		.y = y
	};
}

vec2 vec2f(MATH_PREC_T x, MATH_PREC_T y) {
	return (vec2) {
		.x = x,
		.y = y
	};
}

vec3 vec3f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z) {
	return (vec3) {
		.x = x,
		.y = y,
		.z = z
	};
}

vec4 vec4f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z, MATH_PREC_T w) {
	return (vec4) {
		.x = x,
		.y = y,
		.z = z,
		.w = w
	};
}

mat2 mat2f(MATH_PREC_T* elems) {
	mat2 m;

	if (elems == NULL)
		memset(m.rc, 0, sizeof(m.rc));
	else
		memcpy(m.rc, elems, 2 * 2 * sizeof(MATH_PREC_T));

	return m;
}

mat3 mat3f(MATH_PREC_T* elems) {
	mat3 m;

	if (elems == NULL) 
		memset(m.rc, 0, sizeof(m.rc));
	else
		memcpy(m.rc, elems, 3 * 3 * sizeof(MATH_PREC_T));

	return m;
}

mat4 mat4f(MATH_PREC_T* elems) {
	mat4 m;
	
	if (elems == NULL)
		memset(m.rc, 0, sizeof(m.rc));
	else
		memcpy(m.rc, elems, 4 * 4 * sizeof(MATH_PREC_T));

	return m;
}

int equal2f(vec2* a, vec2* b) { 
	return a->x == b->x && 
			 a->y == b->y;
}

int equal3f(vec3* a, vec3* b) {
	return a->x == b->x && 
			 a->y == b->y &&
			 a->z == b->z;
}

int equal4f(vec4* a, vec4* b) {
	return a->x == b->x && 
			 a->y == b->y &&
			 a->z == b->z &&
			 a->w == b->w;
}

void normalize2f(vec2* v) {
	MATH_PREC_T l = LENGTH2F(v);
	v->x /= l;
	v->y /= l;
}

void normalize3f(vec3* v) {
	MATH_PREC_T l = LENGTH3F(v);
#ifndef _SIMD_SEE
	v->x /= l;
	v->y /= l;
	v->z /= l;
#else
	amm_store96_ps((float*)v, _mm_div_ps(amm_load96_ps((float*)v), 
	                                     _mm_load_ps1(&l)));
#endif
}

void normalize4f(vec4* v) {
	MATH_PREC_T l = LENGTH4F(v);
#ifndef _SIMD_SEE
	v->x /= l;
	v->y /= l;
	v->z /= l;
	v->w /= l;
#else
	amm_store_ps((float*)v, _mm_div_ps(amm_load_ps((float*)v), 
	                                   _mm_load_ps1(&l)));
#endif
}

vec2 add2f(vec2* a, vec2* b) {
	return (vec2) {
		.x = a->x + b->x,
		.y = a->y + b->y
	};
}

vec3 add3f(vec3* a, vec3* b) {
	_ALIGN(16) vec3 r;
#ifndef _SIMD_SEE
	r = (vec3) {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z
	};
#else
	amm_store96_ps((float*)(&r), _mm_add_ps(amm_load96_ps((float*)a),
	                                        amm_load96_ps((float*)b)));
#endif
	return r;
}

vec4 add4f(vec4* a, vec4* b) {
	vec4 r;
#ifndef _SIMD_SEE
	r = (vec4) {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z,
		.w = a->w + b->w
	};
#else
	_mm_store_ps((float*)(&r), _mm_add_ps(amm_load_ps((float*)a), 
	                                      amm_load_ps((float*)b)));
#endif
	return r;
}

vec2 sub2f(vec2* a, vec2* b) {
	return (vec2) {
		.x = a->x - b->x,
		.y = a->y - b->y,
	};
}

vec3 sub3f(vec3* a, vec3* b) {
	_ALIGN(16) vec3 r;
#ifndef _SIMD_SEE
	r = (vec3) {
		.x = a->x - b->x,
		.y = a->y - b->y,
		.z = a->z - b->z,
	};
#else
	amm_store96_ps((float*)(&r), _mm_sub_ps(amm_load96_ps((float*)a),
	                                        amm_load96_ps((float*)b)));
#endif
	return r;
}

vec4 sub4f(vec4* a, vec4* b) {
	vec4 r;
#ifndef _SIMD_SEE
	r = (vec4) {
		.x = a->x - b->x,
		.y = a->y - b->y,
		.z = a->z - b->z,
		.w = a->w - b->w,
	};
#else
	_mm_store_ps((float*)(&r), _mm_sub_ps(amm_load_ps((float*)a),
	                                      amm_load_ps((float*)b)));
#endif
	return r;
}

vec2 mult_av2(MATH_PREC_T alpha, vec2* a) {
	return (vec2) {
		.x = a->x * alpha,
		.y = a->y * alpha,
	};
}

vec3 mult_av3(MATH_PREC_T alpha, vec3* a) {
	_ALIGN(16) vec3 r;
#ifndef _SIMD_SEE
	r = (vec3) {
		.x = a->x * alpha,
		.y = a->y * alpha,
		.z = a->z * alpha,
	};
#else
	amm_store96_ps((float*)(&r), _mm_mul_ps(amm_load96_ps((float*)a),
	                                        _mm_load_ps1(&alpha)));
#endif
	return r;
}

vec4 mult_av4(MATH_PREC_T alpha, vec4* a) {
	vec4 r;
#ifndef _SIMD_SEE
	r = (vec4) {
		.x = a->x * alpha,
		.y = a->y * alpha,
		.z = a->z * alpha,
		.w = a->w * alpha,
	};
#else
	_mm_store_ps((float*)(&r), _mm_mul_ps(amm_load_ps((float*)a),
	                                      _mm_load_ps1(&alpha)));
#endif
	return r;
}

vec2 mult_v2(vec2* a, vec2* b) {
	return vec2f(a->x * b->x, 
	             a->y * b->y);
}

vec3 mult_v3(vec3* a, vec3* b) {
	_ALIGN(16) vec3 r;
#ifndef _SIMD_SEE
	r = vec3f(a->x * b->x, 
				 a->y * b->y, 
				 a->z * b->z);
#else
	amm_store96_ps((float*)(&r), _mm_mul_ps(amm_load96_ps((float*)a),
	                                        amm_load96_ps((float*)b)));
#endif
	return r;
}

vec4 mult_v4(vec4* a, vec4* b) {
	vec4 r;
#ifndef _SIMD_SEE
	r = vec4f(a->x * b->x, 
	          a->y * b->y, 
	          a->z * b->z, 
	          a->w * b->w);
#else
	_mm_store_ps((float*)(&r), _mm_mul_ps(amm_load_ps((float*)a),
	                                      amm_load_ps((float*)b)));
#endif
	return r;
}

MATH_PREC_T dot2f(vec2* a, vec2* b) {
	return a->x * b->x + 
			 a->y * b->y;
}

MATH_PREC_T dot3f(vec3* a, vec3* b) {
	MATH_PREC_T r;
#ifndef _SIMD_SEE
	r = a->x * b->x + 
		 a->y * b->y + 
		 a->z * b->z;
#else
	r = _mm_cvtss_f32(_mm_dp_ps(amm_load96_ps((float*)a), 
	                            amm_load96_ps((float*)b), 
	                            0x77));
#endif
	return r;
}

MATH_PREC_T dot4f(vec4* a, vec4* b) {
	MATH_PREC_T r;
#ifndef _SIMD_SEE
	r = a->x * b->x + 
		 a->y * b->y + 
		 a->z * b->z + 
		 a->w * b->w;
#else
	r = _mm_cvtss_f32(_mm_dp_ps(amm_load_ps((float*)a), 
	                            amm_load_ps((float*)b), 
	                            0xFF));
#endif
	return r;
}

vec3 cross3f(vec3* a, vec3* b) {
	_ALIGN(16) vec3 v;
#ifndef _SIMD_SEE
	v.x = a->y * b->z - a->z * b->y;
	v.y = b->x * a->z - b->z * a->x;
	v.z = a->x * b->y - a->y * b->x;
#else
	__m128 ps3a = amm_load96_ps(a);
	__m128 ps3b = amm_load96_ps(b);
	__m128 sa   = _mm_shuffle_ps(ps3a, ps3a, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 sb   = _mm_shuffle_ps(ps3b, ps3b, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 cl1  = _mm_mul_ps(sa, ps3b);
	__m128 cl0  = _mm_mul_ps(sa, sb);
	__m128 sl1  = _mm_shuffle_ps(cl1, cl1, _MM_SHUFFLE(3, 0, 2, 1));
	amm_store96_ps((float*)(&v), _mm_sub_ps(cl0, sl1));
#endif
	return v;
}

mat2 diagmat2f(MATH_PREC_T a) {
	mat2 m = mat2f(NULL);
	m.rc[0][0] = a;
	m.rc[1][1] = a;
	return m;
}

mat3 diagmat3f(MATH_PREC_T a) {
	mat3 m = mat3f(NULL);
	m.rc[0][0] = a;
	m.rc[1][1] = a;
	m.rc[2][2] = a;
	return m;
}

mat4 diagmat4f(MATH_PREC_T a) {
	mat4 m = mat4f(NULL);
	m.rc[0][0] = a;
	m.rc[1][1] = a;
	m.rc[2][2] = a;
	m.rc[3][3] = a;
	return m;
}

mat4 projmat4f(float fov, float aspect_ratio, float n, float f) {
	mat4 proj = mat4f(NULL);
	
	float t = tan(RADIANS(fov / 2.f)) * n;
	float r = t * aspect_ratio;

	proj.rc[0][0] = n / r;
	proj.rc[1][1] = n / t;
	proj.rc[2][2] = -(f + n) / (n - f);
	proj.rc[2][3] = 2 * f * n / (n - f);
	proj.rc[3][2] = 1;

	return proj;
}

mat4 viewmat4f(vec3* pos, vec3* dir, vec3* up, vec3* right) {
	mat4 view = mat4f(NULL);
	
	int nright = 0;
	if (!right) {
		nright = 1;
		right = malloc(sizeof(vec3));
		*right = cross3f(dir, up);
		normalize3f(right);
	}

	view.rc[0][0] = right->x;
	view.rc[0][1] = right->y;
	view.rc[0][2] = right->z;
	view.rc[1][0] = up->x;
	view.rc[1][1] = up->y;
	view.rc[1][2] = up->z;
	view.rc[2][0] = dir->x;
	view.rc[2][1] = dir->y;
	view.rc[2][2] = dir->z;
	view.rc[3][3] = 1;

	mat4 translation = diagmat4f(1);

	translation.rc[0][3] = -pos->x;
	translation.rc[1][3] = -pos->y;
	translation.rc[2][3] = -pos->z;

	mat4 final = mult_m4(&view, &translation);
	if (nright) free(right);
	return final;
}

mat3 m3_from_m4(mat4* a) {
	mat3 r;

	for (UINT i = 0; i < 3; i++) {
		for (UINT j = 0; j < 3; j++) {
			r.rc[i][j] = a->rc[i][j];
		}
	}

	return r;
}

mat3 transpose_m3(mat3* a) {
	mat3 r;

	for (UINT i = 0; i < 3; i++) {
		for (UINT j = 0; j < 3; j++) {
			r.rc[i][j] = a->rc[j][i];
		}
	}

	return r;
}

/*
mat4 inverse_m4(mat4* a) {

}
*/

mat2 mult_m2(mat2* a, mat2* b) {
	mat2 m = mat2f(NULL);

	for (UINT i = 0; i < 2; i++) {
		for (UINT j = 0; j < 2; j++) {
			for (UINT k = 0; k < 2; k++) {
				m.rc[j][i] += a->rc[j][k] * b->rc[k][i];
			}
		}
	}

	return m;
}

mat3 mult_m3(mat3* a, mat3* b) {
	mat3 m;
#ifndef _SIMD_SEE
	for (UINT i = 0; i < 3; i++) {
		for (UINT j = 0; j < 3; j++) {
			m.rc[j][i] = 0;
			for (UINT k = 0; k < 3; k++) {
				m.rc[j][i] += a->rc[j][k] * b->rc[k][i];
			}
		}
	}
#else
	__m128 ps3ra[3];
	ps3ra[0] = amm_load96_ps((float*)(&a->r0));
	ps3ra[1] = amm_load96_ps((float*)(&a->r1));
	ps3ra[2] = amm_load96_ps((float*)(&a->r2));
	
	__m128 ps3cb[3];
	ps3cb[0] = amm_set96r_ps(b->rc[0][0], b->rc[1][0], b->rc[2][0]);
	ps3cb[1] = amm_set96r_ps(b->rc[0][1], b->rc[1][1], b->rc[2][1]);
	ps3cb[2] = amm_set96r_ps(b->rc[0][2], b->rc[1][2], b->rc[2][2]);

	for (UINT j = 0; j < 3; j++) {
		for (UINT i = 0; i < 3; i++) {
			m.rc[j][i] = _mm_cvtss_f32(_mm_dp_ps(ps3ra[j], 
			                                     ps3cb[i], 
			                                     0x77));
		}
	}
#endif
	return m;
}

mat4 mult_m4(mat4* a, mat4* b) {
	mat4 m;
#ifndef _SIMD_SEE
	for (UINT j = 0; j < 4; j++) {
		for (UINT i = 0; i < 4; i++) {
			m.rc[j][i] = 0;
			for (UINT k = 0; k < 4; k++) {
				m.rc[j][i] += a->rc[j][k] * b->rc[k][i];
			}
		}
	}
#else
	__m128 ps3ra[4];
	ps3ra[0] = amm_load_ps((float*)(&a->r0));
	ps3ra[1] = amm_load_ps((float*)(&a->r1));
	ps3ra[2] = amm_load_ps((float*)(&a->r2));
	ps3ra[3] = amm_load_ps((float*)(&a->r3));
	
	__m128 ps3cb[4];
	ps3cb[0] = _mm_setr_ps(b->rc[0][0], b->rc[1][0], b->rc[2][0], b->rc[3][0]);
	ps3cb[1] = _mm_setr_ps(b->rc[0][1], b->rc[1][1], b->rc[2][1], b->rc[3][1]);
	ps3cb[2] = _mm_setr_ps(b->rc[0][2], b->rc[1][2], b->rc[2][2], b->rc[3][2]);
	ps3cb[3] = _mm_setr_ps(b->rc[0][3], b->rc[1][3], b->rc[2][3], b->rc[3][3]);

	for (UINT j = 0; j < 4; j++) {
		for (UINT i = 0; i < 4; i++) {
			m.rc[j][i] = _mm_cvtss_f32(_mm_dp_ps(ps3ra[j], 
			                                     ps3cb[i], 
															 0xFF));
		}
	}
#endif
	return m;
}

vec2 mult_mv2(mat2* a, vec2* v) {
	vec2 r;
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y;
	return r;
}

vec3 mult_mv3(mat3* a, vec3* v) {
	_ALIGN(16) vec3 r;
#ifndef _SIMD_SSE
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y + a->rc[0][2] * v->z;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y + a->rc[1][2] * v->z;
	r.z = a->rc[2][0] * v->x + a->rc[2][1] * v->y + a->rc[2][2] * v->z;
#else
	r.x = dot3f(&a->r0, v);
	r.y = dot3f(&a->r1, v);
	r.z = dot3f(&a->r2, v);
#endif
	return r;
}

vec4 mult_mv4(mat4* a, vec4* v) {
	vec4 r;
#ifndef _SIMD_SSE
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y + a->rc[0][2] * v->z + a->rc[0][3] * v->w;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y + a->rc[1][2] * v->z + a->rc[1][3] * v->w;
	r.z = a->rc[2][0] * v->x + a->rc[2][1] * v->y + a->rc[2][2] * v->z + a->rc[2][3] * v->w;
	r.w = a->rc[3][0] * v->x + a->rc[3][1] * v->y + a->rc[3][2] * v->z + a->rc[3][3] * v->w;
#else
	r.x = dot4f(&a->r0, v);
	r.y = dot4f(&a->r1, v);
	r.z = dot4f(&a->r2, v);
	r.w = dot4f(&a->r3, v);
#endif
	return r;
}