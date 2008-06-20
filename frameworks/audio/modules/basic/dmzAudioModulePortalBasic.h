#ifndef DMZ_AUDIO_MODULE_PORTAL_BASIC_DOT_H
#define DMZ_AUDIO_MODULE_PORTAL_BASIC_DOT_H

#include <dmzAudioModulePortal.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>


namespace dmz {


   class AudioModule;

   class AudioModulePortalBasic : public Plugin, private AudioModulePortal {

      public:
         //! \cond
         AudioModulePortalBasic (const PluginInfo &Info, const Config &Local);
         ~AudioModulePortalBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // AudioModulePortal Interface
         virtual Boolean is_master_portal () const;

         virtual void set_view (const Vector &Pos, const Matrix &Ori);
         virtual void get_view (Vector &pos, Matrix &ori) const;

         virtual void set_mute_state (const Boolean Value);
         virtual Boolean get_mute_state () const;

      protected:

         void _init (const Config &Local);

         Handle _handle;
         AudioModule *_audioModule;
         String _name;
         //! \endcond

      private:
         AudioModulePortalBasic (const AudioModulePortalBasic &);
         AudioModulePortalBasic &operator= (const AudioModulePortalBasic &);

   };
};

#endif //  DMZ_AUDIO_MODULE_PORTAL_BASIC_DOT_H

