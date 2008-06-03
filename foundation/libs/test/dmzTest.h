#ifndef DMZ_TEST_DOT_H
#define DMZ_TEST_DOT_H

#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTestExport.h>
#include <dmzRuntimeLogObserverBasic.h>

namespace dmz {

   class DMZ_TEST_LINK_SYMBOL Test {

      public:
         Test (const String &AppName, int argc, char *argv[]);
         ~Test ();

         Boolean validate (const String &Message, const Boolean Result);
         void passed ();
         void failed ();
         Int32 result ();

         Runtime rt; //!< Runtime object.
         LogObserverBasic obs; //!< Basic log observer.
         Config config; //!< Root of config context tree.
         Log log; //!< Log.
         String out; //!< Output string. Used by validate, passed, and failed.
         Boolean error; //!< Error flag.

      private:
         Test (const Test &);
         Test &operator= (const Test &);
   };
};

#endif // DMZ_TEST_DOT_H
