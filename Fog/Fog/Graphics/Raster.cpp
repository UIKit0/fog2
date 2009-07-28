// [Fog/Graphics Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Precompiled headers]
#ifdef FOG_PRECOMP
#include FOG_PRECOMP
#endif

// [Dependencies]
#include <Fog/Core/CpuInfo.h>
#include <Fog/Graphics/Raster.h>

namespace Fog {
namespace Raster {

// ============================================================================
// [Fog::Raster - FunctionMap]
// ============================================================================

static FunctionMap functionMapData;

FunctionMap* functionMap;

// ============================================================================
// [Fog::Raster - getRasterOps]
// ============================================================================

FunctionMap::RasterFuncs* getRasterOps(int format, int op)
{
  FOG_ASSERT(format < Image::FormatCount);
  FOG_ASSERT(op < CompositeCount);

  return &functionMap->raster[format][op];
}

} // Raster namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_EXTERN void fog_raster_init_c(void);
FOG_INIT_EXTERN void fog_raster_init_mmx(void);
FOG_INIT_EXTERN void fog_raster_init_sse2(void);

FOG_INIT_EXTERN void fog_raster_init_jit(void);
FOG_INIT_EXTERN void fog_raster_shutdown_jit(void);

FOG_INIT_DECLARE err_t fog_raster_init(void)
{
  using namespace Fog;
  using namespace Fog::Raster;

  FunctionMap* m = functionMap = &functionMapData;

  // [Install generic C implementation]
  fog_raster_init_c();

  // [Install MMX optimized code if supported]

  if (Fog::cpuInfo->hasFeature(Fog::CpuInfo::Feature_MMX))
    fog_raster_init_mmx();

  // [Install SSE2 optimized code if supported]

  if (Fog::cpuInfo->hasFeature(Fog::CpuInfo::Feature_SSE2))
  {
    fog_raster_init_sse2();
  }

  return Error::Ok;
}

FOG_INIT_DECLARE void fog_raster_shutdown(void)
{
}
