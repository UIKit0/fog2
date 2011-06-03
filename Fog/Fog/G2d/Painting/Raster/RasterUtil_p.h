// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTER_RASTERUTIL_P_H
#define _FOG_G2D_PAINTING_RASTER_RASTERUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Render/RenderStructs_p.h>

namespace Fog {
namespace RasterUtil {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterUtil - Multi-Threading]
// ============================================================================

static FOG_INLINE int alignToDelta(int y, int offset, int delta)
{
  FOG_ASSERT(offset >= 0);
  FOG_ASSERT(delta >= 1);
  FOG_ASSERT(offset < delta);

  // At this time we can expect that Y coordinate is always positive. I added
  // the assert here so if this will fail we will know, but it should remain!
  // Raster just can't contain negative coordinate, its [0, 0, width, height].
  FOG_ASSERT(y >= 0);

  uint mody = (uint)y % (uint)delta;

  // Not needed, Y is always positive.
  // if (mody < 0) mody += delta;

  uint newy = (uint)y - mody + (uint)offset;
  if (newy < (uint)y) newy += delta;

  return (int)newy;
}

// ============================================================================
// [Fog::RasterUtil - Pattern]
// ============================================================================

static FOG_INLINE bool isSolidContext(RenderPatternContext* pc)
{
  return (size_t)pc == (size_t)0x1;
}

static FOG_INLINE bool isPatternContext(RenderPatternContext* pc)
{
  return (size_t)pc > (size_t)0x1;
}

// ============================================================================
// [Fog::RasterUtil - Painting]
// ============================================================================

static FOG_INLINE bool isCompositingOperatorNop(uint32_t op)
{
  return op == COMPOSITE_DST;
}

//! @brief Whether the source rectangle @a src can be converted to aligned
//! rectangle @a dst, used to call various fast-paths.
static FOG_INLINE bool canAlignToGrid(BoxI& dst, const RectD& src, double x, double y)
{
  int rx = Math::iround((src.x + x) * 256.0);
  if ((rx & 0xFF) != 0x00) return false;

  int ry = Math::iround((src.y + y) * 256.0);
  if ((ry & 0xFF) != 0x00) return false;

  int rw = Math::iround((src.w) * 256.0);
  if ((rw & 0xFF) != 0x00) return false;

  int rh = Math::iround((src.h) * 256.0);
  if ((rh & 0xFF) != 0x00) return false;

  dst.x0 = (int)(rx >> 8);
  dst.y0 = (int)(ry >> 8);
  dst.x1 = dst.x0 + (int)(rw >> 8);
  dst.y1 = dst.y0 + (int)(rh >> 8);

  return true;
}

#define RASTER_ENTER_PAINT_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      if (ctx.state & (RASTER_CONTEXT_NO_PAINT_MASK)) return ERR_OK; \
      if (ctx.state & (RASTER_CONTEXT_PENDING_MASK)) _postPending(); \
    } \
  FOG_MACRO_END

#define RASTER_ENTER_PAINT_COND(__condition__) \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(!(__condition__))) return ERR_OK; \
    \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      if (ctx.state & (RASTER_CONTEXT_NO_PAINT_MASK)) return ERR_OK; \
      if (ctx.state & (RASTER_CONTEXT_PENDING_MASK)) _postPending(); \
    } \
  FOG_MACRO_END

#define RASTER_ENTER_PAINT_IMAGE(__image__) \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(__image__.isEmpty())) return ERR_OK; \
    \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      /* Image is source so we need to exclude RASTER_CONTEXT_NO_PAINT_SOURCE flag. */ \
      if (ctx.state & (RASTER_CONTEXT_NO_PAINT_MASK & ~(RASTER_CONTEXT_NO_PAINT_SOURCE))) return ERR_OK; \
      if (ctx.state & (RASTER_CONTEXT_PENDING_MASK)) _postPending(); \
    } \
  FOG_MACRO_END

// Called by serializers to ensure that pattern context is created if needed.
#define RASTER_SERIALIZE_ENSURE_PATTERN() \
  FOG_MACRO_BEGIN \
    if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN && FOG_UNLIKELY(_getRasterPatternContext() == NULL)) \
      return ERR_RT_OUT_OF_MEMORY; \
  FOG_MACRO_END

// ============================================================================
// [Fog::RasterUtil - Clipping]
// ============================================================================

// Get usable span instance from span retrieved from mask. We encode 'owned'
// bit into pointer itself so it's needed to clear it to access the instance.
#define RASTER_CLIP_SPAN_GET_USABLE(__span__) \
  ( (Span8*) ((size_t)(__span__) & ~1) )

// Get whether a given span instance is 'owned' or not. Owned means that it
// can be directly manipulated by clipping method. If span is not owned then
// you must replace the entire clip row with new span list.
#define RASTER_CLIP_SPAN_IS_OWNED(__span__) \
  (((size_t)(__span__) & 0x1) == 0)

// Whether a given span instance is VSpan that contains own embedded clip mask.
#define RASTER_CLIP_IS_EMBEDDED_VSPAN(__span__) \
  (reinterpret_cast<const uint8_t*>(__span__) + sizeof(RasterSpan8) == __span__->getGenericMask())

#define RASTER_ENTER_CLIP_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(clipOp >= CLIP_OP_COUNT)) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_CONTEXT_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
  FOG_MACRO_END

#define RASTER_ENTER_CLIP_COND(__condition__) \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(clipOp >= CLIP_OP_COUNT)) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_CONTEXT_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
    \
    if (FOG_UNLIKELY(!(__condition__))) \
      return _clipOpNull(clipOp); \
    \
  FOG_MACRO_END

//! @brief Get clip type from source region (src).
static FOG_INLINE uint32_t Raster_getClipType(const Region& src)
{
  size_t len = src.getLength();
  if (len > 2) len = 2;
  return (uint32_t)len;
}

// TODO: Verify and use.
//
// Binary search region (YX sorted rectangles) and match the first one that
// contains a given 'y'. If there is no such region then the next one will be
// returned
static FOG_INLINE const BoxI* Raster_bsearchRegion(const BoxI* base, size_t length, int y)
{
  FOG_ASSERT(base != NULL);
  FOG_ASSERT(length > 0);

  const BoxI* r;
  size_t i;

  for (i = length; i != 0; i >>= 1)
  {
    r = base + (i >> 1);

    // Try match.
    if (y >= r->y0)
    {
      if (y < r->y1) return r;
      // else: Move left.
    }
    else if (r->y1 <= y)
    {
      // Move right.
      base = r + 1;
      i--;
    }
    // else: Move left.
  }
  return r;
}

//! @}

} // RasterUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTER_RASTERUTIL_P_H
