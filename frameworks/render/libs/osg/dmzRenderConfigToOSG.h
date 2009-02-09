#ifndef DMZ_RENDER_CONFIG_TO_OSG_DOT_H
#define DMZ_RENDER_CONFIG_TO_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzTypesBase.h>
#include <osg/Vec4>

namespace dmz {

   class Config;

   osg::Vec4 config_to_osg_vec4_color (const Config &Source);
   osg::Vec4 config_to_osg_vec4_color (const Config &Source, const osg::Vec4 &Value);
   osg::Vec4 config_to_osg_vec4_color (const String &Name, const Config &Source);
   DMZ_RENDER_UTIL_OSG_LINK_SYMBOL osg::Vec4 config_to_osg_vec4_color ( 
      const String &Name, 
      const Config &Source, 
      const osg::Vec4 &DefaultValue);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const String &Name,
      const Config &Source) {

   const osg::Vec4 Value;
   return config_to_osg_vec4_color (Name, Source, Value);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const Config &Source,
      const osg::Vec4 &Value) {

   return config_to_osg_vec4_color ("", Source, Value);
}

inline osg::Vec4
dmz::config_to_osg_vec4_color (
      const Config &Source) {

   const osg::Vec4 Value;
   return config_to_osg_vec4_color ("", Source, Value);
}

#endif // DMZ_RENDER_CONFIG_TO_OSG_DOT_H

