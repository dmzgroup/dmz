#ifndef DMZ_ARCHIVE_OBSERVER_DOT_H
#define DMZ_ARCHIVE_OBSERVER_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesStringContainer.h>

namespace dmz {

   //! \cond
   const char ArchiveObserverInterfaceName[] = "ArchiveObserverInterface";
   //! \endcond

   class ArchiveModule;

   class ArchiveObserver {

      public:
         static ArchiveObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         Handle get_archive_observer_handle ();
         String get_archive_observer_name ();

         // ArchiveObserver Interface
         virtual StringContainer get_archive_scope (const Handle ArchiveHandle) = 0;
         virtual void store_archive_module (
            const String &Name,
            ArchiveModule &module) = 0;

         virtual void remove_archive_module (
            const String &Name,
            ArchiveModule &module) = 0;

         virtual void pre_create_archive (
            const Handle ArchiveHandle,
            const Int32 Version) = 0;

         virtual void create_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global) = 0;

         virtual void post_create_archive (
            const Handle ArchiveHandle,
            const Int32 Version) = 0;

         virtual void pre_process_archive (
            const Handle ArchiveHandle,
            const Int32 Version) = 0;

         virtual void process_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global) = 0;

         virtual void post_process_archive (
            const Handle ArchiveHandle,
            const Int32 Version) = 0;

      protected:
         ArchiveObserver (const PluginInfo &Info);
         ~ArchiveObserver ();

      private:
         ArchiveObserver ();
         ArchiveObserver (const ArchiveObserver &);
         ArchiveObserver &operator= (const ArchiveObserver &);

         const PluginInfo &__Info;
   };
};


inline dmz::ArchiveObserver *
dmz::ArchiveObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ArchiveObserver *)lookup_rtti_interface (
      ArchiveObserverInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ArchiveObserver::ArchiveObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ArchiveObserverInterfaceName, __Info, (void *)this);
}


inline
dmz::ArchiveObserver::~ArchiveObserver () {

   remove_rtti_interface (ArchiveObserverInterfaceName, __Info);
}


inline dmz::String
dmz::ArchiveObserver::get_archive_observer_name () { return __Info.get_name (); }


inline dmz::Handle
dmz::ArchiveObserver::get_archive_observer_handle () { return __Info.get_handle (); }

#endif // DMZ_ARCHIVE_OBSERVER_DOT_H
