// [Fog/Graphics Library - Public API]
//
// [Licence]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

// [Guard]
#ifndef _FOG_GRAPHICS_PATH_H
#define _FOG_GRAPHICS_PATH_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Matrix;

// ============================================================================
// [Fog::PathData]
// ============================================================================

struct FOG_HIDDEN PathData
{
  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE PathData* ref() const
  {
    refCount.inc();
    return const_cast<PathData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Fog::Atomic<sysuint_t> refCount;
  //! @brief Whether the path is flattened (no curves).
  int flat;
  //! @brief Path capacity (allocated space for vertices).
  sysuint_t capacity;
  //! @brief Path length (count of vertices used).
  sysuint_t length;

  //! @brief Vertices data (aligned to 16 bytes).
  PointD* vertices;
  //! @brief Commands data (space for aligning).
  uint8_t commands[16];
};

// ============================================================================
// [Fog::PathCmd]
// ============================================================================

struct FOG_HIDDEN PathCmd
{
  static FOG_INLINE bool isStop(uint8_t cmd) { return (cmd & PATH_CMD_VERTEX_MASK) == 0; }
  static FOG_INLINE bool isVertex(uint8_t cmd) { return (cmd & PATH_CMD_VERTEX_MASK) != 0; }
  static FOG_INLINE bool isDrawing(uint8_t cmd) { return (cmd & PATH_CMD_VERTEX_MASK) > PATH_CMD_MOVE_TO; }

  static FOG_INLINE bool isMoveTo(uint8_t cmd) { return (cmd == PATH_CMD_MOVE_TO); }
  static FOG_INLINE bool isLineTo(uint8_t cmd) { return (cmd == PATH_CMD_LINE_TO); }

  static FOG_INLINE bool isCurve(uint8_t cmd) { return (cmd & PATH_CMD_CURVE_MASK) != 0; }
  static FOG_INLINE bool isCurve3(uint8_t cmd) { return (cmd == PATH_CMD_CURVE_3); }
  static FOG_INLINE bool isCurve4(uint8_t cmd) { return (cmd == PATH_CMD_CURVE_4); }

  static FOG_INLINE bool isClose(uint8_t cmd) { return (cmd & ~(PATH_CMD_FLAG_CW | PATH_CMD_FLAG_CCW)) == (PATH_CMD_END | PATH_CMD_FLAG_CLOSE); }
  static FOG_INLINE bool isClosed(uint8_t cmd) { return (cmd & PATH_CMD_FLAG_CLOSE) != 0; }

  static FOG_INLINE bool isEndPoly(uint8_t cmd) { return (cmd & PATH_CMD_TYPE_MASK) == PATH_CMD_END; }

  static FOG_INLINE bool isOriented(uint8_t cmd) { return (cmd & (PATH_CMD_FLAG_CW | PATH_CMD_FLAG_CCW)) != 0; }
  static FOG_INLINE bool isCW(uint8_t cmd) { return (cmd & PATH_CMD_FLAG_CW) != 0; }
  static FOG_INLINE bool isCCW(uint8_t cmd) { return (cmd & PATH_CMD_FLAG_CCW) != 0; }
};

// ============================================================================
// [Fog::Path]
// ============================================================================

//! @brief Path defines graphics path that can be filled or stroked by painter.
struct FOG_API Path
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Path();
  Path(const Path& other);
  FOG_INLINE explicit Path(PathData* d) : _d(d) {}
  ~Path();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  static Static<PathData> sharedNull;

  static PathData* _allocData(sysuint_t capacity);
  static PathData* _reallocData(PathData* d, sysuint_t capacity);
  static PathData* _copyData(const PathData* d);

  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  FOG_INLINE bool isDetached() const { return _d->refCount.get() == 1; }

  FOG_INLINE err_t detach() { return !isDetached() ? _detach() : ERR_OK; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get path capacity (count of allocated vertices).
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  //! @brief Get path length (count of vertices used).
  FOG_INLINE sysuint_t getLength() const { return _d->length; }

  //! @brief Get whether path is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }
  //! @brief Get whether path is null.
  FOG_INLINE bool isNull() const { return _d == sharedNull.instancep(); }

  //! @brief Get path commands array.
  FOG_INLINE const uint8_t* getCommands() const { return _d->commands; }
  //! @brief Get path vertices array.
  FOG_INLINE const PointD* getVertices() const { return _d->vertices; }

  FOG_INLINE uint8_t* getMCommands()
  {
    return detach() == ERR_OK ? _d->commands : NULL;
  }

  FOG_INLINE PointD* getMVertices()
  {
    return detach() == ERR_OK ? _d->vertices : NULL;
  }

  FOG_INLINE uint8_t* getXCommands()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Path::getXCommands() - Non-detached data.");
    return _d->commands;
  }

  FOG_INLINE PointD* getXVertices()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Path::getXVertices() - Non-detached data.");
    return _d->vertices;
  }

  err_t reserve(sysuint_t capacity);
  void squeeze();

  sysuint_t _add(sysuint_t count);
  err_t _detach();

  err_t set(const Path& other);
  err_t setDeep(const Path& other);

  void clear();
  void free();

  // --------------------------------------------------------------------------
  // [Bounding Rect]
  // --------------------------------------------------------------------------

  RectD getBoundingRect() const;

  // --------------------------------------------------------------------------
  // [SubPath]
  // --------------------------------------------------------------------------

  //! @brief Get count of vertices in @a subPathId.
  //!
  //! @note Sub path id is simply index of vertex where path starts.
  sysuint_t getSubPathLength(sysuint_t subPathId) const;

  // --------------------------------------------------------------------------
  // [Start / End]
  // --------------------------------------------------------------------------

  err_t start(sysuint_t* index);

  err_t endPoly(uint32_t cmdflags = PATH_CMD_FLAG_CLOSE);
  err_t closePolygon(uint32_t cmdflags = PATH_CMD_FLAG_NONE);

  // --------------------------------------------------------------------------
  // [MoveTo]
  // --------------------------------------------------------------------------

  err_t moveTo(double x, double y);
  err_t moveRel(double dx, double dy);

  FOG_INLINE err_t moveTo(const PointD& pt) { return moveTo(pt.x, pt.y); }
  FOG_INLINE err_t moveRel(const PointD& pt) { return moveRel(pt.x, pt.y); }

  // --------------------------------------------------------------------------
  // [LineTo]
  // --------------------------------------------------------------------------

  err_t lineTo(double x, double y);
  err_t lineRel(double dx, double dy);

  FOG_INLINE err_t lineTo(const PointD& pt) { return lineTo(pt.x, pt.y); }
  FOG_INLINE err_t lineRel(const PointD& pt) { return lineRel(pt.x, pt.y); }

  err_t lineTo(const double* x, const double* y, sysuint_t count);
  err_t lineTo(const PointD* pts, sysuint_t count);

  err_t hlineTo(double x);
  err_t hlineRel(double dx);

  err_t vlineTo(double y);
  err_t vlineRel(double dy);

  // --------------------------------------------------------------------------
  // [ArcTo]
  // --------------------------------------------------------------------------

  err_t _arcTo(double cx, double cy, double rx, double ry, double start, double sweep, uint8_t initialCommand, bool closePath);
  err_t _svgArcTo(
    double rx, double ry,
    double angle,
    bool largeArcFlag,
    bool sweepFlag,
    double x2, double y2,
    uint8_t initialCommand, bool closePath);

  err_t arcTo(double cx, double cy, double rx, double ry, double start, double sweep);
  err_t arcRel(double cx, double cy, double rx, double ry, double start, double sweep);

  FOG_INLINE err_t arcTo(const PointD& cp, const PointD& r, double start, double sweep)
  { return arcTo(cp.x, cp.y, r.x, r.y, start, sweep); }

  FOG_INLINE err_t arcRel(const PointD& cp, const PointD& r, double start, double sweep)
  { return arcRel(cp.x, cp.y, r.x, r.y, start, sweep); }

  err_t svgArcTo(
    double rx, double ry,
    double angle,
    bool largeArcFlag,
    bool sweepFlag,
    double x2, double y2);

  err_t svgArcRel(
    double rx, double ry,
    double angle,
    bool largeArcFlag,
    bool sweepFlag,
    double x2, double y2);

  // --------------------------------------------------------------------------
  // [CurveTo]
  // --------------------------------------------------------------------------

  err_t curveTo(double cx, double cy, double tx, double ty);
  err_t curveRel(double cx, double cy, double tx, double ty);

  err_t curveTo(double tx, double ty);
  err_t curveRel(double tx, double ty);

  FOG_INLINE err_t curveTo(const PointD& cp, const PointD& to)
  { return curveTo(cp.x, cp.y, to.x, to.y); }

  FOG_INLINE err_t curveRel(const PointD& cp, const PointD& to)
  { return curveRel(cp.x, cp.y, to.x, to.y); }

  FOG_INLINE err_t curveTo(const PointD& to)
  { return curveTo(to.x, to.y); }

  FOG_INLINE err_t curveRel(const PointD& to)
  { return curveRel(to.x, to.y); }

  // --------------------------------------------------------------------------
  // [CubicTo]
  // --------------------------------------------------------------------------

  err_t cubicTo(double cx1, double cy1, double cx2, double cy2, double tx, double ty);
  err_t cubicRel(double cx1, double cy1, double cx2, double cy2, double tx, double ty);

  err_t cubicTo(double cx2, double cy2, double tx, double ty);
  err_t cubicRel(double cx2, double cy2, double tx, double ty);

  FOG_INLINE err_t cubicTo(const PointD& cp1, const PointD& cp2, const PointD& to)
  { return cubicTo(cp1.x, cp1.y, cp2.x, cp2.y, to.x, to.y); }

  FOG_INLINE err_t cubicRel(const PointD& cp1, const PointD& cp2, const PointD& to)
  { return cubicRel(cp1.x, cp1.y, cp2.x, cp2.y, to.x, to.y); }

  FOG_INLINE err_t cubicTo(const PointD& cp2, const PointD& to)
  { return cubicTo(cp2.x, cp2.y, to.x, to.y); }

  FOG_INLINE err_t cubicRel(const PointD& cp2, const PointD& to)
  { return cubicRel(cp2.x, cp2.y, to.x, to.y); }

  // --------------------------------------------------------------------------
  // [FlipX / FlipY]
  // --------------------------------------------------------------------------

  err_t flipX(double x1, double x2);
  err_t flipY(double y1, double y2);

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  //! @brief Translate each vertex in path by @a dx and @a dy.
  err_t translate(double dx, double dy);
  //! @brief Translate each vertex in sub-path @a subPathId by @a dx and @a dy.
  err_t translateSubPath(sysuint_t subPathId, double dx, double dy);

  //! @brief Translate each vertex in path by @a pt.
  FOG_INLINE err_t translate(const PointD& pt)
  { return translate(pt.x, pt.y); }

  //! @brief Translate each vertex in sub-path @a subPathId by @a pt.
  FOG_INLINE err_t translateSubPath(sysuint_t subPathId, const PointD& pt)
  { return translateSubPath(subPathId, pt.x, pt.y); }

  // --------------------------------------------------------------------------
  // [Scale]
  // --------------------------------------------------------------------------

  //! @brief Scale each vertex in path by @a sx and @a sy.
  err_t scale(double sx, double sy, bool keepStartPos = false);

  //! @brief Scale each vertex in path by @a pt.
  FOG_INLINE err_t scale(const PointD& pt, bool keepStartPos = false)
  { return scale(pt.x, pt.y, keepStartPos); }

  // --------------------------------------------------------------------------
  // [ApplyMatrix]
  // --------------------------------------------------------------------------

  //! @brief Apply affine matrix transformations to each vertex in path.
  err_t applyMatrix(const Matrix& matrix);
  //! @brief Apply affine matrix transformations to vertex that starts with
  //! @a index and range is given by @a range argument.
  err_t applyMatrix(const Matrix& matrix, const Range& range);

  // --------------------------------------------------------------------------
  // [Complex]
  // --------------------------------------------------------------------------

  //! @brief Add closed rectangle into path.
  err_t addRect(const RectD& r, int direction = PATH_DIRECTION_CW);

  //! @brief Add rectangles.
  err_t addRects(const RectD* r, sysuint_t count, int direction = PATH_DIRECTION_CW);

  //! @brief Add round.
  err_t addRound(const RectD& r, const PointD& radius, int direction = PATH_DIRECTION_CW);

  //! @brief Add Closed ellipse into path.
  err_t addEllipse(const RectD& r, int direction = PATH_DIRECTION_CW);
  //! @overload
  err_t addEllipse(const PointD& cp, const PointD& r, int direction = PATH_DIRECTION_CW);

  //! @brief Add arc into path.
  err_t addArc(const RectD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);
  //! @overload
  err_t addArc(const PointD& cp, const PointD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);

  //! @brief Add chord into path.
  err_t addChord(const RectD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);
  //! @overload
  err_t addChord(const PointD& cp, const PointD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);

  //! @brief Add pie into path.
  err_t addPie(const RectD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);
  //! @overload
  err_t addPie(const PointD& cp, const PointD& r, double start, double sweep, int direction = PATH_DIRECTION_CW);

  //! @brief Add path.
  err_t addPath(const Path& path);
  //! @brief Add translated path.
  err_t addPath(const Path& path, const PointD& pt);
  //! @brief Add transformed path.
  err_t addPath(const Path& path, const Matrix& matrix);

  // --------------------------------------------------------------------------
  // [Flatness]
  // --------------------------------------------------------------------------

  //! @brief Get whether path is flat.
  //!
  //! Path is flat only if it contains only lines (no curves). To make path flat
  //! use @c flatten() or @c flattenTo() methods.
  bool isFlat() const;

  //! @brief Invalidate type of path.
  //!
  //! This method must be called after path was manually changed by getMData() or
  //! similar methods to invalidate type of path.
  FOG_INLINE void resetFlat() const { _d->flat = -1; }

  err_t flatten();
  err_t flatten(const Matrix* matrix, double approximationScale = 1.0);

  err_t flattenTo(Path& dst, const Matrix* matrix, double approximationScale = 1.0) const;
  err_t flattenSubPathTo(Path& dst, sysuint_t subPathId, const Matrix* matrix, double approximationScale = 1.0) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Path& operator=(const Path& other)
  {
    set(other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(PathData)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PATH_H
