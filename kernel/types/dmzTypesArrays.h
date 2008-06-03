#ifndef DMZ_TYPES_ARRAYS_DOT_H
#define DMZ_TYPES_ARRAYS_DOT_H

#include <dmzTypesArrayTemplate.h>

//! \file
//! \brief Contains typedefs for basic array types.

namespace dmz {

//! \addtogroup Types
//! @{

   //! dmz::Int8 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Int8> ArrayInt8;
   //! dmz::Int16 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Int16> ArrayInt16;
   //! dmz::Int32 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Int32> ArrayInt32;
   //! dmz::Int64 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Int64> ArrayInt64;
   //! dmz::UInt8 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<UInt8> ArrayUInt8;
   //! dmz::UInt16 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<UInt16> ArrayUInt16;
   //! dmz::UInt32 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<UInt32> ArrayUInt32;
   //! dmz::UInt64 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<UInt64> ArrayUInt64;
   //! dmz::Float32 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Float32> ArrayFloat32;
   //! dmz::Float64 array. Defined in dmzTypesArrays.h.
   typedef ArrayTemplate<Float64> ArrayFloat64;

//! @}

};

#endif // DMZ_TYPES_ARRAYS_DOT_H
