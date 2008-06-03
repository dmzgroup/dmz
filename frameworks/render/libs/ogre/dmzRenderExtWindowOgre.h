#ifndef DMZ_RENDER_EXT_WINDOW_OGRE_DOT_H
#define DMZ_RENDER_EXT_WINDOW_OGRE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>

#define DMZ_RENDER_EXT_WINDOW_OGRE_INTERFACE_NAME "RenderExtWindowOgreInterface"

namespace Ogre { class RenderWindow; }

namespace dmz {

   class RenderExtWindowOgre {

      public:
         static RenderExtWindowOgre *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean init_render_window () = 0;
         virtual String get_render_window_name () const = 0;

      protected:
         RenderExtWindowOgre (const PluginInfo &Info);
         ~RenderExtWindowOgre ();

         const PluginInfo &_Info;
         
      private:
         RenderExtWindowOgre ();
         RenderExtWindowOgre (const RenderExtWindowOgre &);
         RenderExtWindowOgre &operator= (const RenderExtWindowOgre &);
   };
};


inline dmz::RenderExtWindowOgre *
dmz::RenderExtWindowOgre::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderExtWindowOgre *)lookup_rtti_interface (
      DMZ_RENDER_EXT_WINDOW_OGRE_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderExtWindowOgre::RenderExtWindowOgre (const PluginInfo &Info) :
      _Info (Info) {

   store_rtti_interface (
      DMZ_RENDER_EXT_WINDOW_OGRE_INTERFACE_NAME,
      _Info,
      (void *)this);
}


inline
dmz::RenderExtWindowOgre::~RenderExtWindowOgre () {

   remove_rtti_interface (DMZ_RENDER_EXT_WINDOW_OGRE_INTERFACE_NAME, _Info);
}


#endif //  DMZ_RENDER_EXT_WINDOW_OGRE_DOT_H
