#pragma once

#include "common.h"
#include <immintrin.h>

#ifdef __SSE__
#	define _SIMD_SEE
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

#define amm_set96r_ps(e3, e2, e1) _mm_setr_ps(e3, e2, e1, 0.f)

#define amm_load96_ps(a) 			 _mm_movelh_ps(_mm_loadl_pi( 						  \
																_mm_setzero_ps(), (__m64*)(a)), \
																_mm_load_ss((float*)(a) + 2))	   

#define amm_store96_ps(a, b) 		(_mm_storel_pi((__m64*)(a), (b)),              \
											 _mm_store_ss((float*)(a) + 2, 				     \
										             	  _mm_shuffle_ps((b), (b), 2)))
