// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGRECTELEMENT_P_H
#define _FOG_SVG_DOM_SVGRECTELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

struct FOG_NO_EXPORT SvgRectElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRectElement();
  virtual ~SvgRectElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRenderShape(SvgRenderContext* context) const;
  virtual err_t onCalcBoundingBox(RectF* box) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgCoordAttribute a_rx;
  SvgCoordAttribute a_ry;

private:
  FOG_DISABLE_COPY(SvgRectElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGRECTELEMENT_P_H
