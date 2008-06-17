#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRenderModuleCoreOgre.h>
#include "dmzRenderPluginStatsOgre.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <Ogre/Ogre.h>


dmz::RenderPluginStatsOgre::RenderPluginStatsOgre (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      _portalName (DefaultPortalNameOgre),
      _core (0),
      _debugOverlay (0),
      _statsOverlay (0),
      _log (Info) {

   _init (local);
}


dmz::RenderPluginStatsOgre::~RenderPluginStatsOgre () {

}


// Plugin Interface
void
dmz::RenderPluginStatsOgre::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOgre::cast (PluginPtr);

         if (_core) {

            try {

               _debugOverlay =
                  Ogre::OverlayManager::getSingleton().getByName (
                     "RenderPluginStatsOgre/DebugOverlay");

               if (_debugOverlay) { _debugOverlay->show (); }

               _statsOverlay =
                  Ogre::OverlayManager::getSingleton ().getOverlayElement (
                     "RenderPluginStatsOgre/StatPanel");

               if (_statsOverlay) { _statsOverlay->hide (); }

            }
            catch (Ogre::Exception e) {

               _log.error << e.getFullDescription ().c_str () << endl;

               _debugOverlay = 0;
               _statsOverlay = 0;
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOgre::cast (PluginPtr))) {

         if (_debugOverlay) {

            _debugOverlay->hide ();
            _debugOverlay = 0;
         }

         _statsOverlay = 0;
         _core = 0;
      }
   }
}


void
dmz::RenderPluginStatsOgre::update_time_slice (const Float64 TimeDelta) {

   _update_stats ();
}


// Input Observer Interface
void
dmz::RenderPluginStatsOgre::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

}


void
dmz::RenderPluginStatsOgre::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   if (Value.get_key_state () && _statsOverlay) {

      if (Value.get_key () == KeyF3) {

         if (_statsOverlay->isVisible ()) {

            _statsOverlay->hide ();
         }
         else {

            _statsOverlay->show ();
         }
      }
   }
}


void
dmz::RenderPluginStatsOgre::_update_stats () {

   if (_statsOverlay && _statsOverlay->isVisible ()) {

      static Ogre::String currFps = "Current FPS: ";
      static Ogre::String avgFps = "Average FPS: ";
      static Ogre::String bestFps = "Best FPS: ";
      static Ogre::String worstFps = "Worst FPS: ";
      static Ogre::String tris = "Triangle Count: ";
      static Ogre::String batches = "Batch Count: ";

      // update stats when necessary
      try {

         Ogre::RenderTarget *renderTarget =
            Ogre::Root::getSingleton ().getRenderTarget (_portalName.get_buffer ());

         Ogre::OverlayElement *guiCurr =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/CurrFps");

         Ogre::OverlayElement *guiAvg =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/AverageFps");

         Ogre::OverlayElement *guiWorst =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/WorstFps");

         Ogre::OverlayElement *guiBest =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/BestFps");

         Ogre::OverlayElement *guiTris =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/NumTris");

         Ogre::OverlayElement* guiBatches =
            Ogre::OverlayManager::getSingleton().getOverlayElement(
               "RenderPluginStatsOgre/NumBatches");

         const Ogre::RenderTarget::FrameStats& stats = renderTarget->getStatistics();

         guiCurr->setCaption (currFps + Ogre::StringConverter::toString(stats.lastFPS));

         guiAvg->setCaption (avgFps + Ogre::StringConverter::toString(stats.avgFPS));

         guiWorst->setCaption (
            worstFps + Ogre::StringConverter::toString(stats.worstFPS)
            +" "+Ogre::StringConverter::toString(stats.worstFrameTime)+" ms");

         guiBest->setCaption (
            bestFps + Ogre::StringConverter::toString(stats.bestFPS)
            +" "+Ogre::StringConverter::toString(stats.bestFrameTime)+" ms");

         guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

         guiBatches->setCaption(
            batches + Ogre::StringConverter::toString(stats.batchCount));
      }
      catch(...) { /* ignore */ }
   }
}


void
dmz::RenderPluginStatsOgre::_init (Config &local) {

   activate_default_input_channel (InputEventKeyMask);

   _portalName = config_to_string ("portal.name", local, _portalName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginStatsOgre (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginStatsOgre (Info, local);
}

};
