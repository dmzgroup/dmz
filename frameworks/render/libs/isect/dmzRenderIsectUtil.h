#ifndef DMZ_RENDER_ISECT_UTIL_DOT_H
#define DMZ_RENDER_ISECT_UTIL_DOT_H

#include <dmzRenderIsectExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class IsectResultContainer;
   class RenderModuleIsect;
   class Vector;

  DMZ_RENDER_ISECT_LINK_SYMBOL Boolean
  isect_validate_point (
      const Vector &Start,
      const Vector &Dir,
      const IsectResultContainer &IsectResults,
      Vector &point,
      Vector &normal);

   DMZ_RENDER_ISECT_LINK_SYMBOL Boolean
   isect_clamp_point (
      const Vector &Value,
      RenderModuleIsect &isect,
      Vector &point,
      Vector &normal);
};

#endif // DMZ_RENDER_ISECT_UTIL_DOT_H
