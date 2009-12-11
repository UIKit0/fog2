// [Fog/Graphics library - C++ API]
//
// [Licence]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_SCANLINE_H
#define _FOG_GRAPHICS_SCANLINE_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

// ============================================================================
// [Fog::Scanline32]
// ============================================================================

struct FOG_API Scanline32
{
  //! @brief Type of coordinate in this scanline.
  typedef int32_t Coord;

  //! @brief Span.
  struct Span
  {
    //! @brief X position.
    Coord x;
    //! @brief Length, if negative, it's a solid span, covers is valid.
    Coord len;
    //! @brief Covers.
    uint8_t* covers;
  };

  enum { COORD_INIT = 0x7FFFFFF0 };

  Scanline32();
  ~Scanline32();

  FOG_INLINE int getY() const { return _y; }

  FOG_INLINE uint32_t getCoversCapacity() const { return _coversCapacity; }
  FOG_INLINE uint32_t getSpansCapacity() const { return _spansCapacity; }
  FOG_INLINE uint32_t getSpansCount() const { return _spansCount; }

  FOG_INLINE const uint8_t* getCoversData() const { return _coversData; }
  FOG_INLINE const Span* getSpansData() const { return _spansData; }

  err_t init(int x1, int x2);
  void reset();
  bool grow();

  FOG_INLINE uint finalize(int y)
  {
    _spansCount = (uint32_t)(_spansCur - _spansData);
    if (_spansCount) _y = y;
    return _spansCount;
  }

  FOG_INLINE void addCell(int x, uint cover)
  {
    FOG_ASSERT(_coversData != NULL);

    *_coversCur = (uint8_t)cover;

    if (x == _xEnd && _spansCur[-1].len > 0)
    {
      _spansCur[-1].len++;
    }
    else
    {
      if (_spansCur == _spansEnd && !grow()) return;
      _spansCur[0].x = Coord(x);
      _spansCur[0].len = 1;
      _spansCur[0].covers = _coversCur;
      _spansCur++;
    }

    _xEnd = x + 1;
    _coversCur++;
  }

  FOG_INLINE void addCells(int x, uint len, const uint8_t* covers)
  {
    FOG_ASSERT(_coversData != NULL);

    memcpy(_coversCur, covers, len * sizeof(uint8_t));

    if (x == _xEnd && _spansCur[-1].len > 0)
    {
      _spansCur[-1].len += Coord(len);
    }
    else
    {
      if (_spansCur == _spansEnd && !grow()) return;
      _spansCur[0].x = Coord(x);
      _spansCur[0].len = len;
      _spansCur[0].covers = _coversCur;
      _spansCur++;
    }

    _coversCur += len;
    _xEnd = x + len;
  }

  FOG_INLINE void addSpan(int x, uint len, uint cover)
  {
    FOG_ASSERT(_coversData != NULL);

    if (x == _xEnd && _spansCur[-1].len < 0 && _spansCur[-1].covers[0] == (uint8_t)cover)
    {
      _spansCur[-1].len -= (int)len;
    }
    else
    {
      *_coversCur = (uint8_t)cover;
      if (_spansCur == _spansEnd && !grow()) return;
      _spansCur[0].x = Coord(x);
      _spansCur[0].len = -int(len);
      _spansCur[0].covers = _coversCur++;
      _spansCur++;
    }

    _xEnd = x + len;
  }

  //! @brief End of X (first not valid pixel).
  int _xEnd;
  //! @brief Scanline Y coordinate, set by @c finalize().
  int _y;

  //! @brief Capacity of _coversData array.
  uint32_t _coversCapacity;
  //! @brief Capacity of _coversData array.
  uint32_t _spansCapacity;
  //! @brief Count of spans in _spansData array.
  uint32_t _spansCount;

  //! @brief Covers data.
  uint8_t* _coversData;
  //! @brief Current cover position.
  uint8_t* _coversCur;

  //! @brief Spans data.
  Span* _spansData;
  //! @brief Current span, span[-1] is always valid.
  Span* _spansCur;
  //! @brief End of span data (this points to invalid span).
  Span* _spansEnd;

private:
  FOG_DISABLE_COPY(Scanline32)
};

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_SCANLINE_H
