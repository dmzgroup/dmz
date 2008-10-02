#include <dmzRuntimeConfig.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDataBinder.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>

namespace {

   struct baseStruct {

      const dmz::Handle AttrHandle;
      const dmz::Int32 Element;
      baseStruct *next;

      baseStruct (const dmz::UInt32 TheHandle, const dmz::Int32 TheElement) :
         AttrHandle (TheHandle),
         Element (TheElement),
         next (0) {;}

      virtual ~baseStruct () { if (next) { delete next; next = 0; } }

      virtual dmz::Boolean read (const dmz::Data &InData) = 0;
      virtual dmz::Boolean write (dmz::Data &outData) = 0;
   };

   struct uint32Struct : public baseStruct {

      dmz::UInt32 &value;

      uint32Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::UInt32 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_uint32 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_uint32 (AttrHandle, Element, value);
      }
   };

   struct uint64Struct : public baseStruct {

      dmz::UInt64 &value;

      uint64Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::UInt64 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_uint64 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_uint64 (AttrHandle, Element, value);
      }
   };

   struct int32Struct : public baseStruct {

      dmz::Int32 &value;

      int32Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Int32 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_int32 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_int32 (AttrHandle, Element, value);
      }
   };

   struct int64Struct : public baseStruct {

      dmz::Int64 &value;

      int64Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Int64 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_int64 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_int64 (AttrHandle, Element, value);
      }
   };

   struct float32Struct : public baseStruct {

      dmz::Float32 &value;

      float32Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Float32 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_float32 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_float32 (AttrHandle, Element, value);
      }
   };

   struct float64Struct : public baseStruct {

      dmz::Float64 &value;

      float64Struct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Float64 &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_float64 (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_float64 (AttrHandle, Element, value);
      }
   };

   struct stringStruct : public baseStruct {

      dmz::String &value;

      stringStruct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::String &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_string (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_string (AttrHandle, Element, value);
      }
   };

   struct vectorStruct : public baseStruct {

      dmz::Vector &value;

      vectorStruct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Vector &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_vector (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_vector (AttrHandle, Element, value);
      }
   };

   struct matrixStruct : public baseStruct {

      dmz::Matrix &value;

      matrixStruct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::Matrix &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_matrix (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_matrix (AttrHandle, Element, value);
      }
   };

   struct uuidStruct : public baseStruct {

      dmz::UUID &value;

      uuidStruct (
         const dmz::Handle AttrHandle,
         const dmz::Int32 Element,
         dmz::UUID &theValue) :
         baseStruct (AttrHandle, Element),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_uuid (AttrHandle, Element, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_uuid (AttrHandle, Element, value);
      }
   };

   struct maskStruct : public baseStruct {

      dmz::Mask &value;

      maskStruct (
         const dmz::Handle AttrHandle,
         dmz::Mask &theValue) :
         baseStruct (AttrHandle, 0),
         value (theValue) {;}

      dmz::Boolean read (const dmz::Data &InData) {

         return InData.lookup_mask (AttrHandle, value);
      }

      dmz::Boolean write (dmz::Data &outData) {

         return outData.store_mask (AttrHandle, value);
      }
   };
};

/*!

\class dmz::DataBinder
\ingroup Runtime
\brief Binds external variables for reading and writing Data objects.
\details This class binds external variable to Data attribute elements. The
DataBinder may then be used to read or write a Data object of the same type
used to bind the variables. The read Data objects values are automatically stored
in the bound variables. When writing to a Data object, the values of the bound
variables are used to store values in the Data object.
\sa dmz::Data

*/

struct dmz::DataBinder::State {

   Definitions defs;
   baseStruct *head;
   Log *log;

   State (
         RuntimeContext *context,
         Log *theLog) :
         defs (context),
         head (0),
         log (theLog) {
   }

   ~State () { if (head) { delete head; head = 0; } }
};


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the Log to be used for logging. May be NULL.

*/
dmz::DataBinder::DataBinder (
      RuntimeContext *context,
      Log *log) :
      _state (*(new State (context, log))) {
}


//! Destructor.
dmz::DataBinder::~DataBinder () { delete &_state; }


/*!

\brief Reads Data object and stores read values in the bound external values.
\param[in] InData Data object to read.
\return Returns dmz::True if the Data object was successfully read.

*/
dmz::Boolean
dmz::DataBinder::read_data (const Data &InData) {

   Boolean result (_state.head ? True : False);

   baseStruct *bs (_state.head);

   while (bs) {

      if (!bs->read (InData)) { result = False; }
      bs = bs->next;
   }

   return result;
}


/*!

\brief Writes values of the bound external variables to the Data object.
\param[out] outData Data object to write to.
\return Returns dmz::True if the Data object was successfully written to.
\note If the Data object is not initialized or initialized with a different data type
it is reset with the data type context used to define the binder.

*/
dmz::Boolean
dmz::DataBinder::write_data (Data &outData) const {

   Boolean result (_state.head ? True : False);

   baseStruct *bs (_state.head);

   while (bs) {

      if (!bs->write (outData)) { result = False; }
      bs = bs->next;
   }

   return result;
}


//! Not currently supported.
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Boolean &value) {

   Boolean result (False);

   return result;
}


/*!

\brief Binds name to external UInt32 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, UInt32 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   uint32Struct *ptr (new uint32Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external UInt64 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, UInt64 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   uint64Struct *ptr (new uint64Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Int32 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Int32 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   int32Struct *ptr (new int32Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Int64 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Int64 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   int64Struct *ptr (new int64Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Float32 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Float32 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   float32Struct *ptr (new float32Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Float64 variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Float64 &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   float64Struct *ptr (new float64Struct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external String variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, String &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   stringStruct *ptr (new stringStruct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Vector variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Vector &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   vectorStruct *ptr (new vectorStruct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Matrix variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, Matrix &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   matrixStruct *ptr (new matrixStruct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external UUID variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] Element Attribute element
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, const Int32 Element, UUID &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   uuidStruct *ptr (new uuidStruct (AttrHandle, Element, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}


/*!

\brief Binds name to external Mask variable.
\param[in] Name String containing name of the attributes NameHandle
\param[in] value Reference to external variable to bind.
\return Returns dmz::True if the external variable is successfully bound.

*/
dmz::Boolean
dmz::DataBinder::bind (const String &Name, Mask &value) {

   Boolean result (False);

   const Handle AttrHandle (_state.defs.create_named_handle (Name));
   maskStruct *ptr (new maskStruct (AttrHandle, value));
   if (ptr) { ptr->next = _state.head; _state.head = ptr; result = true; }

   return result;
}

