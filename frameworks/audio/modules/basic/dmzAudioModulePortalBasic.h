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
         AudioModulePortalBasic (const PluginInfo &Info, const Config &Local);
         ~AudioModulePortalBasic (); 

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         String get_audio_portal_name () const;
         UInt32 get_audio_portal_handle () const;

         virtual Boolean is_master_portal () const;

         virtual void set_view (const Vector &Pos, const Matrix &Ori);
         virtual void get_view (Vector &pos, Matrix &ori) const;

         virtual void set_mute_state (const Boolean Value);
         virtual Boolean get_mute_state () const;

      protected:

         void _init (const Config &Local);

         UInt32 _handle;
         AudioModule *_audioModule;
         String _name;

      private:
         AudioModulePortalBasic (const AudioModulePortalBasic &);
         AudioModulePortalBasic &operator= (const AudioModulePortalBasic &);
         
   };
};

#endif //  DMZ_AUDIO_MODULE_PORTAL_BASIC_DOT_H

