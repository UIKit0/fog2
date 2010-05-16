// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COMPILER_INTRINSSE2_H
#define _FOG_CORE_COMPILER_INTRINSSE2_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Compiler/IntrinSSE.h>

#include <emmintrin.h>

// ============================================================================
// [Fog::Core - SSE2 - Fixes]
// ============================================================================

// New VisualStudio2008 contains _mm_castsi128_ps() and _mm_castps_si128(), for
// older ones we need to implement these casts ourself.
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
// I hope that MSVC is intelligent to optimize this to NOP!
static FOG_INLINE __m128 _mm_castsi128_ps(__m128i n) { return *(__m128 *)&n; }
static FOG_INLINE __m128i _mm_castps_si128(__m128 n) { return *(__m128i*)&n; }
#endif // _MSC_VER

// Our extensions (can't be inline, because MSVC complains about constant expression).
#define _mm_shuffle_epi32_f(src, imm) _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(src), imm));

// Fix for stupid msvc debugger feature that will assert you if you want to fill
// xmm register by ones using _mm_cmpeq_epi8() on currently uninitialized variable.
//
// In release mode it's ok so we add setzero() call only in debug builds.
#if defined(_MSC_VER) && (defined(DEBUG) || defined(_DEBUG) || defined(FOG_DEBUG))
# define _mm_ext_fill_si128(__var) \
  __var = _mm_setzero_si128(); \
  __var = _mm_cmpeq_epi8(__var, __var)
#else
# define _mm_ext_fill_si128(__var) \
  __var = _mm_cmpeq_epi8(__var, __var)
#endif

// ============================================================================
// [Fog::Core - SSE2 - Classes]
// ============================================================================

namespace Fog {

//! @addtogroup Fog_Core_Intrinsics
//! @{

//! @brief SSE2 data type that can be used to store / load data to / from
//! xmm register.
union FOG_ALIGNED_TYPE(sse2_t, 16)
{
  __m128d m128d;
  __m128i m128i;
  uint64_t uq[2];
  int64_t  sq[2];
  uint32_t ud[4];
  int32_t  sd[4];
  uint16_t uw[8];
  int16_t  sw[8];
  uint8_t  ub[16];
  int8_t   sb[16];
  float    f[4];
  double   d[2];
};

//! @}

}

// ============================================================================
// [Fog::Core - SSE2 - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Intrinsics
//! @{

#define FOG_DECLARE_SSE_CONST_PI8_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14, val15) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse2_const_##name[16], 16) = \
  { \
    (uint8_t)(val15), \
    (uint8_t)(val14), \
    (uint8_t)(val13), \
    (uint8_t)(val12), \
    (uint8_t)(val11), \
    (uint8_t)(val10), \
    (uint8_t)(val9), \
    (uint8_t)(val8), \
    (uint8_t)(val7), \
    (uint8_t)(val6), \
    (uint8_t)(val5), \
    (uint8_t)(val4), \
    (uint8_t)(val3), \
    (uint8_t)(val2), \
    (uint8_t)(val1), \
    (uint8_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI8_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse2_const_##name[16], 16) = \
  { \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI16_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse2_const_##name[8], 16) = \
  { \
    (uint16_t)(val7), \
    (uint16_t)(val6), \
    (uint16_t)(val5), \
    (uint16_t)(val4), \
    (uint16_t)(val3), \
    (uint16_t)(val2), \
    (uint16_t)(val1), \
    (uint16_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI16_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse2_const_##name[8], 16) = \
  { \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI32_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse2_const_##name[4], 16) = \
  { \
    (uint32_t)(val3), \
    (uint32_t)(val2), \
    (uint32_t)(val1), \
    (uint32_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI32_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse2_const_##name[4], 16) = \
  { \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI64_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse2_const_##name[2], 16) = \
  { \
    (uint64_t)(val1), \
    (uint64_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PI64_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse2_const_##name[2], 16) = \
  { \
    (uint64_t)(val0), \
    (uint64_t)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PD_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const double, _sse2_const_##name[2], 16) = \
  { \
    val1, \
    val0  \
  }

#define FOG_DECLARE_SSE_CONST_PD_SET(name, val0) \
  FOG_ALIGNED_VAR(static const double, _sse2_const_##name[2], 16) = \
  { \
    val0, \
    val0  \
  }

#define FOG_GET_SSE_CONST_PD(name) (*(const __m128d*)_sse2_const_##name)
#define FOG_GET_SSE_CONST_PI(name) (*(const __m128i*)_sse2_const_##name)

//! @}

// [Guard]
#endif // _FOG_CORE_COMPILER_INTRINSSE2_H
