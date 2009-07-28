// [Fog/Graphics Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>
#if defined(FOG_IDE)
#include <Fog/Graphics/Raster/Raster_SSE2_base.cpp.h>
#include <Fog/Graphics/Raster/Raster_SSE2_convert.cpp.h>
#endif // FOG_IDE

namespace Fog {
namespace Raster {

// ============================================================================
// [Fog::Raster - Raster - Argb32 - Src]
// ============================================================================

static void FOG_FASTCALL raster_argb32_pixel_src_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgba;
  ((uint32_t*)dst)[0] = src0;
}

static void FOG_FASTCALL raster_argb32_pixel_a8_src_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  if (msk != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, &src->rgba);

    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_mask_1x1W(a0mm, msk);
    pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    __m128i src0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_store4(dst, src0mm);
  }
}

static void FOG_FASTCALL raster_argb32_span_solid_src_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgba);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    pix_store4(dst, src0mm);
    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  while (w >= 4)
  {
    pix_store16a(dst, src0mm);
    pix_store16a(dst + 16, src0mm);
    pix_store16a(dst + 32, src0mm);
    pix_store16a(dst + 48, src0mm);

    dst += 64;
    w -= 4;
  }

  switch (w & 3)
  {
    case 3: pix_store16a(dst, src0mm); dst += 16;
    case 2: pix_store16a(dst, src0mm); dst += 16;
    case 1: pix_store16a(dst, src0mm); dst += 16;
  }

  if ((_i = _j)) { w = 0; goto blt; }
}

static void FOG_FASTCALL raster_argb32_span_solid_a8_src_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgba);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);
    if (msk0 == 0xFF)
    {
      pix_store4(dst, src0orig);
    }
    else if (msk0)
    {
      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);
    if (msk0 != 0x00000000)
    {
      if (msk0 == 0xFFFFFFFF)
      {
        pix_store16a(dst, src0orig);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_argb32_span_composite_argb32_src_sse2 convert_memcpy32_sse2
#define raster_argb32_span_composite_prgb32_src_sse2 convert_argb32_from_prgb32_sse2
#define raster_argb32_span_composite_rgb32_src_sse2 convert_argb32_from_rgb32_sse2
#define raster_argb32_span_composite_rgb24_src_sse2 convert_rgb32_from_rgb24_sse2

static void FOG_FASTCALL raster_argb32_span_composite_argb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(src0mm, src0mm);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_pack_1x1W(src0mm, src0mm);
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_prgb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_and_demultiply_1x1W(src0mm, src0mm);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_and_demultiply_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_rgb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);
      src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_rgb24_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        ((uint32_t*)dst)[0] = src0;
      }
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_argb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_argb32_span_composite_argb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_prgb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_argb32_span_composite_prgb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    pix_load4(dst0mm, dst);
    pix_unpack_and_demultiply_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_and_demultiply_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_rgb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_argb32_span_composite_rgb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_argb32_span_composite_rgb24_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_argb32_span_composite_rgb24_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src));
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - Src]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_src_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;
  ((uint32_t*)dst)[0] = src0;
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_src_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  if (msk != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, &src->rgbp);

    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_mask_1x1W(a0mm, msk);
    pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    __m128i src0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_store4(dst, src0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_src_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    pix_store4(dst, src0mm);
    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  while (w >= 4)
  {
    pix_store16a(dst, src0mm);
    pix_store16a(dst + 16, src0mm);
    pix_store16a(dst + 32, src0mm);
    pix_store16a(dst + 48, src0mm);

    dst += 64;
    w -= 4;
  }

  switch (w & 3)
  {
    case 3: pix_store16a(dst, src0mm); dst += 16;
    case 2: pix_store16a(dst, src0mm); dst += 16;
    case 1: pix_store16a(dst, src0mm); dst += 16;
  }

  if ((_i = _j)) { w = 0; goto blt; }
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_src_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt_opaque)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);
    if (msk0 == 0xFF)
    {
      pix_store4(dst, src0orig);
    }
    else if (msk0)
    {
      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt_opaque)

  BLIT_SSE2_LARGE_BEGIN(blt_opaque)
    __m128i dst0mm, dst1mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);
    if (msk0 != 0x00000000)
    {
      if (msk0 == 0xFFFFFFFF)
      {
        pix_store16a(dst, src0orig);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt_opaque)
}

#define raster_prgb32_span_composite_argb32_src_sse2 convert_prgb32_from_argb32_sse2
#define raster_prgb32_span_composite_prgb32_src_sse2 convert_memcpy32_sse2
#define raster_prgb32_span_composite_rgb32_src_sse2 convert_argb32_from_rgb32_sse2
#define raster_prgb32_span_composite_rgb24_src_sse2 convert_rgb32_from_rgb24_sse2

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_premultiply_1x1W(src0mm, src0mm);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_pack_1x1W(src0mm, src0mm);
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);
      src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_store4(dst, src0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_src_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        ((uint32_t*)dst)[0] = src0;
      }
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_argb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_prgb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_rgb32_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_const_src_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_rgb24_src_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src));
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_ialpha_1x1W(dst0mm, src0mm, m0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_interpolate_ialpha_2x2W(dst0mm, src0mm, m0mm, im0mm, dst1mm, src1mm, m0mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Rgb32 - Src]
// ============================================================================

static void FOG_FASTCALL raster_rgb32_pixel_src_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgba | 0xFF000000;
  ((uint32_t*)dst)[0] = src0;
}

static void FOG_FASTCALL raster_rgb32_pixel_a8_src_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgba | 0xFF000000;

  if (msk != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgba);
    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_interpolate_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_rgb32_span_solid_src_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgba);
  src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    pix_store4(dst, src0mm);
    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  while (w >= 4)
  {
    pix_store16a(dst, src0mm);
    pix_store16a(dst + 16, src0mm);
    pix_store16a(dst + 32, src0mm);
    pix_store16a(dst + 48, src0mm);

    dst += 64;
    w -= 4;
  }
  switch (w & 3)
  {
    case 3: pix_store16a(dst, src0mm); dst += 16;
    case 2: pix_store16a(dst, src0mm); dst += 16;
    case 1: pix_store16a(dst, src0mm); dst += 16;
  }

  if ((_i = _j)) { w = 0; goto blt; }
}

static void FOG_FASTCALL raster_rgb32_span_solid_a8_src_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgba);
  src0orig = _mm_or_si128(src0orig, MaskFF000000FF000000);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);
    if (msk0 != 0x00)
    {
      if (msk0 == 0xFF)
      {
        pix_store4(dst, src0orig);
      }
      else
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_interpolate_1x1W(dst0mm, src0unpacked, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);
    if (msk0 != 0x00000000)
    {
      if (msk0 == 0xFFFFFFFF)
      {
        pix_store16a(dst, src0orig);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_interpolate_2x2W(dst0mm, src0unpacked, a0mm, dst1mm, src0unpacked, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_rgb32_span_composite_argb32_src_sse2 convert_argb32_from_rgb32_sse2
#define raster_rgb32_span_composite_prgb32_src_sse2 convert_argb32_from_prgb32_sse2
#define raster_rgb32_span_composite_rgb32_src_sse2 convert_memcpy32_sse2
#define raster_rgb32_span_composite_rgb24_src_sse2 convert_rgb32_from_rgb24_sse2

#define raster_rgb32_span_composite_argb32_a8_src_sse2 raster_rgb32_span_composite_rgb32_a8_srcover_sse2
#define raster_rgb32_span_composite_rgb32_a8_src_sse2 raster_rgb32_span_composite_rgb32_a8_srcover_sse2
#define raster_rgb32_span_composite_rgb24_a8_src_sse2 raster_rgb32_span_composite_rgb24_a8_srcover_sse2

#define raster_rgb32_span_composite_argb32_a8_const_src_sse2 raster_rgb32_span_composite_rgb32_a8_const_srcover_sse2
#define raster_rgb32_span_composite_rgb32_a8_const_src_sse2 raster_rgb32_span_composite_rgb32_a8_const_srcover_sse2
#define raster_rgb32_span_composite_rgb24_a8_const_src_sse2 raster_rgb32_span_composite_rgb24_a8_const_srcover_sse2

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - SrcOver]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_srcover_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;

  if ((src0 >> 24) != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;

    pix_unpack_1x1W(src0mm, src0);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgbp;

  if (((src0 >> 24) != 0xFF) | (msk != 0xFF))
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(a0mm, msk);
    pix_expand_alpha_rev_1x1W(a0mm, a0mm);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_srcover_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  if ((src->rgbp >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      pix_store4(dst, src0mm);
      dst += 4;
    BLIT_SSE2_SMALL_END(blt_opaque)

    while (w >= 4)
    {
      pix_store16a(dst, src0mm);
      pix_store16a(dst + 16, src0mm);
      pix_store16a(dst + 32, src0mm);
      pix_store16a(dst + 48, src0mm);

      dst += 64;
      w -= 4;
    }
    switch (w & 3)
    {
      case 3: pix_store16a(dst, src0mm); dst += 16;
      case 2: pix_store16a(dst, src0mm); dst += 16;
      case 1: pix_store16a(dst, src0mm); dst += 16;
    }

    if ((_i = _j)) { w = 0; goto blt_opaque; }
    return;
  }
  else
  {
    __m128i ia0mm;
    pix_unpack_1x2W(src0mm, src0mm);
    pix_expand_alpha_1x2W(ia0mm, src0mm);
    pix_negate_1x1W(ia0mm, ia0mm);

    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;

      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_over_ialpha_1x1W(dst0mm, src0mm, ia0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);

      dst += 4;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i dst1mm;

      pix_load16a(dst0mm, dst);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_over_ialpha_2x2W(dst0mm, src0mm, ia0mm, dst1mm, src0mm, ia0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);

      dst += 16;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  if ((src->rgbp >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)) == 0xFF)
      {
        pix_store4(dst, src0orig);
      }
      else if (msk0)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_alpha_rev_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_opaque)

    BLIT_SSE2_LARGE_BEGIN(blt_opaque)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        if (msk0 == 0xFFFFFFFF)
        {
          pix_store16a(dst, src0orig);
        }
        else
        {
          pix_load16a(dst0mm, dst);
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

          pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
          a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

          a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
          a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

          pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
          pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

          pix_store16a(dst, dst0mm);
        }
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_opaque)
  }
  else
  {
    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)))
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_alpha_rev_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

        pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
        a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

        a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
        a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

        pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_srcover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    if (src0 >> 24)
    {
      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);

      pix_expand_alpha_1x1W(a0mm, src0mm);
      pix_fill_alpha_1x1W(src0mm);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);

      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    if (src0a != 0x00000000)
    {
      if (src0a != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);

        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_srcover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);

    if (src0)
    {
      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_expand_alpha_1x1W(a0mm, src0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_prgb32_span_composite_rgb32_srcover_sse2 convert_argb32_from_rgb32_sse2
#define raster_prgb32_span_composite_rgb24_srcover_sse2 convert_rgb32_from_rgb24_sse2

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((src0a != 0x00) & (msk0 != 0x00))
    {
      msk0 = alphamul(src0a, msk0);

      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);

      pix_expand_mask_1x1W(a0mm, msk0);
      pix_fill_alpha_1x1W(src0mm);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    uint32_t msk0 = ((uint32_t*)msk)[0];

    if ((src0a != 0x00000000) & (msk0 != 0x00000000))
    {
      if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(dst0mm, dst1mm, msk0);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_multiply_2x2W(a0mm, a0mm, dst0mm, a1mm, a1mm, dst1mm);

        pix_load16a(dst0mm, dst);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t msk0 = READ_8(msk);

    if ((src0 != 0x00000000) & (msk0 != 0x00))
    {
      pix_load4(dst0mm, dst);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);

      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_expand_alpha_1x1W(a0mm, src0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    uint32_t msk0 = ((uint32_t*)msk)[0];

    if ((src0a != 0x00000000) & (msk0 != 0x00000000))
    {
      if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_unpack_1x1W(src0mm, src0mm);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(src0mm, dst0mm);
      }

      pix_store4(dst, src0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = ((uint32_t*)msk)[0];

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(src0mm, dst0mm, dst1mm);
      }
      pix_store16a(dst, src0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_unpack_1x1W(src0mm, src0);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        ((uint32_t*)dst)[0] = src0;
      }
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = ((uint32_t*)msk)[0];

    if (msk0 != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(dst0mm, src0mm, src1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_argb32_srcover_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;

    if ((src0a != 0x00))
    {
      if ((src0a == 0xFF))
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
      else
      {
        uint32_t msk0 = alphamul(src0a, msk);

        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a == 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_multiply_2x2W(a0mm, a0mm, m0mm, a1mm, a1mm, m0mm);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_argb32_srcover_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;

    if ((src0a != 0x00))
    {
      if ((src0a == 0xFF))
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_load4(dst0mm, dst);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a == 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_prgb32_span_composite_rgb32_srcover_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  __m128i amask = MaskFF000000FF000000;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t src0 = READ_32(src) | 0xFF000000;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(dst0mm, dst0mm);

    pix_multiply_1x1W(src0mm, src0mm, m0mm);
    pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    src0mm = _mm_or_si128(src0mm, amask);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
    pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    convert_rgb32_from_rgb24_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(dst0mm, dst0mm);

    pix_multiply_1x1W(src0mm, src0mm, m0mm);
    pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
    pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Argb32 - SrcOver]
// ============================================================================

static FOG_INLINE void pix_over_nonpremultiplied_1x1W(__m128i& dst0, const __m128i& src0)
{
  __m128i alpha0;
  __m128i tmp0;

  pix_expand_alpha_1x1W(alpha0, src0);
  pix_multiply_1x1W(tmp0, src0, alpha0);
  pix_negate_1x1W(alpha0, alpha0);
  pix_multiply_1x1W(dst0, dst0, alpha0);
  dst0 = _mm_adds_epu8(dst0, tmp0);
}

// znacka
static void FOG_FASTCALL raster_argb32_pixel_srcover_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;

  if ((src0 >> 24) != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_premultiply_1x1W(dst0mm, dst0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_demultiply_1x1W(dst0mm, dst0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_argb32_pixel_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgba;

  if (((src0 >> 24) != 0xFF) | (msk != 0xFF))
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_argb32_span_solid_srcover_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  uint32_t src0 = src->rgba;
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  if ((src0 >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      ((uint32_t*)dst)[0] = src0;
      dst += 4;
    BLIT_SSE2_SMALL_END(blt_opaque)

    while (w >= 4)
    {
      pix_store16a(dst, src0mm);
      pix_store16a(dst + 16, src0mm);
      pix_store16a(dst + 32, src0mm);
      pix_store16a(dst + 48, src0mm);

      dst += 64;
      w -= 4;
    }
    switch (w & 3)
    {
      case 3: pix_store16a(dst, src0mm); dst += 16;
      case 2: pix_store16a(dst, src0mm); dst += 16;
      case 1: pix_store16a(dst, src0mm); dst += 16;
    }

    if ((_i = _j)) { w = 0; goto blt_opaque; }
    return;
  }
  else
  {
    __m128i ia0mm;
    pix_unpack_1x2W(src0mm, src0mm);
    pix_expand_alpha_1x2W(ia0mm, src0mm);
    pix_negate_1x1W(ia0mm, ia0mm);

    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;

      pix_load4(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_over_ialpha_1x1W(dst0mm, src0mm, ia0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);

      dst += 4;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i dst1mm;

      pix_load16a(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_over_ialpha_2x2W(dst0mm, src0mm, ia0mm, dst1mm, src0mm, ia0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);

      dst += 16;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

static void FOG_FASTCALL raster_argb32_span_solid_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  uint32_t src0 = src->rgba;
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  if ((src0 >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)) == 0xFF)
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else if (msk0)
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_opaque)

    BLIT_SSE2_LARGE_BEGIN(blt_opaque)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        if (msk0 == 0xFFFFFFFF)
        {
          pix_store16a(dst, src0orig);
        }
        else
        {
          pix_load16a(dst0mm, dst);
          dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

          pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
          a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

          a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
          a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

          pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
          pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

          pix_store16a(dst, dst0mm);
        }
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_opaque)
  }
  else
  {
    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)))
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

        pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
        a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

        a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
        a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

        pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

// ============================================================================
// [Fog::Raster - Raster - Rgb32 - SrcOver]
// ============================================================================

static void FOG_FASTCALL raster_rgb32_pixel_srcover_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;

  if ((src0 >> 24) != 0xFF)
  {
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_rgb32_pixel_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgbp;

  if (((src0 >> 24) != 0xFF) | (msk != 0xFF))
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_over_1x1W(dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
  else
  {
    ((uint32_t*)dst)[0] = src0;
  }
}

static void FOG_FASTCALL raster_rgb32_span_solid_srcover_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  uint32_t src0 = src->rgbp;
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  if ((src0 >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      ((uint32_t*)dst)[0] = src0;
      dst += 4;
    BLIT_SSE2_SMALL_END(blt_opaque)

    while (w >= 4)
    {
      pix_store16a(dst, src0mm);
      pix_store16a(dst + 16, src0mm);
      pix_store16a(dst + 32, src0mm);
      pix_store16a(dst + 48, src0mm);

      dst += 64;
      w -= 4;
    }
    switch (w & 3)
    {
      case 3: pix_store16a(dst, src0mm); dst += 16;
      case 2: pix_store16a(dst, src0mm); dst += 16;
      case 1: pix_store16a(dst, src0mm); dst += 16;
    }

    if ((_i = _j)) { w = 0; goto blt_opaque; }
    return;
  }
  else
  {
    __m128i ia0mm;
    pix_unpack_1x2W(src0mm, src0mm);
    pix_expand_alpha_1x2W(ia0mm, src0mm);
    pix_negate_1x1W(ia0mm, ia0mm);

    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;

      pix_load4(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_over_ialpha_1x1W(dst0mm, src0mm, ia0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);

      dst += 4;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i dst1mm;

      pix_load16a(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_over_ialpha_2x2W(dst0mm, src0mm, ia0mm, dst1mm, src0mm, ia0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);

      dst += 16;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

static void FOG_FASTCALL raster_rgb32_span_solid_a8_srcover_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  uint32_t src0 = src->rgbp;
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w);

  if ((src0 >> 24) == 0xFF)
  {
    BLIT_SSE2_SMALL_BEGIN(blt_opaque)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)) == 0xFF)
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else if (msk0)
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_opaque)

    BLIT_SSE2_LARGE_BEGIN(blt_opaque)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        if (msk0 == 0xFFFFFFFF)
        {
          pix_store16a(dst, src0orig);
        }
        else
        {
          pix_load16a(dst0mm, dst);
          dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

          pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
          a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

          a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
          a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

          pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
          pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

          pix_store16a(dst, dst0mm);
        }
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_opaque)
  }
  else
  {
    BLIT_SSE2_SMALL_BEGIN(blt_trans)
      __m128i dst0mm;
      __m128i src0mm;
      __m128i a0mm;

      uint32_t msk0;
      if ((msk0 = READ_8(msk)))
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_expand_pixel_1x2W(a0mm, a0mm);
        pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }

      dst += 4;
      msk += 1;
    BLIT_SSE2_SMALL_END(blt_trans)

    BLIT_SSE2_LARGE_BEGIN(blt_trans)
      __m128i dst0mm, dst1mm;
      __m128i src0mm, src1mm;
      __m128i a0mm, a1mm;

      uint32_t msk0;
      if ((msk0 = ((uint32_t*)msk)[0]))
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

        pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
        a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

        a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
        a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

        pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt_trans)
  }
}

static void FOG_FASTCALL raster_rgb32_span_composite_argb32_srcover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;
    if (src0a != 0x00)
    {
      if (src0a != 0xFF)
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);

        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
      else
      {
        ((uint32_t*)dst)[0] = src0;
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    if (src0a != 0x00000000)
    {
      if (src0a != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);

        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_prgb32_srcover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);

    if (src0)
    {
      pix_load4(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);
      pix_expand_alpha_1x1W(a0mm, src0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_store16a(dst, src0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_rgb32_span_composite_rgb32_srcover_sse2 convert_memcpy32_sse2
#define raster_rgb32_span_composite_rgb24_srcover_sse2 convert_rgb32_from_rgb24_sse2

static void FOG_FASTCALL raster_rgb32_span_composite_argb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((src0a != 0x00) & (msk0 != 0x00))
    {
      msk0 = alphamul(src0a, msk0);

      pix_load4(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);

      pix_expand_mask_1x1W(a0mm, msk0);
      pix_fill_alpha_1x1W(src0mm);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    uint32_t msk0 = ((uint32_t*)msk)[0];

    if ((src0a != 0x00000000) & (msk0 != 0x00000000))
    {
      if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(dst0mm, dst1mm, msk0);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_multiply_2x2W(a0mm, a0mm, dst0mm, a1mm, a1mm, dst1mm);

        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_prgb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t msk0 = READ_8(msk);

    if ((src0 != 0x00000000) & (msk0 != 0x00))
    {
      pix_load4(dst0mm, dst);
      dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0mm);

      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_expand_alpha_1x1W(a0mm, src0mm);
      pix_over_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);
    uint32_t msk0 = ((uint32_t*)msk)[0];

    if ((src0a != 0x00000000) & (msk0 != 0x00000000))
    {
      if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_rgb32_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      pix_load4(src0mm, src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_unpack_1x1W(src0mm, src0mm);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(src0mm, dst0mm);
      }

      pix_store4(dst, src0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = READ_32(msk);

    if (msk0 != 0x00000000)
    {
      pix_load16u(src0mm, src);
      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(src0mm, dst0mm, dst1mm);
      }
      pix_store16a(dst, src0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_rgb24_a8_srcover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0x00)
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src);

      if (msk0 != 0xFF)
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_unpack_1x1W(src0mm, src0);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
      else
      {
        ((uint32_t*)dst)[0] = src0;
      }
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    uint32_t msk0 = ((uint32_t*)msk)[0];

    if (msk0 != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
      if (msk0 != 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_pack_2x2W(dst0mm, src0mm, src1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_argb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_rgb32_span_composite_argb32_srcover_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;

    if ((src0a != 0x00))
    {
      if ((src0a == 0xFF))
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
      else
      {
        uint32_t msk0 = alphamul(src0a, msk);

        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_expand_mask_1x1W(a0mm, msk0);
        pix_fill_alpha_1x1W(src0mm);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a == 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_multiply_2x2W(a0mm, a0mm, m0mm, a1mm, a1mm, m0mm);

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_fill_alpha_2x2W(src0mm, src1mm);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_prgb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    raster_rgb32_span_composite_argb32_srcover_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t src0 = READ_32(src);
    uint32_t src0a = src0 >> 24;

    if ((src0a != 0x00))
    {
      if ((src0a == 0xFF))
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);

        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
      else
      {
        pix_load4(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
        pix_unpack_1x1W(src0mm, src0);
        pix_unpack_1x1W(dst0mm, dst0mm);
        pix_multiply_1x1W(src0mm, src0mm, m0mm);
        pix_expand_alpha_1x1W(a0mm, src0mm);
        pix_over_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);

        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16u(src0mm, src);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if (src0a != 0x00000000)
    {
      if (src0a == 0xFFFFFFFF)
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
      else
      {
        pix_load16a(dst0mm, dst);
        dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);

        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
        pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
        pix_over_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_rgb32_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    convert_memcpy32_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  __m128i amask = MaskFF000000FF000000;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t src0 = READ_32(src) | 0xFF000000;

    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, amask);
    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(dst0mm, dst0mm);

    pix_multiply_1x1W(src0mm, src0mm, m0mm);
    pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16u(src0mm, src);
    pix_load16a(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, amask);
    src0mm = _mm_or_si128(src0mm, amask);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
    pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_rgb32_span_composite_rgb24_a8_const_srcover_sse2(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w)
{
  if (FOG_UNLIKELY(msk == 0xFF))
  {
    convert_rgb32_from_rgb24_sse2(dst, src, w);
    return;
  }

  __m128i m0mm, im0mm;
  pix_expand_mask_1x1W(m0mm, msk);
  pix_expand_pixel_1x2W(m0mm, m0mm);
  pix_negate_1x2W(im0mm, m0mm);

  BLIT_SSE2_INIT(dst, w);

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;

    pix_load4(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(dst0mm, dst0mm);

    pix_multiply_1x1W(src0mm, src0mm, m0mm);
    pix_over_ialpha_1x1W(dst0mm, src0mm, im0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    dst0mm = _mm_or_si128(dst0mm, MaskFF000000FF000000);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m0mm);
    pix_over_ialpha_2x2W(dst0mm, src0mm, im0mm, dst1mm, src1mm, im0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - DstOver]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_dstover_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;
  uint32_t dst0 = READ_32(dst);

  if ((dst0 >> 24) != 0xFF && src0 != 0x00000000)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_unpack_1x1W(src0mm, src0);
    pix_unpack_1x1W(dst0mm, dst0);
    pix_expand_alpha_1x1W(a0mm, dst0mm);
    pix_overrev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_dstover_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgbp;
  uint32_t dst0 = READ_32(dst);

  if ((dst0 >> 24) != 0xFF && src0 != 0x00000000 && msk != 0x00)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_unpack_1x1W(src0mm, src0);
    pix_expand_mask_1x1W(a0mm, msk);
    pix_unpack_1x1W(dst0mm, dst0);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, dst0mm);
    pix_overrev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_dstover_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0orig;
  __m128i src0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;

    if (dst0a != 0xFF)
    {
      if (dst0a == 0x00)
      {
        pix_store4(dst, src0orig);
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a != 0xFFFFFFFF)
    {
      if (dst0a == 0x00000000)
      {
        pix_store16a(dst, src0orig);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src0mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_dstover_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0unpacked;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x2W(src0orig, src0orig);
  pix_unpack_1x2W(src0unpacked, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0;

    if ((dst0 >> 24) != 0xFF && (msk0 = READ_8(msk)))
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_expand_pixel_1x1W(a0mm, a0mm);
      pix_multiply_1x1W(src0mm, src0unpacked, a0mm);
      pix_expand_alpha_1x1W(a0mm, dst0mm);
      pix_overrev_1x1W(dst0mm, src0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0;

    if (dst0a != 0xFFFFFFFF && (msk0 = ((uint32_t*)msk)[0]))
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);

      pix_unpack_1x1W(a0mm, _mm_cvtsi32_si128(msk0));
      a0mm = _mm_unpacklo_epi16(a0mm, a0mm);

      a1mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(3, 3, 2, 2));
      a0mm = _mm_shuffle_epi32(a0mm, _MM_SHUFFLE(1, 1, 0, 0));

      pix_multiply_2x2W(src0mm, src0unpacked, a0mm, src1mm, src0unpacked, a1mm);
      pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
      pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);

      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_dstover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;

    if (dst0a != 0xFF)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_premultiply_1x1W(src0mm, src0mm);

      if (dst0a == 0x00)
      {
        pix_pack_1x1W(src0mm, src0mm);
        pix_store4(dst, src0mm);
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a != 0xFFFFFFFF)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);

      if (dst0a == 0x00000000)
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_dstover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;

    if (dst0a != 0xFF)
    {
      uint32_t src0 = READ_32(src);

      if (dst0a == 0x00)
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a != 0xFFFFFFFF)
    {
      pix_load16u(src0mm, src);

      if (dst0a == 0x00000000)
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_dstover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;

    if (dst0a != 0xFF)
    {
      uint32_t src0 = READ_32(src) | 0xFF000000;
      if (dst0a == 0x00)
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        src0mm = _mm_cvtsi32_si128(src0);
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a != 0xFFFFFFFF)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);

      if (dst0a == 0x00000000)
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_dstover_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;

    if (dst0a != 0xFF)
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;
      if (dst0a == 0x00)
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        src0mm = _mm_cvtsi32_si128(src0);
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a != 0xFFFFFFFF)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

      if (dst0a == 0x00000000)
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_dstover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((dst0a != 0xFF) & (msk0 != 0))
    {
      msk0 = alphamul(dst0a, msk0);
      pix_load4(src0mm, src);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);

      if (dst0a == 0x00)
      {
        pix_pack_1x1W(src0mm, src0mm);
        pix_store4(dst, src0mm);
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a != 0xFFFFFFFF) & (msk0 != 0x00000000))
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_mask_2x2W(a0mm, a0mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);

      if (dst0a == 0x00000000)
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_dstover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((dst0a != 0xFF) & (msk0 != 0x00))
    {
      uint32_t src0 = READ_32(src);

      if ((dst0a == 0x00) & (msk0 == 0xFF))
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a != 0xFFFFFFFF) & (msk0 != 0x00000000))
    {
      pix_load16u(src0mm, src);

      if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_dstover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((dst0a != 0xFF) & (msk0 != 0x00))
    {
      uint32_t src0 = READ_32(src) | 0xFF000000;
      if ((dst0a == 0x00) & (msk0 == 0xFF))
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a != 0xFFFFFFFF) & (msk0 != 0x00000000))
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);

      if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_dstover_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t dst0a = dst0 >> 24;
    uint32_t msk0 = READ_8(msk);

    if ((dst0a != 0xFF) & (msk0 != 0x00))
    {
      uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;
      if ((dst0a == 0x00) & (msk0 == 0xFF))
      {
        ((uint32_t*)dst)[0] = src0;
      }
      else
      {
        src0mm = _mm_cvtsi32_si128(src0);
        pix_unpack_1x1W(dst0mm, dst0);
        pix_unpack_1x1W(src0mm, src0mm);
        pix_expand_mask_1x1W(a0mm, msk0);
        pix_multiply_1x1W(src0mm, src0mm, a0mm);
        pix_expand_alpha_1x1W(a0mm, dst0mm);
        pix_overrev_1x1W(dst0mm, src0mm, a0mm);
        pix_pack_1x1W(dst0mm, dst0mm);
        pix_store4(dst, dst0mm);
      }
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a != 0xFFFFFFFF) & (msk0 != 0x00000000))
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

      if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
      else
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
        pix_expand_alpha_2x2W(a0mm, dst0mm, a1mm, dst1mm);
        pix_overrev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - SrcIn]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_srcin_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, dst0 >> 24);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_pack_1x1W(src0mm, src0mm);
    pix_store4(dst, src0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_srcin_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i m0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(dst0mm, dst0 >> 24);
    pix_expand_mask_1x1W(m0mm, msk);
    pix_multiply_1x1W(dst0mm, dst0mm, m0mm);
    pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_srcin_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    uint32_t dst0 = READ_32(dst);

    if (dst0 != 0x00000000)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_srcin_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_srcin_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_premultiply_1x1W(src0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_srcin_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_srcin_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_srcin_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src) | 0xFF000000);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_srcin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_premultiply_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_srcin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_srcin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_load4(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, amask);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_srcin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src) | 0xFF000000);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - DstIn]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_dstin_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    __m128i dst0mm;
    __m128i src0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0);
    pix_expand_alpha_1x1W(src0mm, src0mm);
    pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_dstin_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    __m128i dst0mm;
    __m128i src0mm;
    __m128i m0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0);
    pix_expand_alpha_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(m0mm, msk);
    pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
    pix_multiply_1x1W(dst0mm, dst0mm, m0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_dstin_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];
    if (dst0 != 0x00000000)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_dstin_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];

    if (dst0 != 0x00000000)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_axxx32_dstin_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];

    if (dst0 != 0x00000000)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(src0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_axxx32_a8_dstin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];

    if (dst0 != 0x00000000)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_unpack_1x1W(a0mm, READ_8(msk));
      pix_expand_alpha_1x1W(src0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_xxxx_a8_dstin_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  FOG_UNUSED(src);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];
    if (dst0 != 0x00000000)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - SrcOut]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_srcout_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t dst0 = READ_32(dst);

  if (dst0 == 0x00000000)
  {
    ((uint32_t*)dst)[0] = src->rgbp;
  }
  else
  {
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(src0mm, src);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, (~dst0) >> 24);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_pack_1x1W(src0mm, src0mm);
    pix_store4(dst, src0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_srcout_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t dst0 = READ_32(dst);

  if ((dst0 == 0x00000000) & (msk == 0xFF))
  {
    ((uint32_t*)dst)[0] = src->rgbp;
  }
  else
  {
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(src0mm, src);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, alphamul((~dst0) >> 24, msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_pack_1x1W(src0mm, src0mm);
    pix_store4(dst, src0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_srcout_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0orig;
  __m128i src0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    if (dst0 == 0x00000000)
    {
      pix_store4(dst, src0orig);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a == 0x00000000)
    {
      pix_store16a(dst, src0orig);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_srcout_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0orig;
  __m128i src0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);

    if ((dst0 == 0x00000000) & (msk0 == 0xFF))
    {
      pix_store4(dst, src0orig);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(a0mm, a0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
    {
      pix_store16a(dst, src0orig);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src0mm);
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_srcout_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);
    pix_load4(src0mm, src);

    if (dst0 == 0x00000000)
    {
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_premultiply_1x1W(src0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a == 0x00000000)
    {
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_srcout_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);
    pix_load4(src0mm, src);

    if (dst0 == 0x00000000)
    {
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a == 0x00000000)
    {
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_srcout_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);

    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    if (dst0 == 0x00000000)
    {
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a == 0x00000000)
    {
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_srcout_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;

    if (dst0 == 0x00000000)
    {
      ((uint32_t*)dst)[0] = src0;
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

    if (dst0a == 0x00000000)
    {
      pix_pack_2x2W(src0mm, src0mm, src1mm);
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_srcout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, src);

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);

    pix_unpack_1x1W(src0mm, src0mm);
    pix_premultiply_1x1W(src0mm, src0mm);

    if ((dst0 == 0x00000000) & (msk0 == 0xFF))
    {
      pix_pack_1x1W(src0mm, src0mm);
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);

    if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
    {
      pix_pack_2x2W(src0mm, src0mm, src1mm);
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_srcout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);
    pix_load4(src0mm, src);

    if ((dst0 == 0x00000000) & (msk0 == 0xFF))
    {
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
    {
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_srcout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);
    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    if ((dst0 == 0x00000000) & (msk0 == 0xFF))
    {
      pix_store4(dst, src0mm);
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);

    if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
    {
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_srcout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);
    uint32_t src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;

    if ((dst0 == 0x00000000) & (msk0 == 0xFF))
    {
      ((uint32_t*)dst)[0] = src0;
    }
    else
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0);
      pix_expand_alpha_1x1W(dst0mm, dst0mm);
      pix_negate_1x1W(dst0mm, dst0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0 = READ_32(msk);


    if ((dst0a == 0x00000000) & (msk0 == 0xFFFFFFFF))
    {
      pix_pack_2x2W(src0mm, src0mm, src1mm);
      pix_store16a(dst, src0mm);
    }
    else
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_alpha_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_negate_2x2W(dst0mm, dst0mm, dst1mm, dst1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - DstOut]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_dstout_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t dst0 = READ_32(dst);
  uint32_t src0 = src->rgbp;

  if ((dst0 != 0x00000000) & (src0 != 0x00000000))
  {
    __m128i dst0mm;
    __m128i src0mm;

    pix_unpack_1x1W(dst0mm, dst0);
    pix_expand_mask_1x1W(src0mm, (~src0) >> 24);
    pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_dstout_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t dst0 = READ_32(dst);
  uint32_t src0ia = alphaneg(alphamul(src->rgbp >> 24, msk));

  if ((dst0 != 0x00000000) & (src0ia != 0xFF))
  {
    __m128i dst0mm;
    __m128i src0mm;

    pix_unpack_1x1W(dst0mm, dst0);
    pix_expand_mask_1x1W(src0mm, src0ia);
    pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_dstout_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  if (src->rgbp == 0x00000000) return; // nop

  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(src0mm, src0mm);
  pix_negate_1x1W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    uint32_t dst0 = READ_32(dst);

    if (dst0 != 0x00000000)
    {
      __m128i dst0mm;

      pix_unpack_1x1W(dst0mm, dst0);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src0mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_dstout_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(src0mm, src0mm);
  pix_negate_1x1W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    uint32_t dst0 = READ_32(dst);

    if (dst0 != 0x00000000)
    {
      __m128i dst0mm;
      __m128i a0mm;

      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(a0mm, READ_8(msk));
      pix_multiply_1x1W(a0mm, a0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  if (src->rgbp == 0x00000000)
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;
      __m128i a0mm, a1mm;

      pix_load16a(dst0mm, dst);

      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
      if (dst0a != 0x00000000)
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
        pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt)
  }
  else
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;
      __m128i a0mm, a1mm;

      pix_load16a(dst0mm, dst);

      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
      if (dst0a != 0x00000000)
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
        pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src0mm);
        pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt)
  }
}

static void FOG_FASTCALL raster_prgb32_span_composite_axxx32_dstout_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t src0 = READ_32(src);

    if ((dst0 != 0x00000000) & (src0 != 0x00000000))
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(src0mm, (~src0) >> 24);
      pix_multiply_1x1W(dst0mm, dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_xor_si128(src0mm, MaskFFFFFFFFFFFFFFFF);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t src0a = pix_pack_alpha_to_uint32(src0mm);

    if ((dst0a != 0x00000000) & (src0a != 0xFFFFFFFF))
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, src0mm, dst1mm, dst1mm, src1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_axxx32_a8_dstout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];

    if (dst0 != 0x00000000)
    {
      pix_load4(src0mm, src);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0mm);
      pix_unpack_1x1W(a0mm, READ_8(msk));
      pix_expand_alpha_1x1W(src0mm, src0mm);
      pix_negate_1x1W(src0mm, src0mm);
      pix_multiply_1x1W(a0mm, a0mm, src0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_alpha_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_negate_2x2W(src0mm, src0mm, src1mm, src1mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_xxxx_a8_dstout_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  FOG_UNUSED(src);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = ((uint32_t*)dst)[0];

    if (dst0 != 0x00000000)
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(a0mm, READ_8(msk));
      pix_negate_1x1W(a0mm, a0mm);
      pix_multiply_1x1W(dst0mm, dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    if (dst0a != 0x00000000)
    {
      pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
      pix_negate_2x2W(a0mm, a0mm, a1mm, a1mm);
      pix_multiply_2x2W(dst0mm, dst0mm, a0mm, dst1mm, dst1mm, a1mm);
      pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - SrcAtop]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_srcatop_sse2(
  uint8_t* dst, const Solid* src)
{
  uint32_t src0 = src->rgbp;
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    if ((src0 >> 24) != 0xFF)
    {
      __m128i src0mm;
      __m128i dst0mm;

      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }
    else
    {
      ((uint32_t*)dst)[0] = src0;
    }
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_srcatop_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  uint32_t src0 = src->rgbp;
  uint32_t dst0 = READ_32(dst);

  if (dst0 != 0x00000000)
  {
    if (((src0 >> 24) != 0xFF) | (msk != 0xFF))
    {
      __m128i src0mm;
      __m128i dst0mm;
      __m128i a0mm;

      pix_unpack_1x1W(dst0mm, dst0);
      pix_unpack_1x1W(src0mm, src0);
      pix_expand_mask_1x1W(a0mm, msk);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }
    else
    {
      ((uint32_t*)dst)[0] = src0;
    }
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_srcatop_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  uint32_t src0 = src->rgbp;

  __m128i src0orig;
  __m128i src0mm;
  __m128i srcia0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);
  pix_expand_alpha_1x2W(srcia0mm, src0mm);
  pix_negate_1x1W(srcia0mm, srcia0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    uint32_t dst0 = READ_32(dst);
    if (dst0 != 0x00000000)
    {
      __m128i dst0mm;

      pix_unpack_1x1W(dst0mm, dst0);
      pix_atop_ialpha_1x1W(dst0mm, src0mm, srcia0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  if ((src0 >> 24) != 0xFF)
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;

      pix_load16a(dst0mm, dst);
      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

      if (dst0a != 0x00000000)
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_atop_ialpha_2x2W(dst0mm, src0mm, srcia0mm, dst1mm, src0mm, srcia0mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }

      dst += 16;
    BLIT_SSE2_LARGE_END(blt)
  }
  else
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;

      pix_load16a(dst0mm, dst);
      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);

      if (dst0a != 0x00000000)
      {
        if (dst0a == 0xFFFFFFFF)
        {
          pix_store16a(dst, src0orig);
        }
        else
        {
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
          pix_atop_ialpha_2x2W(dst0mm, src0mm, srcia0mm, dst1mm, src0mm, srcia0mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
          pix_store16a(dst, dst0mm);
        }
      }

      dst += 16;
    BLIT_SSE2_LARGE_END(blt)
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_srcatop_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  uint32_t src0 = src->rgbp;

  __m128i src0orig;
  __m128i src0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    uint32_t dst0 = READ_32(dst);
    uint32_t msk0 = READ_8(msk);

    if ((dst0 != 0x00000000) & (msk0 != 0x00))
    {
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(a0mm, a0mm, src0mm);
      pix_atop_1x1W(dst0mm, a0mm);
      pix_pack_1x1W(dst0mm, dst0mm);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  if ((src0 >> 24) != 0xFF)
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;
      __m128i a0mm, a1mm;

      pix_load16a(dst0mm, dst);
      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
      uint32_t msk0 = READ_32(msk);

      if ((dst0a != 0x00000000) & (msk0 != 0x00000000))
      {
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(a0mm, a1mm, msk0);
        pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src0mm);
        pix_atop_2x2W(dst0mm, a0mm, dst1mm, a1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt)
  }
  else
  {
    BLIT_SSE2_LARGE_BEGIN(blt)
      __m128i dst0mm, dst1mm;
      __m128i a0mm, a1mm;

      pix_load16a(dst0mm, dst);
      uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
      uint32_t msk0 = READ_32(msk);

      if ((dst0a != 0x00000000) & (msk0 != 0x00000000))
      {
        if ((dst0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
        {
          pix_store16a(dst, src0orig);
        }
        else
        {
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
          pix_expand_mask_2x2W(a0mm, a1mm, msk0);
          pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src0mm);
          pix_atop_2x2W(dst0mm, a0mm, dst1mm, a1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
          pix_store16a(dst, dst0mm);
        }
      }

      dst += 16;
      msk += 4;
    BLIT_SSE2_LARGE_END(blt)
  }
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0;
    if ((dst0 = READ_32(dst)) && (src0 = READ_32(src)))
    {
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_premultiply_1x1W(src0mm, src0mm);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t src0a;

    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);

      if ((src0a = pix_pack_alpha_to_uint32(src0mm)) != 0x00000000)
      {
        if ((dst0a == 0xFFFFFFFF) & (src0a == 0xFFFFFFFF))
        {
          pix_store16a(dst, src0mm);
        }
        else
        {
          pix_unpack_2x2W(src0mm, src1mm, src0mm);
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
          pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
          pix_atop_2x2W(dst0mm, src0mm, dst1mm, src1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
          pix_store16a(dst, dst0mm);
        }
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0;
    if ((dst0 = READ_32(dst)) && (src0 = READ_32(src)))
    {
      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t src0a;

    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0a = pix_pack_alpha_to_uint32(src0mm);

      if (src0a != 0x00000000)
      {
        if ((dst0a == 0xFFFFFFFF) & (src0a == 0xFFFFFFFF))
        {
          pix_store16a(dst, src0mm);
        }
        else
        {
          pix_unpack_2x2W(src0mm, src1mm, src0mm);
          pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
          pix_atop_2x2W(dst0mm, src0mm, dst1mm, src1mm);
          pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
          pix_store16a(dst, dst0mm);
        }
      }
    }

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_prgb32_span_composite_rgb32_srcatop_sse2 raster_prgb32_span_composite_rgb32_srcin_sse2
#define raster_prgb32_span_composite_rgb24_srcatop_sse2 raster_prgb32_span_composite_rgb24_srcin_sse2

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0, msk0;
    if ((dst0 = READ_32(dst)) && (src0 = READ_32(src)) && (msk0 = READ_8(msk)))
    {
      __m128i m0mm;

      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_premultiply_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(m0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, m0mm);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t src0a;
    uint32_t msk0;

    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);
      if ((src0a = pix_pack_alpha_to_uint32(src0mm)) != 0x00000000)
      {
        if ((msk0 = READ_32(msk)) != 0x00000000)
        {
          if ((dst0a == 0xFFFFFFFF) & (src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
          {
            pix_store16a(dst, src0mm);
          }
          else
          {
            __m128i m0mm, m1mm;

            pix_unpack_2x2W(src0mm, src1mm, src0mm);
            pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
            pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
            pix_expand_mask_2x2W(m0mm, m1mm, msk0);
            pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m1mm);
            pix_atop_2x2W(dst0mm, src0mm, dst1mm, src1mm);
            pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
            pix_store16a(dst, dst0mm);
          }
        }
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0, msk0;
    if ((dst0 = READ_32(dst)) && (src0 = READ_32(src)) && (msk0 = READ_8(msk)))
    {
      __m128i m0mm;

      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(m0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, m0mm);
      pix_atop_1x1W(dst0mm, src0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t src0a;
    uint32_t msk0;

    if (dst0a != 0x00000000)
    {
      pix_load16u(src0mm, src);

      if ((src0a = pix_pack_alpha_to_uint32(src0mm)) != 0x00000000)
      {
        if ((msk0 = READ_32(msk)) != 0x00000000)
        {
          if ((dst0a == 0xFFFFFFFF) & (src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
          {
            pix_store16a(dst, src0mm);
          }
          else
          {
            __m128i m0mm, m1mm;
            pix_unpack_2x2W(src0mm, src1mm, src0mm);
            pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
            pix_expand_mask_2x2W(m0mm, m1mm, msk0);
            pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m1mm);
            pix_atop_2x2W(dst0mm, src0mm, dst1mm, src1mm);
            pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
            pix_store16a(dst, dst0mm);
          }
        }
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0, msk0;
    if ((dst0 = READ_32(dst)) && (msk0 = READ_8(msk)))
    {
      __m128i m0mm;

      src0 = READ_32(src) | 0xFF000000;

      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(m0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, m0mm);
      pix_negate_1x1W(m0mm, m0mm);
      pix_atop_ialpha_1x1W(dst0mm, src0mm, m0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0;

    if (dst0a != 0x00000000 && (msk0 = READ_32(msk)) != 0x00000000)
    {
      pix_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);

      if ((dst0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_store16a(dst, src0mm);
      }
      else
      {
        __m128i m0mm, m1mm;
        pix_unpack_2x2W(src0mm, src1mm, src0mm);
        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(m0mm, m1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m1mm);
        pix_negate_2x2W(m0mm, m0mm, m1mm, m1mm);
        pix_atop_ialpha_2x2W(dst0mm, src0mm, m0mm, dst1mm, src1mm, m1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_srcatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    uint32_t dst0, src0, msk0;
    if ((dst0 = READ_32(dst)) && (msk0 = READ_8(msk)))
    {
      __m128i m0mm;

      src0 = PixFmt_RGB24::fetch(src) | 0xFF000000;

      pix_unpack_1x1W(src0mm, src0);
      pix_unpack_1x1W(dst0mm, dst0);
      pix_expand_mask_1x1W(m0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, m0mm);
      pix_negate_1x1W(m0mm, m0mm);
      pix_atop_ialpha_1x1W(dst0mm, src0mm, m0mm);
      pix_pack_1x1W(dst0mm, dst0mm);

      pix_store4(dst, dst0mm);
    }

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);

    uint32_t dst0a = pix_pack_alpha_to_uint32(dst0mm);
    uint32_t msk0;

    if (dst0a != 0x00000000 && (msk0 = READ_32(msk)) != 0x00000000)
    {
      pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

      if ((dst0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
      {
        pix_pack_2x2W(src0mm, src0mm, src1mm);
        pix_store16a(dst, src0mm);
      }
      else
      {
        __m128i m0mm, m1mm;

        pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
        pix_expand_mask_2x2W(m0mm, m1mm, msk0);
        pix_multiply_2x2W(src0mm, src0mm, m0mm, src1mm, src1mm, m1mm);
        pix_negate_2x2W(m0mm, m0mm, m1mm, m1mm);
        pix_atop_ialpha_2x2W(dst0mm, src0mm, m0mm, dst1mm, src1mm, m1mm);
        pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
        pix_store16a(dst, dst0mm);
      }
    }

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - DstAtop]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_dstatop_sse2(
  uint8_t* dst, const Solid* src)
{
  __m128i src0mm;
  __m128i dst0mm;
  __m128i a0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_load4(dst0mm, dst);
  pix_unpack_1x1W(src0mm, src0mm);
  pix_unpack_1x1W(dst0mm, dst0mm);
  pix_expand_alpha_1x1W(a0mm, src0mm);
  pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
  pix_pack_1x1W(dst0mm, dst0mm);

  pix_store4(dst, dst0mm);
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_dstatop_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  __m128i src0mm;
  __m128i dst0mm;
  __m128i a0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_load4(dst0mm, dst);
  pix_unpack_1x1W(src0mm, src0mm);
  pix_unpack_1x1W(dst0mm, dst0mm);
  pix_expand_mask_1x1W(a0mm, msk);
  pix_multiply_1x1W(src0mm, src0mm, a0mm);
  pix_expand_alpha_1x1W(a0mm, src0mm);
  pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
  pix_pack_1x1W(dst0mm, dst0mm);

  pix_store4(dst, dst0mm);
}

static void FOG_FASTCALL raster_prgb32_span_solid_dstatop_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  __m128i src0mm;
  __m128i a0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(a0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src0mm, a0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_dstatop_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;
    __m128i m0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_mask_1x1W(m0mm, READ_8(msk));
    pix_multiply_1x1W(m0mm, m0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, m0mm);
    pix_atoprev_1x1W(dst0mm, m0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;
    __m128i m0mm, m1mm;

    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_expand_mask_2x2W(m0mm, m1mm, READ_32(msk));
    pix_multiply_2x2W(m0mm, m0mm, src0mm, m1mm, m1mm, src0mm);
    pix_expand_alpha_2x2W(a0mm, m0mm, a1mm, m1mm);
    pix_atoprev_2x2W(dst0mm, m0mm, a0mm, dst1mm, m1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_prgb32_span_composite_rgb32_dstatop_sse2 raster_prgb32_span_composite_rgb32_dstover_sse2
#define raster_prgb32_span_composite_rgb24_dstatop_sse2 raster_prgb32_span_composite_rgb24_dstover_sse2

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_dstatop_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src) | 0xFF000000);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_atoprev_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_atoprev_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - Xor]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_xor_sse2(
  uint8_t* dst, const Solid* src)
{
  if (src->rgbp)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_xor_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  if (src->rgbp && msk)
  {
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(src0mm, &src->rgbp);
    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_mask_1x1W(a0mm, msk);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);

    pix_store4(dst, dst0mm);
  }
}

static void FOG_FASTCALL raster_prgb32_span_solid_xor_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  if ((src->rgbp >> 24) == 0xFF)
  {
    raster_prgb32_span_solid_srcout_sse2(dst, src, w);
    return;
  }

  __m128i src0mm;
  __m128i srcia0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);
  pix_expand_alpha_1x2W(srcia0mm, src0mm);
  pix_negate_1x1W(srcia0mm, srcia0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_xor_ialpha_1x1W(dst0mm, src0mm, srcia0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;

    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_xor_ialpha_2x2W(dst0mm, src0mm, srcia0mm, dst1mm, src0mm, srcia0mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_xor_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);
  pix_unpack_1x2W(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;
    __m128i m0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_expand_mask_1x1W(m0mm, READ_8(msk));
    pix_multiply_1x1W(m0mm, m0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, m0mm);
    pix_xor_1x1W(dst0mm, m0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;
    __m128i m0mm, m1mm;

    pix_load16a(dst0mm, dst);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_expand_mask_2x2W(m0mm, m1mm, READ_32(msk));
    pix_multiply_2x2W(m0mm, m0mm, src0mm, m1mm, m1mm, src0mm);
    pix_expand_alpha_2x2W(a0mm, m0mm, a1mm, m1mm);
    pix_xor_2x2W(dst0mm, m0mm, a0mm, dst1mm, m1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_xor_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_xor_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

#define raster_prgb32_span_composite_rgb32_xor_sse2 raster_prgb32_span_composite_rgb32_srcout_sse2
#define raster_prgb32_span_composite_rgb24_xor_sse2 raster_prgb32_span_composite_rgb24_srcout_sse2

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_xor_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_xor_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_expand_alpha_1x1W(a0mm, src0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_expand_alpha_2x2W(a0mm, src0mm, a1mm, src1mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_xor_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, MaskFF000000FF000000);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_xor_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_unpack_1x1W(dst0mm, dst0mm);
    pix_unpack_1x1W(src0mm, PixFmt_RGB24::fetch(src) | 0xFF000000);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_xor_1x1W(dst0mm, src0mm, a0mm);
    pix_pack_1x1W(dst0mm, dst0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm, src1mm;
    __m128i dst0mm, dst1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_unpack_2x2W(dst0mm, dst1mm, dst0mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a0mm);
    pix_xor_2x2W(dst0mm, src0mm, a0mm, dst1mm, src1mm, a1mm);
    pix_pack_2x2W(dst0mm, dst0mm, dst1mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

// ============================================================================
// [Fog::Raster - Raster - Prgb32 - Add]
// ============================================================================

static void FOG_FASTCALL raster_prgb32_pixel_add_sse2(
  uint8_t* dst, const Solid* src)
{
  if (src->rgbp == 0x00000000) return;

  __m128i dst0mm;
  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_load4(dst0mm, dst);
  dst0mm = _mm_adds_epu8(dst0mm, src0mm);
  pix_store4(dst, dst0mm);
}

static void FOG_FASTCALL raster_prgb32_pixel_a8_add_sse2(
  uint8_t* dst, const Solid* src, uint32_t msk)
{
  if (src->rgbp == 0x00000000) return;

  __m128i dst0mm;
  __m128i src0mm;
  __m128i a0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_load4(dst0mm, dst);

  if (msk)
  {
    pix_unpack_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, msk);
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_pack_1x1W(src0mm, src0mm);
  }

  dst0mm = _mm_adds_epu8(dst0mm, src0mm);
  pix_store4(dst, dst0mm);
}

static void FOG_FASTCALL raster_prgb32_span_solid_add_sse2(
  uint8_t* dst, const Solid* src, sysint_t w)
{
  if (src->rgbp == 0x00000000) return;

  __m128i src0mm;

  pix_load4(src0mm, &src->rgbp);
  pix_expand_pixel_1x4B(src0mm, src0mm);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;

    pix_load16a(dst0mm, dst);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_solid_a8_add_sse2(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w)
{
  if (src->rgbp == 0x00000000) return;

  __m128i src0orig;
  __m128i src0mm;

  pix_load4(src0orig, &src->rgbp);
  pix_expand_pixel_1x4B(src0orig, src0orig);
  pix_unpack_1x2W(src0mm, src0orig);

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    uint32_t msk0 = READ_8(msk);
    if (msk0 != 0xFF)
    {
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(a0mm, a0mm, src0mm);
      pix_pack_1x1W(a0mm, a0mm);
      dst0mm = _mm_adds_epu8(dst0mm, a0mm);
      pix_store4(dst, dst0mm);
    }
    else
    {
      dst0mm = _mm_adds_epu8(dst0mm, src0orig);
      pix_store4(dst, dst0mm);
    }

    dst += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    uint32_t msk0 = READ_32(msk);
    if (msk0 != 0xFFFFFFFF)
    {
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(a0mm, a0mm, src0mm, a1mm, a1mm, src0mm);
      pix_pack_2x2W(a0mm, a0mm, a1mm);
      dst0mm = _mm_adds_epu8(dst0mm, a0mm);
      pix_store16a(dst, dst0mm);
    }
    else
    {
      dst0mm = _mm_adds_epu8(dst0mm, src0orig);
      pix_store16a(dst, dst0mm);
    }

    dst += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_add_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_pack_1x1W(src0mm, src0mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_pack_2x2W(src0mm, src0mm, src1mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_argb32_a8_add_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    pix_unpack_1x1W(src0mm, src0mm);
    pix_premultiply_1x1W(src0mm, src0mm);
    pix_expand_mask_1x1W(a0mm, READ_8(msk));
    pix_multiply_1x1W(src0mm, src0mm, a0mm);
    pix_pack_1x1W(src0mm, src0mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    pix_unpack_2x2W(src0mm, src1mm, src0mm);
    pix_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
    pix_expand_mask_2x2W(a0mm, a1mm, READ_32(msk));
    pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
    pix_pack_2x2W(src0mm, src0mm, src1mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_add_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i src0mm;
    __m128i dst0mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_prgb32_a8_add_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0xFF)
    {
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_pack_1x1W(src0mm, src0mm);
    }

    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0xFFFFFFFF)
    {
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_pack_2x2W(src0mm, src0mm, src1mm);
    }

    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_add_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb32_a8_add_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  __m128i amask = MaskFF000000FF000000;

  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    pix_load4(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0xFF)
    {
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_pack_1x1W(src0mm, src0mm);
    }

    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 4;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_load16u(src0mm, src);
    src0mm = _mm_or_si128(src0mm, amask);

    uint32_t msk0 = READ_8(msk);

    if (msk0 != 0xFFFFFFFF)
    {
      pix_unpack_2x2W(src0mm, src1mm, src0mm);
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
      pix_pack_2x2W(src0mm, src0mm, src1mm);
    }

    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 16;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_add_sse2(
  uint8_t* dst, const uint8_t* src, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;

    pix_load4(dst0mm, dst);
    src0mm = _mm_cvtsi32_si128(PixFmt_RGB24::fetch(src) | 0xFF000000);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);
    pix_pack_2x2W(src0mm, src0mm, src1mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
  BLIT_SSE2_LARGE_END(blt)
}

static void FOG_FASTCALL raster_prgb32_span_composite_rgb24_a8_add_sse2(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w)
{
  BLIT_SSE2_INIT(dst, w)

  BLIT_SSE2_SMALL_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm;
    __m128i a0mm;

    pix_load4(dst0mm, dst);
    src0mm = _mm_cvtsi32_si128(PixFmt_RGB24::fetch(src) | 0xFF000000);

    uint32_t msk0 = READ_8(msk);
    if (msk0 != 0xFF)
    {
      pix_unpack_1x1W(src0mm, src0mm);
      pix_expand_mask_1x1W(a0mm, msk0);
      pix_multiply_1x1W(src0mm, src0mm, a0mm);
      pix_pack_1x1W(src0mm, src0mm);
    }

    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store4(dst, dst0mm);

    dst += 4;
    src += 3;
    msk += 1;
  BLIT_SSE2_SMALL_END(blt)

  BLIT_SSE2_LARGE_BEGIN(blt)
    __m128i dst0mm;
    __m128i src0mm, src1mm;
    __m128i a0mm, a1mm;

    pix_load16a(dst0mm, dst);
    pix_fetch_rgb24_2x2W(src0mm, src1mm, src);

    uint32_t msk0 = READ_8(msk);
    if (msk0 != 0xFFFFFFFF)
    {
      pix_expand_mask_2x2W(a0mm, a1mm, msk0);
      pix_multiply_2x2W(src0mm, src0mm, a0mm, src1mm, src1mm, a1mm);
    }

    pix_pack_2x2W(src0mm, src0mm, src1mm);
    dst0mm = _mm_adds_epu8(dst0mm, src0mm);
    pix_store16a(dst, dst0mm);

    dst += 16;
    src += 12;
    msk += 4;
  BLIT_SSE2_LARGE_END(blt)
}

} // Raster namespace
} // Fog namespace
