#include "coremath.h"

vec2_i vec2i(MATH_INT_T x, MATH_INT_T y) {
	vec2_i v = {
		.x = x,
		.y = y
	};
	return v;
}

vec2 vec2f(MATH_PREC_T x, MATH_PREC_T y) {
	vec2 v;
	v.x = x, v.y = y;
	return v;
}

vec3 vec3f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z) {
	vec3 v;
	v.x = x, v.y = y, v.z = z;
	return v;
}

vec4 vec4f(MATH_PREC_T x, MATH_PREC_T y, MATH_PREC_T z, MATH_PREC_T w) {
	vec4 v;
	v.x = x, v.y = y, v.z = z, v.w = w;
	return v;
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

void normalize2f(vec2* v) {
	MATH_PREC_T l = LENGTH2F(v);
	v->x /= l;
	v->y /= l;
}

void normalize3f(vec3* v) {
	MATH_PREC_T l = LENGTH3F(v);
	v->x /= l;
	v->y /= l;
	v->z /= l;
}

void normalize4f(vec4* v) {
	MATH_PREC_T l = LENGTH4F(v);
	v->x /= l;
	v->y /= l;
	v->z /= l;
	v->w /= l;
}

vec2 add2f(vec2* a, vec2* b) {
	vec2 r = {
		.x = a->x + b->x,
		.y = a->y + b->y
	};
	return r;
}

vec3 add3f(vec3* a, vec3* b) {
	vec3 r = {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z
	};
	return r;
}

vec4 add4f(vec4* a, vec4* b) {
	vec4 r = {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z,
		.w = a->w + b->w
	};
	return r;
}

MATH_PREC_T dot2f(vec2* a, vec2* b) {
	return a->x * b->x + a->y * b->y;
}

MATH_PREC_T dot3f(vec3* a, vec3* b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

MATH_PREC_T dot4f(vec4* a, vec4* b) {
	return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

vec3 cross3f(vec3* a, vec3* b) {
	vec3 v;
	v.x = a->y * b->z - a->z * b->y;
	v.y = b->x * a->z - b->z * a->x;
	v.z = a->x * b->y - a->y * b->x;
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

	if (!right) {
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
	return final;
}

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
	mat3 m = mat3f(NULL);

	for (UINT i = 0; i < 3; i++) {
		for (UINT j = 0; j < 3; j++) {
			for (UINT k = 0; k < 3; k++) {
				m.rc[j][i] += a->rc[j][k] * b->rc[k][i];
			}
		}
	}

	return m;
}

mat4 mult_m4(mat4* a, mat4* b) {
	mat4 m = mat4f(NULL);

	for (UINT i = 0; i < 4; i++) {
		for (UINT j = 0; j < 4; j++) {
			for (UINT k = 0; k < 4; k++) {
				m.rc[j][i] += a->rc[j][k] * b->rc[k][i];
			}
		}
	}

	return m;
}

vec2 mult_mv2(mat2* a, vec2* v) {
	vec2 r;
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y;
	return r;
}

vec3 mult_mv3(mat3* a, vec3* v) {
	vec3 r;
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y + a->rc[0][2] * v->z;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y + a->rc[1][2] * v->z;
	r.z = a->rc[2][0] * v->x + a->rc[2][1] * v->y + a->rc[2][2] * v->z;
	return r;
}

vec4 mult_mv4(mat4* a, vec4* v) {
	vec4 r;
	r.x = a->rc[0][0] * v->x + a->rc[0][1] * v->y + a->rc[0][2] * v->z + a->rc[0][3] * v->w;
	r.y = a->rc[1][0] * v->x + a->rc[1][1] * v->y + a->rc[1][2] * v->z + a->rc[1][3] * v->w;
	r.z = a->rc[2][0] * v->x + a->rc[2][1] * v->y + a->rc[2][2] * v->z + a->rc[2][3] * v->w;
	r.w = a->rc[3][0] * v->x + a->rc[3][1] * v->y + a->rc[3][2] * v->z + a->rc[3][3] * v->w;
	return r;
}