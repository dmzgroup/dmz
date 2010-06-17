#ifndef DMZ_TYPES_MATH_DOT_H
#define DMZ_TYPES_MATH_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

class Vector;
class Matrix;

//! \addtogroup Types
//! @{

DMZ_KERNEL_LINK_SYMBOL Float64 get_heading (const Matrix &Ori);

DMZ_KERNEL_LINK_SYMBOL Boolean is_zero32 (const Float32 Value);
DMZ_KERNEL_LINK_SYMBOL Boolean is_zero32 (const Float32 Value, const Float32 Epsilon);
DMZ_KERNEL_LINK_SYMBOL Boolean is_zero64 (const Float64 Value);
DMZ_KERNEL_LINK_SYMBOL Boolean is_zero64 (const Float64 Value, const Float64 Epsilon);

DMZ_KERNEL_LINK_SYMBOL Float64 to_degrees (const Float64 Value);
DMZ_KERNEL_LINK_SYMBOL Float64 to_radians (const Float64 Value);

DMZ_KERNEL_LINK_SYMBOL Float64 normalize_angle (const Float64 Value, const Float64 Min);

DMZ_KERNEL_LINK_SYMBOL Float64 normalize_angle (const Float64 Value);

//! @}

};

#endif // DMZ_TYPES_MATH_DOT_H
