#ifndef DMZ_ARCHIVE_OBSERVER_UTIL_DOT_H
#define DMZ_ARCHIVE_OBSERVER_UTIL_DOT_H

#include <dmzArchiveUtilExport.h>
#include <dmzArchiveObserver.h>
#include <dmzTypesMask.h>

namespace dmz {

   class ArchiveModule;
   class Config;
   class Log;

   class DMZ_ARCHIVE_UTIL_LINK_SYMBOL ArchiveObserverUtil : public ArchiveObserver {

      public:
         Boolean is_active_archive_handle (const Handle ArchiveHandle);

         void init_archive (const Config &Init, Log *log = 0);
         void init_archive_scope (const Config &Init);
         void add_archive_scope (const String &Scope);
         Boolean is_archive_scope_empty ();

         Handle activate_default_archive ();
         Handle activate_archive (const String &ArchiveName);
         Boolean activate_archive (const Handle ArchiveHandle);
         void deactivate_default_archive ();
         void deactivate_archive (const String &ArchiveName);
         void deactivate_archive (const Handle ArchiveHandle);
         void deactivate_archive_all ();

         ArchiveModule *get_archive_module ();

         // ArchiveObserver Interface.
         virtual StringContainer get_archive_scope (const Handle ArchiveHandle);

         virtual void store_archive_module (
            const String &Name,
            ArchiveModule &module);

         virtual void remove_archive_module (
            const String &Name,
            ArchiveModule &module);

         virtual void pre_create_archive (
            const Handle ArchiveHandle,
            const Int32 Version) {;}
         
         virtual void create_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global) {;}

         virtual void post_create_archive (
            const Handle ArchiveHandle,
            const Int32 Version) {;}         

         virtual void pre_process_archive (
            const Handle ArchiveHandle,
            const Int32 Version) {;}
         
         virtual void process_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global) {;}

         virtual void post_process_archive (
            const Handle ArchiveHandle,
            const Int32 Version) {;}

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

