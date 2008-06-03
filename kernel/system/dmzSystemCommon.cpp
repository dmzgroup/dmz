#include <dmzSystem.h>

/*!

\defgroup System DMZ Kernel System Layer
\brief System abstraction layer.
\details The DMZ Kernel System Layer provides an interface to common system level
functionality such as threading, locking, and I/O.

\file dmzSystem.h
\brief Contains basic system level functions.

\enum dmz::ByteOrderEnum
\ingroup System
\brief Byte order enumeration.
\details Defined in dmzSystem.h.

*/

/*!

\ingroup System
\brief Returns the byte order of the current system.
\details Defined in dmzSystem.h.
\return Returns dmz::ByteOrderBigEndian for big endian based systems and
dmz::ByteOrderLittleEndian for little endian systems.

*/
dmz::ByteOrderEnum
dmz::get_byte_order () {

   ByteOrderEnum result (ByteOrderLittleEndian);
   UInt16 test (0);
   ((char *)&test)[1] = char (1);
   if (test == 1) { result = ByteOrderBigEndian; }

   return result;
}

