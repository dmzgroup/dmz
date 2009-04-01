#ifndef DMZ_TYPES_MATH_DOT_H
#define DMZ_TYPES_MATH_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

class Vector;
class Matrix;

DMZ_KERNEL_LINK_SYMBOL void get_orthogonal_component (
   const Vector &Normal,
   const Vector &Value,
   Vector &ortho,
   Vector *remainder = 0);

DMZ_KERNEL_LINK_SYMBOL Float64 get_heading (const Matrix &Ori);

DMZ_KERNEL_LINK_SYMBOL Float64 get_rotation_angle (const Vector &V1, const Vector &V2);

};

#endif // DMZ_TYPES_MATH_DOT_H
