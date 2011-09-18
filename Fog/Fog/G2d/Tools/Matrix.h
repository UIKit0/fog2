// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_MATRIX_H
#define _FOG_G2D_TOOLS_MATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::MatrixDataF]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataF
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataF* addRef() const
  {
    reference.inc();
    return const_cast<MatrixDataF*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.matrixf.dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return size.w == 0;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t area)
  {
    return sizeof(MatrixDataF) - sizeof(float) + area * sizeof(float);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Matrix size.
  SizeI size;
  //! @brief Elements.
  float data[1];
};

// ============================================================================
// [Fog::MatrixDataD]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataD
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataD* addRef() const
  {
    reference.inc();
    return const_cast<MatrixDataD*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.matrixd.dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return size.w == 0;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t area)
  {
    return sizeof(MatrixDataD) - sizeof(double) + area * sizeof(double);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Size.
  SizeI size;
  //! @brief Elements.
  double data[1];
};

// ============================================================================
// [Fog::MatrixF]
// ============================================================================

//! @brief Matrix (float).
struct FOG_NO_EXPORT MatrixF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixF()
  {
    _api.matrixf.ctor(this);
  }

  FOG_NO_EXPORT MatrixF(const MatrixF& other)
  {
    _api.matrixf.ctorCopy(this, &other);
  }

  FOG_NO_EXPORT MatrixF(const SizeI& size, const float* data = NULL)
  {
    _api.matrixf.ctorCreate(this, &size, data);
  }

  explicit FOG_INLINE MatrixF(MatrixDataF* d)
    : _d(d)
  {
  }

  FOG_NO_EXPORT ~MatrixF()
  {
    _api.matrixf.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach() { return _api.matrixf.detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _d->size; }
  FOG_INLINE int getWidth() const { return _d->size.w; }
  FOG_INLINE int getHeight() const { return _d->size.h; }
  FOG_INLINE bool isEmpty() const { return _d->size.w == 0; }

  FOG_INLINE const float* getData() const
  {
    return _d->data;
  }

  FOG_INLINE float* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixF::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE const float* getRow(size_t index) const
  {
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixF::getRow() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  FOG_INLINE float* getRowX(size_t index)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixF::getRowX() - Not detached.");
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixF::getRowX() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const SizeI& size, const float* data = NULL)
  {
    return _api.matrixf.create(this, &size, data);
  }

  FOG_INLINE err_t resize(const SizeI& size, float value = 0.0f)
  {
    return _api.matrixf.resize(this, &size, value);
  }

  FOG_INLINE void reset()
  {
    _api.matrixf.reset(this);
  }

  FOG_INLINE float getCell(int x, int y) const
  {
    return _api.matrixf.getCell(this, x, y);
  }

  FOG_INLINE err_t setCell(int x, int y, float value)
  {
    return _api.matrixf.setCell(this, x, y, value);
  }

  FOG_INLINE err_t fill(const RectI& rect, float value)
  {
    return _api.matrixf.fill(this, &rect, value);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixF& operator=(const MatrixF& other)
  {
    _api.matrixf.copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const MatrixF& other) const { return  _api.matrixf.eq(this, &other); }
  FOG_INLINE bool operator!=(const MatrixF& other) const { return !_api.matrixf.eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const MatrixF* a, const MatrixF* b)
  {
    return _api.matrixf.eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.matrixf.eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE MatrixDataF* _dCreate(const SizeI& size, const float* data = NULL)
  {
    return _api.matrixf.dCreate(&size, data);
  }

  static FOG_INLINE void _dFree(MatrixDataF* d)
  {
    _api.matrixf.dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(MatrixDataF)
};

// ============================================================================
// [Fog::MatrixD]
// ============================================================================

//! @brief Matrix (float).
struct FOG_NO_EXPORT MatrixD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixD()
  {
    _api.matrixd.ctor(this);
  }

  FOG_NO_EXPORT MatrixD(const MatrixD& other)
  {
    _api.matrixd.ctorCopy(this, &other);
  }

  FOG_NO_EXPORT MatrixD(const SizeI& size, const double* data = NULL)
  {
    _api.matrixd.ctorCreate(this, &size, data);
  }

  explicit FOG_INLINE MatrixD(MatrixDataD* d)
    : _d(d)
  {
  }

  FOG_NO_EXPORT ~MatrixD()
  {
    _api.matrixd.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach() { return _api.matrixd.detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _d->size; }
  FOG_INLINE int getWidth() const { return _d->size.w; }
  FOG_INLINE int getHeight() const { return _d->size.h; }
  FOG_INLINE bool isEmpty() const { return _d->size.w == 0; }

  FOG_INLINE const double* getData() const
  {
    return _d->data;
  }

  FOG_INLINE double* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixD::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE const double* getRow(size_t index) const
  {
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixD::getRow() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  FOG_INLINE double* getRowX(size_t index)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixD::getRowX() - Not detached.");
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixD::getRowX() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const SizeI& size, const double* data = NULL)
  {
    return _api.matrixd.create(this, &size, data);
  }

  FOG_INLINE err_t resize(const SizeI& size, double value = 0.0f)
  {
    return _api.matrixd.resize(this, &size, value);
  }

  FOG_INLINE void reset()
  {
    _api.matrixd.reset(this);
  }

  FOG_INLINE double getCell(int x, int y) const
  {
    return _api.matrixd.getCell(this, x, y);
  }

  FOG_INLINE err_t setCell(int x, int y, double value)
  {
    return _api.matrixd.setCell(this, x, y, value);
  }

  FOG_INLINE err_t fill(const RectI& rect, double value)
  {
    return _api.matrixd.fill(this, &rect, value);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixD& operator=(const MatrixD& other)
  {
    _api.matrixd.copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const MatrixD& other) const { return  _api.matrixd.eq(this, &other); }
  FOG_INLINE bool operator!=(const MatrixD& other) const { return !_api.matrixd.eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const MatrixD* a, const MatrixD* b)
  {
    return _api.matrixd.eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.matrixd.eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE MatrixDataD* _dCreate(const SizeI& size, const double* data = NULL)
  {
    return _api.matrixd.dCreate(&size, data);
  }

  static FOG_INLINE void _dFree(MatrixDataD* d)
  {
    _api.matrixd.dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(MatrixDataD)
};

// ============================================================================
// [Fog::MatrixT<>]
// ============================================================================

_FOG_NUM_T(Matrix)
_FOG_NUM_T(MatrixData)
_FOG_NUM_F(Matrix)
_FOG_NUM_F(MatrixData)
_FOG_NUM_D(Matrix)
_FOG_NUM_D(MatrixData)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_MATRIX_H
