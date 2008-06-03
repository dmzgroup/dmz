#ifndef DMZ_RENDER_MODULE_ISECT_DOT_H
#define DMZ_RENDER_MODULE_ISECT_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzRenderIsect.h>

#define DMZ_RENDER_MODULE_ISECT_INTERFACE_NAME "RenderModuleIsectInterface"

namespace dmz {

   class IsectParameters;
   class IsectResult;
   class IsectTestContainer;
   class Isect;
   class Vector;

   class RenderModuleIsect {

      public:
         static RenderModuleIsect *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean do_isect (
            const IsectParameters &Parameters,
            const IsectTestContainer &TestValues,
            IsectResultContainer &result) = 0;

         virtual UInt32 enable_isect (const Handle ObjectHandle) = 0;
         virtual UInt32 disable_isect (const Handle ObjectHandle) = 0;

      protected:
         RenderModuleIsect (const PluginInfo &Info);
         ~RenderModuleIsect ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::RenderModuleIsect *
dmz::RenderModuleIsect::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModuleIsect *)lookup_rtti_interface (
      DMZ_RENDER_MODULE_ISECT_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModuleIsect::RenderModuleIsect (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_RENDER_MODULE_ISECT_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::RenderModuleIsect::~RenderModuleIsect () {

   remove_rtti_interface (DMZ_RENDER_MODULE_ISECT_INTERFACE_NAME, __Info);
}

#endif //  DMZ_RENDER_MODULE_ISECT_DOT_H
