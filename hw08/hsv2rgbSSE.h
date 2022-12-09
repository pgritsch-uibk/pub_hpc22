#ifndef HPC22_HSV2RGBSSE_H
#define HPC22_HSV2RGBSSE_H

#include <array>
#include <cstdint>

#if defined(__aarch64__)
#include "sse2neon/sse2neon.h"
#else
#include <emmintrin.h>
#include <smmintrin.h>
//#include "sse2neon/sse2neon.h"
#endif

std::array<__m128, 4> hsvToRgb(__m128 h) {
	// numbers needed for "switch" statement
	const __m128 _0 = _mm_set_ps1(0.f);
	const __m128 _1 = _mm_set_ps1(1.f);
	const __m128 _2 = _mm_set_ps1(2.f);
	const __m128 _3 = _mm_set_ps1(3.f);
	const __m128 _4 = _mm_set_ps1(4.f);
	const __m128 _5 = _mm_set_ps1(5.f);

	const __m128 _UINT8_MAX = _mm_set_ps1(UINT8_MAX);

	// from the algorithm
	const __m128 v = _1;
	const __m128 vh = _mm_mul_ps(h, _mm_set_ps1(6.f));
	const __m128 i = _mm_floor_ps(vh);
	const __m128 f = _mm_sub_ps(vh, i);
	const __m128 p = _mm_set_ps1(0.f);
	const __m128 q = _mm_sub_ps(_1, f);
	const __m128 t = f;

	// if h >= 1.0 -> we return black -> just and this mask with the output
	const __m128 hGe1Mask = _mm_cmplt_ps(h, _1);

	// create masks for switch
	const __m128 mask0 = _mm_cmpeq_ps(i, _0);
	const __m128 mask1 = _mm_cmpeq_ps(i, _1);
	const __m128 mask2 = _mm_cmpeq_ps(i, _2);
	const __m128 mask3 = _mm_cmpeq_ps(i, _3);
	const __m128 mask4 = _mm_cmpeq_ps(i, _4);
	const __m128 mask5 = _mm_cmpeq_ps(i, _5);

	const __m128 redMask0 = _mm_and_ps(mask0, v);
	const __m128 redMask1 = _mm_and_ps(mask1, q);
	const __m128 redMask2 = _mm_and_ps(mask2, p);
	const __m128 redMask3 = _mm_and_ps(mask3, p);
	const __m128 redMask4 = _mm_and_ps(mask4, t);
	const __m128 redMask5 = _mm_and_ps(mask5, v);

	const __m128 greenMask0 = _mm_and_ps(mask0, t);
	const __m128 greenMask1 = _mm_and_ps(mask1, v);
	const __m128 greenMask2 = _mm_and_ps(mask2, v);
	const __m128 greenMask3 = _mm_and_ps(mask3, q);
	const __m128 greenMask4 = _mm_and_ps(mask4, p);
	const __m128 greenMask5 = _mm_and_ps(mask5, p);

	const __m128 blueMask0 = _mm_and_ps(mask0, p);
	const __m128 blueMask1 = _mm_and_ps(mask1, p);
	const __m128 blueMask2 = _mm_and_ps(mask2, t);
	const __m128 blueMask3 = _mm_and_ps(mask3, v);
	const __m128 blueMask4 = _mm_and_ps(mask4, v);
	const __m128 blueMask5 = _mm_and_ps(mask5, q);

	__m128 redComponent =
	    _mm_or_ps(_mm_or_ps(_mm_or_ps(redMask0, redMask1), _mm_or_ps(redMask2, redMask3)),
	              _mm_or_ps(redMask4, redMask5));
	redComponent = _mm_and_ps(hGe1Mask, redComponent);
	redComponent = _mm_mul_ps(_UINT8_MAX, redComponent);

	__m128 greenComponent =
	    _mm_or_ps(_mm_or_ps(_mm_or_ps(greenMask0, greenMask1), _mm_or_ps(greenMask2, greenMask3)),
	              _mm_or_ps(greenMask4, greenMask5));
	greenComponent = _mm_and_ps(hGe1Mask, greenComponent);
	greenComponent = _mm_mul_ps(_UINT8_MAX, greenComponent);

	__m128 blueComponent =
	    _mm_or_ps(_mm_or_ps(_mm_or_ps(blueMask0, blueMask1), _mm_or_ps(blueMask2, blueMask3)),
	              _mm_or_ps(blueMask4, blueMask5));
	blueComponent = _mm_and_ps(hGe1Mask, blueComponent);
	blueComponent = _mm_mul_ps(_UINT8_MAX, blueComponent);

	__m128 fourthComponent = _mm_set_ps1(0.f);

	_MM_TRANSPOSE4_PS(redComponent, greenComponent, blueComponent, fourthComponent);

	return { redComponent, greenComponent, blueComponent, fourthComponent };
}

#endif // HPC22_HSV2RGBSSE_H
