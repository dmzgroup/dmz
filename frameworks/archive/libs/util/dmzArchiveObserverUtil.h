#ifndef DMZ_ARCHIVE_OBSERVER_UTIL_DOT_H
#define DMZ_ARCHIVE_OBSERVER_UTIL_DOT_H

#include <dmzArchiveUtilExport.h>
#include <dmzArchiveObserver.h>
#include <dmzTypesMask.h>

namespace dmz {

   class ArchiveModule;
   class Config;

   class DMZ_ARCHIVE_UTIL_LINK_SYMBOL ArchiveObserverUtil : public ArchiveObserver {

      public:
         Handle activate_default_archive ();
         Handle activate_archive (const String &ArchiveName);
         void deactivate_default_archive ();
         void deactivate_archive (const String &ArchiveName);
         void deactivate_archive (const Handle ArchiveHandle);
         void deactivate_archive_all ();

         ArchiveModule *get_archive_module ();

         // ArchiveObserver Interface.
         virtual void store_archive_module (
            const String &Name,
            ArchiveModule &module);

         virtual void remove_archive_module (
            const String &Name,
            ArchiveModule &module);

         virtual void pre_create_archive (const Handle ArchiveHandle) {;}

         virtual void create_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global) {;}

         virtual void post_create_archive (const Handle ArchiveHandle) {;}

         virtual void pre_process_archive (const Handle ArchiveHandle) {;}

         virtual void process_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global) {;}

         virtual void post_process_archive (const Handle ArchiveHandle) {;}

      protected:
         ArchiveObserverUtil (const PluginInfo &Info, const Config &Init);
         ~ArchiveObserverUtil ();

      private:
         struct State;
         ArchiveObserverUtil ();
         ArchiveObserverUtil (const ArchiveObserverUtil &);
         ArchiveObserverUtil &operator= (const ArchiveObserverUtil &);
         State &__state;
   };
};

#endif // DMZ_ARCHIVE_OBSERVER_UTIL_DOT_H

