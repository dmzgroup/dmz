#ifndef DMZ_RENDER_PICK_2D_UTIL_DOT_H
#define DMZ_RENDER_PICK_2D_UTIL_DOT_H

#include <dmzRenderUtilExport.h>
#include <dmzRenderPick.h>


namespace dmz {


   class DMZ_RENDER_UTIL_LINK_SYMBOL RenderPickUtil : public RenderPick {

      public:
         RenderModulePick *get_render_module_pick ();

         // RenderPick Interface.
         virtual void store_render_module_pick (
            const String &Name,
            RenderModulePick &module);

         virtual void remove_render_module_pick (
            const String &Name,
            RenderModulePick &module);

         virtual Boolean screen_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Handle &objectHandle);

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
            Handle &objectHandle);

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &sourcePosX,
            Int32 &sourcePosY);

      protected:
         RenderPickUtil (const PluginInfo &Info, const Config &Init);
         ~RenderPickUtil ();

         virtual void _store_render_module_pick (RenderModulePick &pickMod) {;}
         virtual void _remove_render_module_pick (RenderModulePick &pickMod) {;}

      private:
         struct State;
         RenderPickUtil ();
         RenderPickUtil (const RenderPickUtil &);
         RenderPickUtil &operator= (const RenderPickUtil &);
         State &__state;
   };
};


#endif // DMZ_RENDER_PICK_2D_UTIL_DOT_H

