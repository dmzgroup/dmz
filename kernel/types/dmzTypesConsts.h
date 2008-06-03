#ifndef DMZ_TYPES_CONSTS_DOT_H
#define DMZ_TYPES_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzKernelExport.h>

/*!

\file
\brief Contains platform and mathematical constants.

*/

namespace dmz {

   //! \addtogroup Types
   //! @{

   // HashTable Consts
   DMZ_KERNEL_LINK_SYMBOL extern const UInt32 AutoGrow;

   // Math Consts
   DMZ_KERNEL_LINK_SYMBOL extern const Float32 Pi32;
   DMZ_KERNEL_LINK_SYMBOL extern const Float32 HalfPi32;
   DMZ_KERNEL_LINK_SYMBOL extern const Float32 TwoPi32;
   DMZ_KERNEL_LINK_SYMBOL extern const Float64 Pi64;
   DMZ_KERNEL_LINK_SYMBOL extern const Float64 HalfPi64;
   DMZ_KERNEL_LINK_SYMBOL extern const Float64 TwoPi64;

   DMZ_KERNEL_LINK_SYMBOL extern const Float32 Epsilon32;
   DMZ_KERNEL_LINK_SYMBOL extern const Float64 Epsilon64;

   DMZ_KERNEL_LINK_SYMBOL extern const Float32 EarthGravity32;
   DMZ_KERNEL_LINK_SYMBOL extern const Float64 EarthGravity64;

   //! @}
};

#endif // DMZ_TYPES_CONSTS_DOT_H
