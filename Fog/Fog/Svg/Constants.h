// [Fog/Svg Library - C++ API]
//
// [Licence]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_CONSTANTS_H
#define _FOG_SVG_CONSTANTS_H

// [Dependencies]
#include <Fog/Build/Build.h>

//! @addtogroup Fog_Svg
//! @{

namespace Fog {

// ============================================================================
// [Fog::SVG_ELEMENT_TYPE]
// ============================================================================

enum SVG_ELEMENT_TYPE
{
  SVG_ELEMENT_NONE,

  SVG_ELEMENT_CIRCLE,
  SVG_ELEMENT_DEFS,
  SVG_ELEMENT_ELLIPSE,
  SVG_ELEMENT_G,
  SVG_ELEMENT_IMAGE,
  SVG_ELEMENT_LINE,
  SVG_ELEMENT_LINEAR_GRADIENT,
  SVG_ELEMENT_PATH,
  SVG_ELEMENT_POLYGON,
  SVG_ELEMENT_POLYLINE,
  SVG_ELEMENT_RADIAL_GRADIENT,
  SVG_ELEMENT_RECT,
  SVG_ELEMENT_SOLID_COLOR,
  SVG_ELEMENT_STOP
};

// ============================================================================
// [Fog::SVG_UNIT_TYPE]
// ============================================================================

enum SVG_UNIT_TYPE
{
  SVG_UNIT_NONE = 0,
  SVG_UNIT_CM,
  SVG_UNIT_EM,
  SVG_UNIT_EX,
  SVG_UNIT_IN,
  SVG_UNIT_MM,
  SVG_UNIT_PC,
  SVG_UNIT_PT,
  SVG_UNIT_PX,
  SVG_UNIT_PERCENT,

  SVG_UNIT_INVALID
};

// ============================================================================
// [Fog::SVG_PAINT_TYPE]
// ============================================================================

enum SVG_PAINT_TYPE
{
  SVG_PAINT_FILL = 0,
  SVG_PAINT_STROKE = 1
};

// ============================================================================
// [Fog::SVG_STYLE_TYPE]
// ============================================================================

enum SVG_STYLE_TYPE
{
  // When adding value here, it's important to add value to Fog/Core/Strings.h
  // and Fog/Core/Strings.cpp. There are static strings for style names.
  SVG_STYLE_NONE = 0,

  SVG_STYLE_CLIP_PATH,
  SVG_STYLE_CLIP_RULE,
  SVG_STYLE_ENABLE_BACKGROUND,
  SVG_STYLE_FILL,
  SVG_STYLE_FILL_OPACITY,
  SVG_STYLE_FILL_RULE,
  SVG_STYLE_FILTER,
  SVG_STYLE_FONT_FAMILY,
  SVG_STYLE_FONT_SIZE,
  SVG_STYLE_LETTER_SPACING,
  SVG_STYLE_MASK,
  SVG_STYLE_OPACITY,
  SVG_STYLE_STOP_COLOR,
  SVG_STYLE_STOP_OPACITY,
  SVG_STYLE_STROKE,
  SVG_STYLE_STROKE_DASH_ARRAY,
  SVG_STYLE_STROKE_DASH_OFFSET,
  SVG_STYLE_STROKE_LINE_CAP,
  SVG_STYLE_STROKE_LINE_JOIN,
  SVG_STYLE_STROKE_MITER_LIMIT,
  SVG_STYLE_STROKE_OPACITY,
  SVG_STYLE_STROKE_WIDTH,

  SVG_STYLE_INVALID
};

// ============================================================================
// [Fog::SVG_VALUE_TYPE]
// ============================================================================

//! @brief Svg value types.
enum SVG_VALUE_TYPE
{
  //! @brief Value is nothing special (not recognized style).
  SVG_VALUE_NONE = 0,

  //! @brief Value is enumeration. To get it use @c getValue() or @c getValueAsUInt32().
  SVG_VALUE_ENUM,
  //! @brief Value is color. To get it use @c getValue() or @c getValueAsUInt32().
  SVG_VALUE_COLOR,
  //! @brief Value is color or url (for gradients).
  SVG_VALUE_COLOR_OR_PATTERN,
  //! @brief Value is string. To get it use @c getValue() or @c getValueAsDouble()
  //! and @c unitType().
  SVG_VALUE_COORD,
  //! @brief Value is opacity. To get it use @c getValue() or @c getValueAsDouble().
  SVG_VALUE_OPACITY,
  //! @brief Value is string. To get it use @c getValue().
  SVG_VALUE_STRING,

  SVG_VALUE_INVALID
};

// ============================================================================
// [Fog::SVG_PATTERN_TYPE]
// ============================================================================

enum SVG_PATTERN_TYPE
{
  SVG_PATTERN_NONE = 0,
  SVG_PATTERN_COLOR = 1,
  SVG_PATTERN_URI = 2,

  SVG_PATTERN_INVALID
};

// ============================================================================
// [Fog::SVG_PATTERN_UNITS]
// ============================================================================

enum SVG_PATTERN_UNITS
{
  SVG_OBJECT_BOUNDING_BOX = 0,
  SVG_USER_SPACE_ON_USE = 1
};

// ============================================================================
// [Fog::ERR_SVG]
// ============================================================================

//! @brief Error codes used in Fog/Svg.
enum ERR_SVG_ENUM
{
  // Errors Range.
  ERR_SVG_START = 0x00011100,
  ERR_SVG_LAST  = 0x000111FF,

  // Svg Errors.

  //! @brief Error set if svg embedded data are not in base64 format.
  ERR_SVG_UNSUPPORTED_DATA_ENCODING = ERR_SVG_START
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_SVG_CONSTANTS_H