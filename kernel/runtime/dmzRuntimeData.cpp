#include "dmzRuntimeContext.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include "dmzRuntimeIteratorState.h"
#include <dmzSystemStream.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

#include <string.h> // for memset and memcpy

using namespace dmz;

namespace {

static inline
void local_string_to (const String &From, Boolean &to) { to = string_to_boolean (From); }

static inline
void local_string_to (const String &From, Int32 &to) { to = string_to_int32 (From); }

static inline
void local_string_to (const String &From, UInt32 &to) { to = string_to_uint32 (From); }

static inline
void local_string_to (const String &From, Int64 &to) { to = string_to_int64 (From); }

static inline
void local_string_to (const String &From, UInt64 &to) { to = string_to_uint64 (From); }

static inline
void local_string_to (const String &From, Float32 &to) { to = string_to_float32 (From); }

static inline
void local_string_to (const String &From, Float64 &to) { to = string_to_float64 (From); }

static inline
void local_string_to (const String &From, String &to) { to = From; }

struct attrStruct {

   const BaseTypeEnum Type;
   const Int32 Size;

   attrStruct (const BaseTypeEnum TheType, const Int32 TheSize) :
         Type (TheType),
         Size (TheSize) {;}
};

static const attrStruct BooleanAt (BaseTypeBoolean, sizeof (UInt32));
static const attrStruct Int32At (BaseTypeInt32, sizeof (Int32));
static const attrStruct Int64At (BaseTypeInt64, sizeof (Int64));
static const attrStruct UInt32At (BaseTypeUInt32, sizeof (UInt32));
static const attrStruct UInt64At (BaseTypeUInt64, sizeof (UInt64));
static const attrStruct Float32At (BaseTypeFloat32, sizeof (Float32));
static const attrStruct Float64At (BaseTypeFloat64, sizeof (Float64));
static const attrStruct StringAt (BaseTypeString, sizeof (UInt32));

struct dataStruct {

   const Handle AttrHandle;
   const attrStruct &Attr;
   char *data;
   Int32 size;

   dataStruct (const Handle TheHandle, const attrStruct &TheAttr) :
         AttrHandle (TheHandle),
         Attr (TheAttr), data (0), size (0) {;}

   dataStruct (const dataStruct &Value) :
         AttrHandle (Value.AttrHandle),
         Attr (Value.Attr),
         data (0),
         size (0) { *this = Value; }

   ~dataStruct () { if (data) { delete []data; data = 0; } }

   Int32 get_element_count () const { return Attr.Size ? (size / Attr.Size) : 0; }

   Boolean validate_offset (const Int32 Offset) {

      Boolean result (False);
      const Int32 ElementSizeof (Attr.Size);

      if (Offset >= 0) {

         if ((Offset + ElementSizeof) > size) {

            const Int32 NewSize = Offset + ElementSizeof;
            char *ptr = new char[NewSize];

            if (ptr) {

               memset (ptr, 0, NewSize);

               if (data && size) {

                  memcpy (ptr, data, size);
                  delete [](data); data = 0;
               }

               data = ptr;
               size = NewSize;
               result = True;
            }
         }
         else if (data) { result = True; }
      }

      return result;
   }

   Boolean store_string_handle (const Int32 Element, const UInt32 StrHandle) {

      Boolean result (False);
      const Int32 ElementSizeof = Attr.Size;
      const Int32 Offset = Element * ElementSizeof;

      const Boolean Validated (validate_offset (Offset));

      if (data && Validated) {

         *((UInt32 *)(&(data[Offset]))) = StrHandle;
         result = True;
      }

      return result;
   }

   UInt32 lookup_string_handle (const Int32 Element) const {

      UInt32 result (0);
      const Int32 ElementSizeof = Attr.Size;
      const Int32 Offset = Element * ElementSizeof;

      if (data && (Offset >= 0) && ((Offset + ElementSizeof) <= size)) {

         result = *((UInt32 *)(&(data[Offset])));
      }

      return result;
   }

   dataStruct &operator= (const dataStruct &Value) {

      if (size != Value.size) {

         if (data) { delete []data; data = 0; }
         size = Value.size;

         if (size) { data = new char[size]; }

         if (data) { memset (data, 0, size); }
      }

      if (data && Value.data) {

         memcpy (data, Value.data, size);
      }

      return *this;
   }

   Boolean operator== (const dataStruct &Value) {

      Boolean result (False);

      if (!data && !Value.data) { result = True; }
      else if (data && Value.data && (Value.size == size) && (&Value.Attr == &Attr)) {

         result = !memcmp (data, Value.data, size);
      }

      return result;
   }

   Boolean operator!= (const dataStruct &Value) { return !(*this == Value); }
};


struct stringStruct {

   UInt32 count;
   HashTableUInt32Template<String> table;

   stringStruct () : count (0) {;}
   ~stringStruct () { table.empty (); }

   UInt32 get_next_handle () {

      count++;
      while (table.lookup (count)) { count++; if (!count) { count++; } }
      return count;
   }

   stringStruct &operator= (const stringStruct &Table) {

      count = Table.count;

      table.empty ();
      table.copy (Table.table);

      return *this;
   }
};

typedef stringStruct * stringStructPtr;

template <class T> class dataConvertTemplate {

   public:
      dataConvertTemplate (const BaseTypeEnum Type);
      ~dataConvertTemplate () {;}

      Boolean write (
         const T &Value,
         const Int32 Element,
         stringStructPtr &table,
         dataStruct &ds);

      Boolean read (
         const dataStruct &Ds,
         const Int32 Element,
         const stringStruct *Table,
         T &data);

   protected:
      const BaseTypeEnum _Type;
};



template <class T> inline
dataConvertTemplate<T>::dataConvertTemplate (const BaseTypeEnum Type) :
      _Type (Type) {;}


template <class T> inline Boolean
dataConvertTemplate<T>::write (
      const T &Value,
      const dmz::Int32 Element,
      stringStructPtr &table,
      dataStruct &ds) {

   Boolean result (False);

   const Int32 Offset = Element * ds.Attr.Size;
   const BaseTypeEnum Type = ds.Attr.Type;

   const Boolean Validated (ds.validate_offset (Offset));
   char *ptr (ds.data);

   if (ptr && Validated) {

      result = True;

      if (Type == _Type) { *((T *)(&(ptr[Offset]))) = Value; }
      else if (Type == BaseTypeFloat64) {

         *((Float64 *)(&(ptr[Offset]))) = Float64 (Value);
      }
      else if (Type == BaseTypeFloat32) {

         *((Float32 *)(&(ptr[Offset]))) = Float32 (Value);
      }
      else if (Type == BaseTypeBoolean) {

         *((UInt32 *)(&(ptr[Offset]))) = UInt32 (Value);
      }
      else if (Type == BaseTypeInt32) { *((Int32 *)(&(ptr[Offset]))) = Int32 (Value); }
      else if (Type == BaseTypeUInt32) { *((UInt32 *)(&(ptr[Offset]))) = UInt32 (Value); }
      else if (Type == BaseTypeInt64) { *((Int64 *)(&(ptr[Offset]))) = Int64 (Value); }
      else if (Type == BaseTypeUInt64) { *((UInt64 *)(&(ptr[Offset]))) = UInt64 (Value); }
      else if (Type == BaseTypeString) {

         if (!table) { table = new stringStruct; }

         if (table) {

            UInt32 &handle (*((UInt32 *)(&(ptr[Offset]))));

            String *str (0);

            if (!handle) {

               handle = table->get_next_handle ();
               str = new String;
               if (!table->table.store (handle, str)) { delete str; str = 0; }
            }
            else { str = table->table.lookup (handle); }

            if (str) { str->flush () << Value; }
         }
      }
   }

   return result;
}


template <class T> inline Boolean
dataConvertTemplate<T>::read (
      const dataStruct &Ds,
      const Int32 Element,
      const stringStruct *Table,
      T &data) {

   Boolean result (False);

   const Int32 ElementSizeof = Ds.Attr.Size;
   const Int32 Offset = Element * ElementSizeof;
   const BaseTypeEnum Type = Ds.Attr.Type;

   if ((Offset >= 0) && ((Offset + ElementSizeof) <= Ds.size)) {

      result = True;

      if (Type == _Type) { data = *((T *)(&(Ds.data[Offset]))); }
      else if (Type == BaseTypeFloat64) { data = T (*((Float64 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeFloat32) { data = T (*((Float32 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeInt32) { data = T (*((Int32 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeBoolean) { data = T (*((UInt32 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeUInt32) { data = T (*((UInt32 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeInt64) { data = T (*((Int64 *)(&(Ds.data[Offset])))); }
      else if (Type == BaseTypeUInt64) { data = T (*((UInt64 *)(&(Ds.data[Offset])))); }
      else if (Table && (Type == BaseTypeString)) {

         const UInt32 Handle (UInt32 (*(&(Ds.data[Offset]))));

         String *ptr = Table->table.lookup (Handle);

         if (ptr) { local_string_to (*ptr, data); }
      }
   }

   return result;
}


static dataConvertTemplate<UInt32> booleanConvert (BaseTypeBoolean);
static dataConvertTemplate<Int32> int32Convert (BaseTypeInt32);
static dataConvertTemplate<Int64> int64Convert (BaseTypeInt64);
static dataConvertTemplate<UInt32> uint32Convert (BaseTypeUInt32);
static dataConvertTemplate<UInt64> uint64Convert (BaseTypeUInt64);
static dataConvertTemplate<Float32> float32Convert (BaseTypeFloat32);
static dataConvertTemplate<Float64> float64Convert (BaseTypeFloat64);

};

struct Data::State {

   HashTableHandleTemplate<dataStruct> dataTable;
   RuntimeContext *context;
   dataStruct *cache;
   stringStruct *stringTable;

   State (RuntimeContext *theContext) :
         context (theContext),
         cache (0),
         stringTable (0) { if (context) { context->ref (); } }

   void empty () {

      cache = 0;
      dataTable.empty ();
      if (stringTable) { delete stringTable; stringTable = 0; }
   }

   ~State () { empty (); if (context) { context->unref (); } }

   dataStruct *get_data (const Handle AttrHandle, const attrStruct *AttrPtr = 0) {

      dataStruct *ds (0);

      if (cache && (cache->AttrHandle == AttrHandle)) { ds = cache; }

      if (!ds) {

         ds = dataTable.lookup (AttrHandle);

         if (!ds && AttrPtr && AttrHandle) {

            ds = new dataStruct (AttrHandle, *AttrPtr);

            if (!dataTable.store (AttrHandle, ds)) { delete ds; ds = 0; }
         }

         if (ds) { cache = ds; }
      }

      return ds;
   }

   void to_string (const dataStruct &Ds, const Int32 Element, String &str) {

      if (stringTable && Ds.Attr.Type == BaseTypeString) {

         const UInt32 StrHandle (Ds.lookup_string_handle (Element));

         if (StrHandle) {

            String *ptr (stringTable->table.lookup (StrHandle));

            if (ptr) { str = *ptr; }
         }
      }
   }

   State &operator= (const State &Value) {

      cache = 0;

      if (Value.context && (Value.context != context)) {

         if (context) { context->unref (); context = 0; }

         context = Value.context;

         if (context) { context->ref (); }
      }

      dataTable.empty ();
      dataTable.copy (Value.dataTable);

      if (Value.stringTable) {

         if (!stringTable) { stringTable = new stringStruct; }
         if (stringTable) { *stringTable = *(Value.stringTable); }
      }
      else if (stringTable) { delete stringTable; stringTable = 0; }

      return *this;
   };
};


/*!

\class dmz::Data
\ingroup Runtime
\brief Class for storing a runtime defined data structure.
\details A dmz::Data object stores an instance of a runtime defined data structure.
A dmz::Data object is comprised of
named attributes. These named attributes may be one of the basic dmz types
(i.e. dmz::Int32, dmz::UInt32, dmz::Int64, dmz::UInt64, dmz::Float32, dmz::Float64,
and dmz::String). Each attribute may have an array elements. For example,
an attribute that represents a dmz::Vector is actually and attribute of type dmz::Float64
with three elements. Elements are indexed like a C array and start at zero.
The Handles may be create from a dmz::Definitions object.
\sa Definitions

*/


//! Default constructor.
dmz::Data::Data (RuntimeContext *context) : _state (*(new State (context))) {;}


/*!

\brief Copy constructor.
\details Uses the assignment operator to copy the data object.
\sa dmz::Data::operator=()

*/
dmz::Data::Data (const Data &Value) : _state (*(new State (Value._state.context))) {

   *this = Value;
}


//! Destructor.
dmz::Data::~Data () { delete &_state; }


/*!

\brief Assignment operator.
\details Copies the content of a data object. After the copy operation the
data object will be of the same type as the data object being copied.
\param[in] Value Data object to copy.
\return Returns reference to self.

*/
dmz::Data &
dmz::Data::operator= (const Data &Value) {

   _state = Value._state;
   return *this;
}


/*!

\brief Relational "equal to" operator.
\details Compares the attribute handles and element values.
\param[in] Value Data object to compare values.
\return Returns dmz::True if the two data object contain the same values for
their attributes.

*/
dmz::Boolean
dmz::Data::operator== (const Data &Value) const {

   Boolean result (False);

   if (_state.dataTable.get_count () == Value._state.dataTable.get_count ()) {

      result = True;
      HashTableHandleIterator it;

      dataStruct *ds (_state.dataTable.get_first (it));

      while (result && ds) {

         dataStruct *compareDs (Value._state.dataTable.lookup (it.get_hash_key ()));

         if (compareDs) {

            if (compareDs->Attr.Type == ds->Attr.Type) {

               if (ds->Attr.Type == BaseTypeString) {

                  const Int32 ElementCount (ds->get_element_count ());
                  Int32 elCount (0);
                  Boolean keepChecking (True);

                  while (keepChecking) {

                     if (elCount >= ElementCount) { keepChecking = False; }
                     else {

                        String str1, str2;

                        _state.to_string (*ds, elCount, str1);
                        Value._state.to_string (*compareDs, elCount, str2);

                        if (str1 != str2) { keepChecking = False; result = False; }
                        else { elCount++; }
                     }
                  }
               }
               else if (*ds != *compareDs) {

                  result = False;
               }

               ds = _state.dataTable.get_next (it);
            }
            else { result = False; }
         }
         else { result = False; }
      }
   }

   return result;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::Data::operator!= (const Data &Value) const { return !(*this == Value); }


/*!

\brief Logical not operator.
\details Tests if the data object has any attributes stored.
\return Returns dmz::True if the data object has any attributes stored.

*/
dmz::Boolean
dmz::Data::operator! () const { return _state.dataTable.get_count () == 0; }


//! Gets RuntimeContext stored in Data object.
void
dmz::Data::set_runtime_context (RuntimeContext *context) {

   if (context != _state.context) {

      if (_state.context) { _state.context->unref (); }
      _state.context = context;
      if (_state.context) { _state.context->ref (); }
   }
}


//! Gets RuntimeContext stored in Data object.
dmz::RuntimeContext *
dmz::Data::get_runtime_context () const { return _state.context; }


/*!

\brief Clears Data object.
\details This function deletes the elements from unbounded attributes and zeros out
the elements in fixed sized attributes.

*/
void
dmz::Data::clear () { _state.empty (); }


/*!

\brief Gets first attribute handle.
\param[in] it RuntimeIterator used to iterate over the attributes.
\return Returns the handle of the first attribute stored in the Data object.
Returns zero if the Data object is empty.

*/
dmz::Handle
dmz::Data::get_first_attribute (RuntimeIterator &it) const {

   Handle result (0);

   dataStruct *ptr = _state.dataTable.get_first (it.state.it);
   if (ptr) { result = ptr->AttrHandle; _state.cache = ptr; }

   return result;
}


/*!

\brief Gets next attribute handle.
\param[in] it RuntimeIterator used to iterate over the attributes.
\return Returns the handle of the next attribute stored in the Data object.
Returns zero if there a no more attribute handles to return.

*/
dmz::Handle
dmz::Data::get_next_attribute (RuntimeIterator &it) const {

   Handle result (0);

   dataStruct *ptr = _state.dataTable.get_next (it.state.it);
   if (ptr) { result = ptr->AttrHandle; _state.cache = ptr; }

   return result;
}


//! Returns number of attributes defined in the Data object.
dmz::Int32
dmz::Data::get_attribute_count () const { return _state.dataTable.get_count (); }


/*!

\brief Returns number of elements contained in an attribute.
\param[in] AttrHandle Attribute handle.
\return Returns number of element the attribute has.

*/
dmz::Int32
dmz::Data::lookup_attribute_element_count (const Handle AttrHandle) const {

   Int32 result (0);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) {

      const Int32 ElementSizeof (ds->Attr.Size);
      if (ElementSizeof) { result = ds->size / ElementSizeof; }
   }

   return result;
}


/*!

\brief Returns BaseTypeEnum of an attribute.
\param[in] AttrHandle Attribute handle.
\return Returns the attributes BaseTypeEnum.

*/
dmz::BaseTypeEnum
dmz::Data::lookup_attribute_base_type_enum (const Handle AttrHandle) const {

   BaseTypeEnum result (BaseTypeUnknown);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) { result = ds->Attr.Type; }

   return result;
}


/*!

\brief Store a dmz::Boolean in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_boolean (
      const Handle AttrHandle,
      const Int32 Element,
      const Boolean Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &BooleanAt);

   if (ds) {

      result = booleanConvert.write (Value ? 1 : 0, Element, _state.stringTable, *ds);
   }

   return result;

}


/*!

\brief Look up a dmz::Boolean from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_boolean (
      const Handle AttrHandle,
      const Int32 Element,
      Boolean &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) {

      UInt32 uvalue (0);

      if (booleanConvert.read (*ds, Element, _state.stringTable, uvalue)) {

         value = (uvalue > 0) ? True : False;
         result = True;
      }
   }

   return result;
}


/*!

\brief Store a dmz::UInt32 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_uint32 (
      const Handle AttrHandle,
      const Int32 Element,
      const UInt32 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &UInt32At);
   if (ds) { result = uint32Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::UInt32 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_uint32 (
      const Handle AttrHandle,
      const Int32 Element,
      UInt32 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = uint32Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::UInt64 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_uint64 (
      const Handle AttrHandle,
      const Int32 Element,
      const UInt64 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &UInt64At);
   if (ds) { result = uint64Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::UInt64 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_uint64 (
      const Handle AttrHandle,
      const Int32 Element,
      UInt64 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = uint64Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::Int32 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_int32 (const Handle AttrHandle, const Int32 Element, const Int32 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Int32At);
   if (ds) { result = int32Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::Int32 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_int32 (
      const Handle AttrHandle,
      const Int32 Element,
      Int32 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = int32Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::Int64 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_int64 (const Handle AttrHandle, const Int32 Element, const Int64 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Int64At);
   if (ds) { result = int64Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::Int64 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_int64 (
      const Handle AttrHandle,
      const Int32 Element,
      Int64 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = int64Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::Float32 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_float32 (
      const Handle AttrHandle,
      const Int32 Element,
      const Float32 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Float32At);
   if (ds) { result = float32Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::Float32 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_float32 (
      const Handle AttrHandle,
      const Int32 Element,
      Float32 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = float32Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::Float64 in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_float64 (
      const Handle AttrHandle,
      const Int32 Element,
      const Float64 Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Float64At);
   if (ds) { result = float64Convert.write (Value, Element, _state.stringTable, *ds); }

   return result;
}


/*!

\brief Look up a dmz::Float64 from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_float64 (
      const Handle AttrHandle,
      const Int32 Element,
      Float64 &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);
   if (ds) { result = float64Convert.read (*ds, Element, _state.stringTable, value); }

   return result;
}


/*!

\brief Store a dmz::String in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_string (
      const Handle AttrHandle,
      const Int32 Element,
      const String &Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &StringAt);

   if (ds) {

      const BaseTypeEnum Type = ds->Attr.Type;

      if (Type == BaseTypeString) {

         UInt32 handle (ds->lookup_string_handle (Element));

         String *ptr (0);

         if (!_state.stringTable) { _state.stringTable = new stringStruct; }

         if (_state.stringTable) {

            if (!handle) {

               handle = _state.stringTable->get_next_handle ();

               if (handle) {

                  if (ds->store_string_handle (Element, handle)) {

                     ptr = new String;

                     if (!_state.stringTable->table.store (handle, ptr)) {

                        delete ptr; ptr = 0; handle = 0;
                     }
                  }
                  else { handle = 0; }
               }
            }
            else { ptr = _state.stringTable->table.lookup (handle); }

            if (ptr) { *ptr = Value; result = True; }
         }
      }
      else if (Type == BaseTypeFloat64) {

         const Float64 Converted (string_to_float64 (Value));
         result = float64Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeFloat32) {

         const Float32 Converted (string_to_float32 (Value));
         result = float32Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeInt32) {

         const Int32 Converted (string_to_int32 (Value));
         result = int32Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeUInt32) {

         const UInt32 Converted (string_to_uint32 (Value));
         result = uint32Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeInt64) {

         const Int64 Converted (string_to_int64 (Value));
         result = int64Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeUInt64) {

         const UInt64 Converted (string_to_uint64 (Value));
         result = uint64Convert.write (Converted, Element, _state.stringTable, *ds);
      }
      else if (Type == BaseTypeBoolean) {

         const UInt32 Converted (string_to_boolean (Value) ? 1 : 0);
         result = booleanConvert.write (Converted, Element, _state.stringTable, *ds);
      }
   }

   return result;
}


/*!

\brief Store a dmz::String in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Type BaseTypeEnum specifies the attributes native base type.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_string (
      const Handle AttrHandle,
      const Int32 Element,
      const BaseTypeEnum Type,
      const String &Value) {

   if (!_state.get_data (AttrHandle)) {

      const attrStruct *Attr (&StringAt);

      if (Type == BaseTypeBoolean) { Attr = &BooleanAt; }
      else if (Type == BaseTypeUInt32) { Attr = &UInt32At; }
      else if (Type == BaseTypeUInt64) { Attr = &UInt64At; }
      else if (Type == BaseTypeInt32) { Attr = &Int32At; }
      else if (Type == BaseTypeInt64) { Attr = &Int64At; }
      else if (Type == BaseTypeFloat32) { Attr = &Float32At; }
      else if (Type == BaseTypeFloat64) { Attr = &Float64At; }

      _state.get_data (AttrHandle, Attr);
   }

   return store_string (AttrHandle, Element, Value);
}


/*!

\brief Look up a dmz::String from an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be returned.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_string (
      const Handle AttrHandle,
      const Int32 Element,
      String &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) {

      const BaseTypeEnum Type = ds->Attr.Type;

      if (Type == BaseTypeString) {

         UInt32 strHandle (ds->lookup_string_handle (Element));

         if (strHandle && _state.stringTable) {

            String *ptr = _state.stringTable->table.lookup (strHandle);

            if (ptr) { value = *ptr; result = True; }
         }

         if (!result) {

            const Int32 AtSize (ds->Attr.Size);

            if ((Element >= 0) && ((Element < AtSize) || (AtSize == 0))) {

               value.flush () << "";
               result = True;
            }
         }
      }
      else if (Type == BaseTypeFloat64) {

         Float64 converted (0.0);
         result = float64Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeFloat32) {

         Float32 converted (0.0f);
         result = float32Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeInt32) {

         Int32 converted (0);
         result = int32Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeUInt32) {

         UInt32 converted (0);
         result = uint32Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeInt64) {

         Int64 converted (0);
         result = int64Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeUInt64) {

         UInt64 converted (0);
         result = uint64Convert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << converted;
      }
      else if (Type == BaseTypeBoolean) {

         UInt32 converted (0);
         result = booleanConvert.read (*ds, Element, _state.stringTable, converted);
         value.flush () << (converted > 0 ? "true" : "false");
      }
   }

   return result;
}


/*!

\brief Store a dmz::Vector in three attribute elements.
\note It takes three real elements to store one Vector. The \a Element parameter
treats the Vector as a single unit. Thus, if a Vector is stored with the \a Element
is set to 2, the start of the Vector will actually be the Float64 element of 6.
This function provides syntactic sugar for storing Vectors in a Data object.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_vector (
      const Handle AttrHandle,
      const Int32 Element,
      const Vector &Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Float64At);

   if (ds) {

      const Int32 Offset (Element * 3);

      result =
         float64Convert.write (Value.get_x (), Offset, _state.stringTable, *ds) &&
         float64Convert.write (Value.get_y (), Offset + 1, _state.stringTable, *ds) &&
         float64Convert.write (Value.get_z (), Offset + 2, _state.stringTable, *ds);
   }

   return result;
}


/*!

\brief Look up a dmz::Vector from three attribute elements.
\note It takes three real elements to store one Vector. The \a Element parameter
treats the Vector as a single unit. Thus, if a Vector is being retrieved with the
\a Element
set to 2, the start of the Vector returned will actually be the Float64 element of 6.
This function provides syntactic sugar for retrieving Vectors in a Data object.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] value Variable to be stored.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_vector (
      const Handle AttrHandle,
      const Int32 Element,
      Vector &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) {

      const Int32 Offset (Element * 3);

      Float64 x (0.0), y (0.0), z (0.0);

      if (float64Convert.read (*ds, Offset, _state.stringTable, x) &&
            float64Convert.read (*ds, Offset + 1, _state.stringTable, y) &&
            float64Convert.read (*ds, Offset + 2, _state.stringTable, z)) {

         value.set_xyz (x, y, z);
         result = True;
      }
   }

   return result;
}


/*!

\brief Store a dmz::Matrix in nine attribute elements.
\note It takes nine real elements to store one Matrix. The \a Element parameter
treats the Matrix as a single unit. Thus, if a Matrix is stored with the \a Element
set to 2, the start of the Matrix will actually be the Float64 element of 18.
This function provides syntactic sugar for storing Matrices in a Data object.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_matrix (
      const Handle AttrHandle,
      const Int32 Element,
      const Matrix &Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &Float64At);

   if (ds) {

      const Int32 Offset (Element * 9);
      Float64 array[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
      Value.to_array (array);

      result =
         float64Convert.write (array[0], Offset, _state.stringTable, *ds) &&
         float64Convert.write (array[1], Offset + 1, _state.stringTable, *ds) &&
         float64Convert.write (array[2], Offset + 2, _state.stringTable, *ds) &&
         float64Convert.write (array[3], Offset + 3, _state.stringTable, *ds) &&
         float64Convert.write (array[4], Offset + 4, _state.stringTable, *ds) &&
         float64Convert.write (array[5], Offset + 5, _state.stringTable, *ds) &&
         float64Convert.write (array[6], Offset + 6, _state.stringTable, *ds) &&
         float64Convert.write (array[7], Offset + 7, _state.stringTable, *ds) &&
         float64Convert.write (array[8], Offset + 8, _state.stringTable, *ds);
   }

   return result;
}


/*!

\brief Look up a dmz::Matrix from nine attribute elements.
\note It takes nine real elements to store one Matrix. The \a Element parameter
treats the Matrix as a single unit. Thus, if a Matrix is being retrieved with the
\a Element
set to 2, the start of the Matrix returned will actually be the Float64 element of 18.
This function provides syntactic sugar for retrieving Matrices in a Data object.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] value Variable to be stored.
\return Returns dmz::True if the element was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_matrix (
      const Handle AttrHandle,
      const Int32 Element,
      Matrix &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds) {

      const Int32 Offset (Element * 9);
      Float64 array[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

      if (float64Convert.read (*ds, Offset, _state.stringTable, array[0]) &&
            float64Convert.read (*ds, Offset + 1, _state.stringTable, array[1]) &&
            float64Convert.read (*ds, Offset + 2, _state.stringTable, array[2]) &&
            float64Convert.read (*ds, Offset + 3, _state.stringTable, array[3]) &&
            float64Convert.read (*ds, Offset + 4, _state.stringTable, array[4]) &&
            float64Convert.read (*ds, Offset + 5, _state.stringTable, array[5]) &&
            float64Convert.read (*ds, Offset + 6, _state.stringTable, array[6]) &&
            float64Convert.read (*ds, Offset + 7, _state.stringTable, array[7]) &&
            float64Convert.read (*ds, Offset + 8, _state.stringTable, array[8])) {

         value.from_array (array);

         result = True;
      }
   }

   return result;
}


/*!

\brief Store a dmz::UUID as a String in an attribute element.
\note The UUID is converted and stored as a String.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_uuid (const Handle AttrHandle, const Int32 Element, const UUID &Value) {

   return store_string (AttrHandle, Element, Value.to_string ());
}


/*!

\brief Lookup a dmz::UUID that is stored as a String in an attribute element.
\note The UUID is converted from a String.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::lookup_uuid (
      const Handle AttrHandle,
      const Int32 Element,
      UUID &value) const {

   Boolean result (False);

   String tmp;

   if (lookup_string (AttrHandle, Element, tmp)) { result = value.from_string (tmp); }

   return result;
}


/*!

\brief Store a dmz::Handle in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::store_handle (
      const Handle AttrHandle,
      const Int32 Element,
      const Handle &Value) {

   return store_uint32 (AttrHandle, Element, Value);
}


/*!

\brief Lookup a dmz::Handle that is stored in an attribute element.
\param[in] AttrHandle Attribute handle.
\param[in] Element Index of attribute element.
\param[out] value Variable to be stored.
\return Returns dmz::True if the element was successfully stored.

*/
dmz::Boolean
dmz::Data::lookup_handle (
      const Handle AttrHandle,
      const Int32 Element,
      Handle &value) const {

   return lookup_uint32 (AttrHandle, Element, value);
}


/*!

\brief Store a dmz::Mask as an unbounded attribute of dmz::UInt32 elements.
\note This function does not have an \a Element parameter. Since a mask
is unbounded, it is not possible to know in advance the number of elements required
to store it. Thus a mask always starts at element zero.
\param[in] AttrHandle Attribute handle.
\param[in] Value Variable to be stored.
\return Returns dmz::True if the mask was successfully stored.

*/
dmz::Boolean
dmz::Data::store_mask (const Handle AttrHandle, const Mask &Value) {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle, &UInt32At);

   if (ds && (ds->Attr.Type == BaseTypeUInt32)) {

      const Int32 ElementSize (ds->Attr.Size);
      const Int32 Size (Value.get_size () * ElementSize);

      if (Size > ds->size) {

         if (ds->data) { delete [](ds->data); ds->data = 0; }
         ds->size = 0;

         ds->data = new char[Size];
         if (ds->data) {

            ds->size = Size;
            memset (ds->data, '\0', ds->size);
         }
      }
      else if (ds->data) { memset (ds->data, '\0', ds->size); }

      if (ds->data && (ElementSize > 0)) {

         const Int32 Count = ds->size / ElementSize;
         UInt32 *ptr = (UInt32 *)(ds->data);

         for (Int32 ix = 0; ix < Count; ix++) {

            ptr[ix] = Value.get_sub_mask (ix);
         }

         result = True;
      }
   }

   return result;
}


/*!

\brief Lookup a dmz::Mask stored as an unbounded attribute of dmz::UInt32 elements.
\note This function does not have an \a Element parameter. Since a mask
is unbounded, it is not possible to know in advance the number of elements required
to store it. Thus a mask always starts at element zero.
\param[in] AttrHandle Attribute handle.
\param[out] value Variable to be stored.
\return Returns dmz::True if the mask was successfully retrieved.

*/
dmz::Boolean
dmz::Data::lookup_mask (const Handle AttrHandle, Mask &value) const {

   Boolean result (False);

   dataStruct *ds = _state.get_data (AttrHandle);

   if (ds && (ds->Attr.Type == BaseTypeUInt32)) {

      const Int32 ElementSize (ds->Attr.Size);

      if (ElementSize > 0) {

          const Int32 Count (ds->size / ElementSize);

          value.clear ();
          value.grow (Count);

          result = True;

          UInt32 *ptr ((UInt32 *)(ds->data));

          if (ptr) {

             for (Int32 ix = 0; ix < Count; ix++) {

                if (!value.set_sub_mask (ix, ptr[ix])) { result = False; }
             }
          }
      }
   }

   return result;
}


//! Write a Data object to the Stream.
Stream &
operator<< (Stream &stream, const Data &Value) {

      stream << "dmz::Data object" << endl;

      RuntimeContext *context (Value.get_runtime_context ());
      Definitions defs (context);

      RuntimeIterator it;

      String buffer;
      buffer.repeat (" ", 3);

      for (
            Handle handle = Value.get_first_attribute (it);
            handle;
            handle = Value.get_next_attribute (it)) {

         if (context) {

            stream << buffer << defs.lookup_named_handle_name (handle)
               << "[" << handle << "]" << " =";
         }
         else { stream << buffer << handle << " ="; }

         const Int32 Elements (Value.lookup_attribute_element_count (handle));

         for (Int32 ix = 0; ix < Elements; ix++) {

            String value;
            if (Value.lookup_string (handle, ix, value)) {

               stream << " '" << value << "'";
            }
            else { stream << " " << "<undefined element>"; }
         }
         stream << ";" << endl;
      }

   return stream;
}
