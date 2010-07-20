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

         HashTableHandleTemplate<ResourcesObserver> obsTable;
         HashTableStringTemplate<Config> rcTable;
         HashTableStringTemplate<StringContainer> pathTable;

      private:
         ~RuntimeContextResources ();
   };
   //! \endcond
};

inline
dmz::RuntimeContextResources::~RuntimeContextResources () {

   obsTable.clear ();
   rcTable.empty ();
   pathTable.empty ();
}

#endif // DMZ_RUNTIME_CONTEXT_RESOURCES_DOT_H
