#ifndef DMZ_RUNTIME_CONTEXT_RESOURCES_DOT_H
#define DMZ_RUNTIME_CONTEXT_RESOURCES_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeResourcesObserver.h>
#include <dmzSystemFile.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesString.h>

namespace dmz {

   //! \cond
   class RuntimeContextResources : public RefCountDeleteOnZero {

      public:
         RuntimeContextResources () {;}

         HashTableHandleTemplate<ResourcesObserver> rcObsTable;
         HashTableStringTemplate<Config> rcTable;
         HashTableHandleTemplate<ResourcesObserver> pathObsTable;
         HashTableStringTemplate<StringContainer> pathTable;

      private:
         ~RuntimeContextResources ();
   };
   //! \endcond
};

inline
dmz::RuntimeContextResources::~RuntimeContextResources () {

   rcObsTable.clear ();
   rcTable.empty ();
   pathObsTable.clear ();
   pathTable.empty ();
}

#endif // DMZ_RUNTIME_CONTEXT_RESOURCES_DOT_H
