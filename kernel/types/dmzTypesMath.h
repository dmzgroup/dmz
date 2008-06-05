#ifndef DMZ_TYPES_MATH_DOT_H
#define DMZ_TYPES_MATH_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Vector;
   class Matrix;

   DMZ_KERNEL_LINK_SYMBOL void get_ortho (
      const Vector &Normal,
      const Vector &Value, 
      Vector &ortho,
      Vector *remainder = 0);
      
   DMZ_KERNEL_LINK_SYMBOL Float64 get_heading (const Matrix &Ori);
};
 
#endif // DMZ_TYPES_MATH_DOT_H
