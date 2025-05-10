#pragma once

#include "common.h"
#include <immintrin.h>

#ifdef __SSE__
#	define _SIMD_SSE 
#endif

// TODO: PROVIDE A SYSTEM WITH PROPER ALIGNMENT, NOW
// USING UNALIGNED LOADING BECAUSE OF COMPATIBILITY
#define _SIMD_USE_UNALIGNED_ADRESS_ONLY

#define _ALIGN(X) __attribute__((aligned((X))))
#define _MAX_ALIGN __attribute__((aligned))

#if defined(_SIMD_USE_ALIGNED_ADRESS_ONLY)
#	define amm_store_ps(a, b) _mm_store_ps((a), (b))
#	define amm_load_ps(a)     _mm_load_ps((a))
#elif defined(_SIMD_USE_UNALIGNED_ADRESS_ONLY)
#	define amm_store_ps(a, b) _mm_storeu_ps((a), (b))
#	define amm_load_ps(a)     _mm_loadu_ps((a))
#endif