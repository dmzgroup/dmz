
/*!

\class dmz::HashTableLock
\ingroup Types
\brief Provides interface for create a hash table lock.
\details Hash tables use the lock for thread safe operations. This interface
allows the hash tables to be flexible when it comes to the type of lock used.
\sa dmz::HashTableStringTemplate \n
dmz::HashTableUInt32Template \n
dmz::HashTableUInt64Template \n
dmz::HashTableUUIDTemplate

\fn dmz::HashTableLock::~HashTableLock ()
\brief Destructor.

\fn void dmz::HashTableLock::lock ()
\brief Pure virtual function that when implemented will lock the HashTableLock.

\fn void dmz::HashTableLock::unlock ()
\brief Pure virtual function that when implemented will unlock the HashTableLock.

*/
