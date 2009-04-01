#ifndef DMZ_RENDER_CONFIG_TO_OSG_DOT_H
#define DMZ_RENDER_CONFIG_TO_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzTypesBase.h>
#include <osg/Vec3>
#include <osg/Vec4>

namespace dmz {

   class Config;

   osg::Vec3 config_to_osg_vec3 (const Config &Source);
   osg::Vec3 config_to_osg_vec3 (const Config &Source, const osg::Vec3 &Value);
   osg::Vec3 config_to_osg_vec3 (const String &Name, const Config &Source);

   DMZ_RENDER_UTIL_OSG_LINK_SYMBOL osg::Vec3 config_to_osg_vec3 ( 
      const String &Name, 
      const Config &Source, 
      const osg::Vec3 &DefaultValue);

   osg::Vec4 config_to_osg_vec4_color (const Config &Source);
   osg::Vec4 config_to_osg_vec4_color (const Config &Source, const osg::Vec4 &Value);
   osg::Vec4 config_to_osg_vec4_color (const String &Name, const Config &Source);

   DMZ_RENDER_UTIL_OSG_LINK_SYMBOL osg::Vec4 config_to_osg_vec4_color ( 
      const String &Name, 
      const Config &Source, 
      const osg::Vec4 &DefaultValue);
};


inline osg::Vec3
dmz::config_to_osg_vec3 (
      const String &Name,
      const Config &Source) {

   const osg::Vec3 Value;
   return config_to_osg_vec3 (Name, Source, Value);
}


inline osg::Vec3
dmz::config_to_osg_vec3 (
      const Config &Source,
      const osg::Vec3 &Value) {

   return config_to_osg_vec3 ("", Source, Value);
}


inline osg::Vec3
dmz::config_to_osg_vec3 (
      const Config &Source) {

   const osg::Vec3 Value;
   return config_to_osg_vec3 ("", Source, Value);
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

