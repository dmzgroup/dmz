#ifndef DMZ_OBJECT_CALC_DOT_H
#define DMZ_OBJECT_CALC_DOT_H

#include <dmzObjectUtilExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Config;
   class Log;
   class ObjectModule;
   class RuntimeContext;

   class ObjectAttributeCalculator {

      public:
         virtual ~ObjectAttributeCalculator () {;}
         virtual void store_object_module (ObjectModule *module) = 0;
         virtual Float64 calculate (const Handle ObjectHandle) = 0;

      protected:
         ObjectAttributeCalculator () {;}
   };

   DMZ_OBJECT_UTIL_LINK_SYMBOL ObjectAttributeCalculator *
   config_to_object_attribute_calculator (
      const String &Name,
      Config &source,
      RuntimeContext *context,
      Log *log = 0);
};

#endif // DMZ_OBJECT_CALC_DOT_H
