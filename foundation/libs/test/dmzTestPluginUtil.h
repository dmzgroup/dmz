#ifndef DMZ_TEST_PLUGIN_UTIL_DOT_H
#define DMZ_TEST_PLUGIN_UTIL_DOT_H

#include <dmzTestExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class RuntimeContext;

   class DMZ_TEST_LINK_SYMBOL TestPluginUtil {

      public:
         TestPluginUtil (const String &AppName, RuntimeContext *context);
         ~TestPluginUtil ();

         void validate (const Boolean Value, const String &Message);
         void passed ();
         void failed ();
         void exit (const String &Message);

         String out; //!< String for storing test messages.

      protected:

      private:
         struct State;
         TestPluginUtil ();
         TestPluginUtil (const TestPluginUtil &);
         TestPluginUtil &operator= (const TestPluginUtil &);
         State &__state;
   };
};

#endif // DMZ_TEST_PLUGIN_UTIL_DOT_H
