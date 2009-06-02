#include <dmzRenderConfigToOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

osg::Vec3
dmz::config_to_osg_vec3 (
      const String &Name,
      const Config &Source,
      const osg::Vec3 &DefaultValue) {
   
   Config cd;

   if (Name) {

      Source.lookup_config (Name, cd);
   }
   else { cd = Source; }

   const osg::Vec3 Result (
      config_to_float32 ("x", cd, DefaultValue.x ()),
      config_to_float32 ("y", cd, DefaultValue.y ()),
      config_to_float32 ("z", cd, DefaultValue.z ()));

   return Result;
}


osg::Vec4
dmz::config_to_osg_vec4_color (
      const String &Name,
      const Config &Source,
      const osg::Vec4 &DefaultValue) {
   
   Config cd;

   if (Name) {

      Source.lookup_config (Name, cd);
   }
   else { cd = Source; }

   const osg::Vec4 Result (
      config_to_float32 ("r", cd, DefaultValue.x ()),
      config_to_float32 ("g", cd, DefaultValue.y ()),
      config_to_float32 ("b", cd, DefaultValue.z ()),
      config_to_float32 ("a", cd, DefaultValue.w ()));

   return Result;
}

