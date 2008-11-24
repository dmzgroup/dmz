#ifndef DMZ_TYPES_HASH_TABLE_HANDLE_TEMPLATE_DOT_H
#define DMZ_TYPES_HASH_TABLE_HANDLE_TEMPLATE_DOT_H

#include <dmzTypesHashTableHandle.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>

namespace dmz {

/*!

\ingroup Types
\brief Handle hash table template.
\details Provides a template interface to the type unsafe dmz::HashTableHandle class.
The HashTableHandleTemplate retains the order in which the elements are stored in the
table. When iterating over the elements, they are returned in the order in which
they were stored from earliest to latest. The hash table is by default \b not thread
safe. If a HashTableLock is provided, the hash table will use it to make all
functions thread safe.

*/
   template <class T> class HashTableHandleTemplate {

      public:
         HashTableHandleTemplate ();
         HashTableHandleTemplate (const HashTableHandleTemplate<T> &Table);
         HashTableHandleTemplate (HashTableLock *lock);

         HashTableHandleTemplate (
            const Int32 Size,
            const UInt32 Attributes,
            HashTableLock *lock);

         ~HashTableHandleTemplate ();

         HashTableHandleTemplate<T> &operator= (
            const HashTableHandleTemplate<T> &Table);

         void copy (const HashTableHandleTemplate<T> &Table);

         void clear ();
         void empty ();

         Int32 get_size () const;
         Int32 get_count () const;

         Boolean move_first (const Handle &Key);
         Boolean move_up (const Handle &Key);
         Boolean move_before (const Handle &Key, const Handle &TargetKey);
         Boolean move_after (const Handle &Key, const Handle &TargetKey);
         Boolean move_down (const Handle &Key);
         Boolean move_last (const Handle &Key);

         T *get_first (HashTableHandleIterator &it) const;
         T *get_last (HashTableHandleIterator &it) const;
         T *get_next (HashTableHandleIterator &it) const;
         T *get_prev (HashTableHandleIterator &it) const;

         T *lookup (const Handle &Key) const;
         Boolean store (const Handle &Key, T *data);
         T *remove (const Handle &Key);

         void set_lock (HashTableLock *lock);
         HashTableLock *get_lock ();

      private:
         void __lock () const;
         void __unlock () const;

         HashTableHandle __table; //!< Internal state.
         HashTableLock *__tableLock; //!< Internal lock.
   };
};


template <class T> inline void
dmz::HashTableHandleTemplate<T>::__lock () const {

   if (__tableLock) { __tableLock->lock (); }
}


template <class T> inline void
dmz::HashTableHandleTemplate<T>::__unlock () const {

   if (__tableLock) { __tableLock->unlock (); }
}


/*!

\brief Default constructor.
\details The table will auto grow by default.

*/
template <class T> inline
dmz::HashTableHandleTemplate<T>::HashTableHandleTemplate () : __tableLock (0) {;}


/*!

\brief Copy constructor.
\param[in] Table Hash table to copy.
\note The elements are not copied. Each table will contain pointers to the same
elements.

*/
template <class T> inline
dmz::HashTableHandleTemplate<T>::HashTableHandleTemplate (
      const HashTableHandleTemplate<T> &Table) : __tableLock (0) { *this = Table; }


//! Lock constructor.
template <class T> inline
dmz::HashTableHandleTemplate<T>::HashTableHandleTemplate (
      HashTableLock *lock) : __tableLock (lock) {;}


/*!

\brief Constructor.
\details If the table is set to auto grow, then the size will always be a power of
two. Also if the table is set to auto grow and the \a Size is not a power of two, the
initial size will be the first power of two number greater than \a Size.
If auto grow is not set, then the table will be the exact size specified by \a Size.
\param[in] Size Initial size of the table.
\param[in] Attributes Simple 32 bit mask containing the tables attributes.
\param[in] lock Pointer to the lock used to make the table thread safe. May be NULL
if the hash table does not need to be thread safe.
\note The only value the attribute mask can take at this time is dmz::AutoGrow.

*/
template <class T> inline
dmz::HashTableHandleTemplate<T>::HashTableHandleTemplate (
      const Int32 Size,
      const UInt32 Attributes,
      HashTableLock *lock) : __table (Size, Attributes), __tableLock (lock) {;}


//! Destructor
template <class T> inline
dmz::HashTableHandleTemplate<T>::~HashTableHandleTemplate () {;}


/*!

\brief Assignment operator.
\details Elements are not copied. After the operation, the table will contain the
same pointers to elements as the table being copied.

*/
template <class T> inline dmz::HashTableHandleTemplate<T> &
dmz::HashTableHandleTemplate<T>::operator= (const HashTableHandleTemplate<T> &Table) {

   __lock ();
   const UInt32 Attributes (Table.__table.get_attributes ());
   __table.set_attributes (Attributes);

   if (Attributes & AutoGrow) {

      __table.clear ();
      const Int32 Size (Table.__table.get_size ());
      if (Size) { __table.grow (Size); }
      else { __table.set_table_size (0); }
   }
   else { __table.set_table_size (Table.__table.get_size ()); }

   HashTableHandleIterator it;

   T *data = Table.get_first (it);

   while (data) {

      __table.store (it.get_hash_key (), (void *)data);
      data = Table.get_next (it);
   }
   __unlock ();

   return *this;
}


/*!

\brief Copies the contents of a table.
\details This function will make a copy of the contents of the \a Table being copied.
After the operation, each table will contain its own copies of each element.
\note This function may only be used when the template type T supports the copy
constructor.
\param[in] Table HashTableHandle to be copied.

*/
template <class T> inline void
dmz::HashTableHandleTemplate<T>::copy (const HashTableHandleTemplate<T> &Table) {

   if (Table.get_count ()) {

      __lock ();
      const UInt32 Attributes (Table.__table.get_attributes ());
      __table.set_attributes (Attributes);

      if (Attributes & AutoGrow) {

         __table.grow (Table.__table.get_size ());
      }
      else { __table.set_table_size (Table.__table.get_size ()); }

      HashTableHandleIterator it;

      T *data = Table.get_first (it);

      while (data) {

         T *nd (new T (*data));
         if (!__table.store (it.get_hash_key (), (void *)nd)) { delete nd; nd = 0; }
         data = Table.get_next (it);
      }
      __unlock ();
   }
}


/*!

\brief Clears the hash table.
\note This function does not delete the elements in table but only clears out the
table so that is it empty.

*/
template <class T> inline void
dmz::HashTableHandleTemplate<T>::clear () { __table.clear (); }


/*!

\brief Clears table and deletes elements.
\details This function deletes all the elements in the table.
\note This function may only be used when template type T has a public destructor.

*/
template <class T> inline void
dmz::HashTableHandleTemplate<T>::empty () {

   HashTableHandleIterator it;

   __lock ();
   T *data = (T *)__table.get_next  (it);

   while (data) {

      delete data;
      data = (T *)__table.get_next (it);
   }

   __table.clear ();
   __unlock ();
}


/*!

\brief Gets the size of the table.
\note The size indicated how man element slots the table has.

*/
template <class T> inline dmz::Int32
dmz::HashTableHandleTemplate<T>::get_size () const { return __table.get_size (); }


//! Gets number of elements stored in the table.
template <class T> inline dmz::Int32
dmz::HashTableHandleTemplate<T>::get_count () const { return __table.get_count (); }


//! Moves element to the beginning of the list.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_first (const Handle &Key) {

   __lock ();
   Boolean result = __table.move (Key, 0, False, True);
   __unlock ();

   return result;
}


//! Moves element to the beginning of the list.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_up (const Handle &Key) {

   __lock ();
   Boolean result = __table.move (Key, 0, True, True);
   __unlock ();

   return result;
}

//! Moves element before the element specified by TargetKey.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_before (
      const Handle &Key,
      const Handle &TargetKey) {

   __lock ();
   Boolean result = __table.move (Key, &TargetKey, False, True);
   __unlock ();

   return result;
}


//! Moves element after the element specified by TargetKey.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_after (
      const Handle &Key,
      const Handle &TargetKey) {

   __lock ();
   Boolean result = __table.move (Key, &TargetKey, False, False);
   __unlock ();

   return result;
}


//! Moves element to the end of the list.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_down (const Handle &Key) {

   __lock ();
   Boolean result = __table.move (Key, 0, True, False);
   __unlock ();

   return result;
}

//! Moves element to the end of the list.
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::move_last (const Handle &Key) {

   __lock ();
   Boolean result = __table.move (Key, 0, False, False);
   __unlock ();

   return result;
}


/*!

\brief Gets first element stored in the table.
\param[in] it Iterator used to traverse the table.
\return Returns pointer to the first element in the table. Will return NULL if the
table is empty.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::get_first (HashTableHandleIterator &it) const {

   __lock ();
   it.reset ();
   T *result =  (T *)__table.get_next (it);
   __unlock ();

   return result;
}


/*!

\brief Gets last element stored in the table.
\param[in] it Iterator used to traverse the table.
\return Returns pointer to the last element in the table. Will return NULL if the
table is empty.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::get_last (HashTableHandleIterator &it) const {

   __lock ();
   it.reset ();
   T *result = (T *)__table.get_next (it, True);
   __unlock ();
   return result;
}


/*!

\brief Gets next element stored in the table.
\param[in] it Iterator used to traverse the table.
\return Returns pointer to the next element in the table. Will return NULL if all
elements have been iterated over.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::get_next (HashTableHandleIterator &it) const {

   __lock ();
   T *result = (T *)__table.get_next (it);
   __unlock ();

   return result;
}


/*!

\brief Gets previous element stored in the table.
\param[in] it Iterator used to traverse the table.
\return Returns pointer to the previous element in the table. Will return NULL if all
elements have been iterated over.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::get_prev (HashTableHandleIterator &it) const {

   __lock ();
   T *result = (T *)__table.get_next (it, True);
   __unlock ();

   return result;
}


/*!

\brief Looks up element in table.
\param[in] Key Hash key associated with the element being lookup.
\return Returns pointer to the element. Will return NULL if no element was
stored with the given key.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::lookup (const Handle &Key) const {

   __lock ();
   T *result = (T *)__table.lookup (Key);
   __unlock ();

   return result;
}


/*!

\brief Stores element in table.
\param[in] Key Hash key used to store the element.
\param[in] data Pointer to the element being stored.
\return Returns dmz::True if the element was stored. Returns dmz::False
if an element has already been stored using the given hash key or the
table is out of storage space.

*/
template <class T> inline dmz::Boolean
dmz::HashTableHandleTemplate<T>::store (const Handle &Key, T *data) {

   __lock ();
   const Boolean Result =  __table.store (Key, (void *)data);
   __unlock ();
   return Result;
}


/*!

\brief Remove element from table.
\param[in] Key Hash key associated with element being removed.
\return Returns a pointer to the element removed from the table. Returns NULL
if there is no element associated with the given key.

*/
template <class T> inline T *
dmz::HashTableHandleTemplate<T>::remove (const Handle &Key) {

   __lock ();
   T *result = (T *)__table.remove (Key);
   __unlock ();

   return result;
}


//! Sets the table lock.
template <class T> inline void
dmz::HashTableHandleTemplate<T>::set_lock (HashTableLock *lock) {

   __tableLock = lock;
}


//! Gets the table lock.
template <class T> inline dmz::HashTableLock *
dmz::HashTableHandleTemplate<T>::get_lock () { return __tableLock; }
#endif /* DMZ_TYPES_HASH_TABLE_HANDLE_TEMPLATE_DOT_H */
