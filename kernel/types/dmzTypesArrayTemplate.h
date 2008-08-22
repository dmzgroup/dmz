#ifndef DMZ_TYPES_ARRAY_TEMPLATE_DOT_H
#define DMZ_TYPES_ARRAY_TEMPLATE_DOT_H

#include <dmzTypesBase.h>
#include <string.h>

namespace dmz {

/*!

\brief Simple array template.
\ingroup Types
\note Only use with types that may be duplicated with memcpy and initialized with a zero.

*/
   template <class T> class ArrayTemplate {

      public:
         ArrayTemplate ();
         ArrayTemplate (const Int32 Size);
         ArrayTemplate (const ArrayTemplate<T> &Value);

         ~ArrayTemplate ();

         ArrayTemplate<T> &operator= (const ArrayTemplate<T> &Value);

         void clear ();
         void empty ();

         Int32 get_size () const;
         Int32 set_size (const Int32 Size);

         T get (const Int32 Index) const;
         Boolean set (const Int32 Index, const T Value);

      protected:
         Int32 _size; //!< Array size.
         T *_array; //!< Array storage.
   };
};


//! Constructor. No initial storage allocated.
template <class T> inline
dmz::ArrayTemplate<T>::ArrayTemplate () : _size (0), _array (0) {;}


/*!

\brief Constructor. Creates initial storage.
\param[in] Size Number of element the array should initially have.

*/
template <class T> inline
dmz::ArrayTemplate<T>::ArrayTemplate (const Int32 Size) : _size (0), _array (0) {

   set_size (Size);
}


//! Copy constructor.
template <class T> inline
dmz::ArrayTemplate<T>::ArrayTemplate (const ArrayTemplate<T> &Value) :
      _size (0),
      _array (0) {

   *this = Value;
}


//! Destructor. Storage is deleted when object is destroyed.
template <class T> inline
dmz::ArrayTemplate<T>::~ArrayTemplate () { if (_array) { delete []_array; _array = 0; } }


/*!

\brief Assignment operator.
\param[in] Value Array to copy.
\return Returns reference to self.
\note The array will resize to be the same size as the array being copied.

*/
template <class T> inline dmz::ArrayTemplate<T> &
dmz::ArrayTemplate<T>::operator= (const ArrayTemplate<T> &Value) {

   if (_array && (Value._size != _size)) {

      delete []_array; _array = 0; _size = 0;
   }

   if (Value._array) {

      if (!_array) { _array = new T[Value._size]; }

      if (_array) {

         _size = Value._size;
         memcpy (_array, Value._array, _size * sizeof (T));
      }
   }

   return *this;
}


//! Zeros out the storage.
template <class T> inline void
dmz::ArrayTemplate<T>::clear () {

   if (_size && _array) { memset (_array, 0, _size * sizeof (T)); }
}


//! Deletes storage.
template <class T> inline void
dmz::ArrayTemplate<T>::empty () {

   if (_size && _array) { delete []_array; _array = 0; _size = 0; }
}


//! Returns number elements in storage.
template <class T> inline dmz::Int32
dmz::ArrayTemplate<T>::get_size () const { return _size; }


/*!

\brief Sets storage size.
\details Previous data is retained when array is resized. If the new size is small
than the current size, all elements that do not fit are lost.
\param[in] Size Number element to allocate in the new storage.
\return Returns the number elements in the new storage.

*/
template <class T> inline dmz::Int32
dmz::ArrayTemplate<T>::set_size (const Int32 Size) {

   T *oldArray (_array);
   const Int32 OldSize (_size);

   if (Size > 0) {

      _array = new T[Size];

      if (_array) {

         _size = Size;

         memset (_array, 0, _size * sizeof (T));

         if (oldArray) {

            const Int32 CopySize (_size > OldSize ? OldSize : _size);

            memcpy (_array, oldArray, CopySize * sizeof (T));
         }
      }
   }
   else { _array = 0; _size = 0; }

   if (oldArray) { delete []oldArray; oldArray = 0; }

   return _size;
}


//! Returns element found at index. Returns element initialized to zero if not found.
template <class T> inline T
dmz::ArrayTemplate<T>::get (const Int32 Index) const {

   T result (0);

   if (Index < _size) { result = _array[Index]; }

   return result;
}


/*!

\brief Stores element at the specified index.
\param[in] Index Location in storage to put element.
\param[in] Value Element to be stored.
\return Returns dmz::True if element was stored.
\note If the \a Index points to a place outside of the current storage. The array
attempts to resized the storage to accommodate the new element.

*/
template <class T> inline dmz::Boolean
dmz::ArrayTemplate<T>::set (const Int32 Index, const T Value) {

   Boolean result (False);
   if (Index >= _size) { set_size (Index + 1); }
   if (Index < _size) { _array[Index] = Value; result = True; }
   return result;
}

#endif // DMZ_TYPES_ARRAY_TEMPLATE_DOT_H
