#ifndef DMZ_RUNTIME_CONFIG_TO_MATRIX_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_MATRIX_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;

   Matrix config_to_matrix (const Config &Source);
   Matrix config_to_matrix (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Matrix config_to_matrix (
      const String &Name,
      const Config &Source,
      const Matrix &DefaultValue);
};


inline dmz::Matrix
dmz::config_to_matrix (const Config &Source) {

   Matrix value; value.set_identity ();
   return config_to_matrix ("", Source, value);
}


inline dmz::Matrix
dmz::config_to_matrix (const String &Name, const Config &Source) {

   Matrix value; value.set_identity ();
   return config_to_matrix (Name, Source, value);
}

#endif // DMZ_RUNTIME_CONFIG_TO_MATRIX_DOT_H

