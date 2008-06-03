#ifndef DMZ_RUNTIME_TYPE_CONTEXT_DOT_H
#define DMZ_RUNTIME_TYPE_CONTEXT_DOT_H

#include "dmzRuntimeConfigContext.h"
#include <dmzRuntimeHandle.h>
#include <dmzSystemRefCount.h>
#include <dmzSystemSpinLock.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>

namespace dmz {

   class TypeContext : public RefCountDeleteOnZero {

      public:
         TypeContext (
            const String &TheName,
            RuntimeContext *context,
            TypeContext *theParent,
            ConfigContext *theConfig);

         const String Name;
         const RuntimeHandle Handle;

         SpinLock lock;

         TypeContext *parent;
         ConfigContext *config;
         HashTableHandleTemplate<TypeContext> table;

      protected:
         ~TypeContext ();

      private:
         TypeContext ();
         TypeContext (const TypeContext &);
         TypeContext &operator= (const TypeContext &);
   };
};

inline
dmz::TypeContext::TypeContext (
      const String &TheName,
      RuntimeContext *context,
      TypeContext *theParent,
      ConfigContext *theConfig) :
      Name (TheName),
      Handle (TheName + ".Type", context),
      parent (theParent),
      config (theConfig) {

   if (config) { config->ref (); }
   if (theParent) {

      theParent->lock.lock ();
      if (theParent->table.store (Handle.get_runtime_handle (), this)) { this->ref (); }
      theParent->lock.unlock ();
   }
}


inline
dmz::TypeContext::~TypeContext () {

   HashTableHandleIterator it;

   lock.lock ();

      parent = 0;

      TypeContext *current = table.get_first (it);

      while (current) {

         current->lock.lock ();
         current->parent = 0;
         current->lock.unlock ();
         current->unref ();
         current = table.get_next (it);
      }

      table.clear ();

      if (config) { config->unref (); config = 0; }

   lock.unlock ();

}

#endif // DMZ_RUNTIME_TYPE_CONTEXT_DOT_H
