#ifndef DMZ_MODULE_HELLO_WORLD_DOT_H
#define DMZ_MODULE_HELLO_WORLD_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesString.h>
#include <stdio.h>

#define DMZ_MODULE_HELLO_WORLD_INTERFACE_NAME "ModuleHellowWorldInterface"

namespace dmz {

   class RuntimeContext;

   class ModuleHelloWorld {

      public:
        static ModuleHelloWorld *cast (const Plugin *PluginPtr);

        virtual void print_hello () = 0;

      protected:
         ModuleHelloWorld (const PluginInfo &Info);
         ~ModuleHelloWorld ();

      private:
         ModuleHelloWorld ();
         ModuleHelloWorld (const ModuleHelloWorld &);
         ModuleHelloWorld &operator= (const ModuleHelloWorld &);

         const PluginInfo &__Info;
   };
};


inline dmz::ModuleHelloWorld *
dmz::ModuleHelloWorld::cast (const Plugin *PluginPtr) {

   return (ModuleHelloWorld *)lookup_rtti_interface (
      DMZ_MODULE_HELLO_WORLD_INTERFACE_NAME,
      "",
      PluginPtr);
}


inline
dmz::ModuleHelloWorld::ModuleHelloWorld (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_MODULE_HELLO_WORLD_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::ModuleHelloWorld::~ModuleHelloWorld () {

   remove_rtti_interface (DMZ_MODULE_HELLO_WORLD_INTERFACE_NAME, __Info);
}

#endif // DMZ_MODULE_HELLO_WORLD_DOT_H
