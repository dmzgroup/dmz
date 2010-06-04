#include <dmzObjectCalc.h>
#include "dmzObjectCalcPrivate.h"
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMath.h>

//! \cond
dmz::ObjectAttrOperator::ObjectAttrOperator (const ObjectAttrOperatorEnum Operator) :
      _Op (Operator),
      _argList (0) {;}


dmz::ObjectAttrOperator::~ObjectAttrOperator () {

   if (_argList) { delete _argList; _argList = 0; }
}


void
dmz::ObjectAttrOperator::store_object_module (ObjectModule *module) {

   ArgStruct *current (_argList);

   while (current) {

      current->calc.store_object_module (module);
      current = current->next;
   }
}


dmz::Float64
dmz::ObjectAttrOperator::calculate (const Handle ObjectHandle) {

   ArgStruct *current (_argList);

   Float64 result (current ? current->calc.calculate (ObjectHandle) : 0.0);

   current = (current ? current->next : 0);

   while (current) {

      const Float64 Value (current->calc.calculate (ObjectHandle));

      if (_Op == ObjectAttrOperatorAdd) { result += Value; }
      else if (_Op == ObjectAttrOperatorSubtract) { result -= Value; }
      else if (_Op == ObjectAttrOperatorMultiply) { result *= Value; }
      else if (_Op == ObjectAttrOperatorDivide) {

         if (!is_zero64 (Value)) { result = result / Value; }
      }

      current = current->next;
   }

   return result;
}


void
dmz::ObjectAttrOperator::add_calculator (ObjectAttributeCalculator &calc) {

   ArgStruct *ptr (new ArgStruct (calc));

   if (ptr) {

      if (!_argList) { _argList = ptr; }
      else {

         ArgStruct *current (_argList);

         while (current->next) { current = current->next; }

         current->next = ptr;
      }
   }
}


dmz::ObjectAttrValue::ObjectAttrValue (const Handle TheAttrHandle) :
      _AttrHandle (TheAttrHandle),
      _objMod (0) {;}


dmz::ObjectAttrValue::~ObjectAttrValue () {;}


void
dmz::ObjectAttrValue::store_object_module (ObjectModule *module) { _objMod = module; }


dmz::Float64
dmz::ObjectAttrValue::calculate (const Handle ObjectHandle) {

   Float64 result (0.0);

   if (_objMod) { _objMod->lookup_scalar (ObjectHandle, _AttrHandle, result); }

   return result;
}

static const dmz::UInt32 SuperMask = 0x01;
static const dmz::UInt32 SubMask = 0x01;

dmz::ObjectAttrLinkCount::ObjectAttrLinkCount (
      const Handle TheAttrHandle,
      const UInt32 LinkMask) :
      _AttrHandle (TheAttrHandle),
      _LinkMask (LinkMask),
      _objMod (0) {;}


dmz::ObjectAttrLinkCount::~ObjectAttrLinkCount () {;}


void
dmz::ObjectAttrLinkCount::store_object_module (ObjectModule *module) {

   _objMod = module;
}


dmz::Float64
dmz::ObjectAttrLinkCount::calculate (const Handle ObjectHandle) {

   dmz::Float64 result (0.0);

   if (_objMod) {

      if (_LinkMask & SuperMask) {

         HandleContainer container;

         if (_objMod->lookup_super_links (ObjectHandle, _AttrHandle, container)) {

            result += container.get_count ();
         }
      }

      if (_LinkMask & SubMask) {

         HandleContainer container;

         if (_objMod->lookup_sub_links (ObjectHandle, _AttrHandle, container)) {

            result += container.get_count ();
         }
      }
   }

   return result;
}


dmz::ObjectAttrObjectTypeCount::ObjectAttrObjectTypeCount (
      const ObjectTypeSet &Set) :
      _Set (Set),
      _objMod (0) {;}


dmz::ObjectAttrObjectTypeCount::~ObjectAttrObjectTypeCount () {;}


void
dmz::ObjectAttrObjectTypeCount::store_object_module (ObjectModule *module) {

   _objMod = module;
}


dmz::Float64
dmz::ObjectAttrObjectTypeCount::calculate (const Handle ObjectHandle) {

   dmz::Float64 result (0.0);

   if (_objMod) {

      if (_Set.contains_type (_objMod->lookup_object_type (ObjectHandle))) {

         result = 1.0;
      }
   }

   return result;
}


dmz::ObjectAttrConst::ObjectAttrConst (const Float64 ConstValue) :
      _ConstValue (ConstValue) {;}


dmz::ObjectAttrConst::~ObjectAttrConst () {;}


void
dmz::ObjectAttrConst::store_object_module (ObjectModule *module) {;}


dmz::Float64
dmz::ObjectAttrConst::calculate (const Handle ObjectHandle) { return _ConstValue; }

using namespace dmz;

namespace {

static ObjectAttrOperatorEnum
local_string_to_operator (const String TypeName) {

   ObjectAttrOperatorEnum op (ObjectAttrOperatorAdd);

   const String LowerTypeName (TypeName.get_lower ());

   if ((LowerTypeName == "add")) { op = ObjectAttrOperatorAdd; }
   else if ((LowerTypeName == "subtract")) { op = ObjectAttrOperatorSubtract; }
   else if ((LowerTypeName == "multiply")) { op = ObjectAttrOperatorMultiply; }
   else if ((LowerTypeName == "divide")) { op = ObjectAttrOperatorDivide; }

   return op;
}


static ObjectAttributeCalculator *
local_build_calc_tree (
      Config &source,
      Definitions &defs,
      RuntimeContext *context,
      Log *log) {

   ObjectAttributeCalculator *result (0);

   const String Type = config_to_string ("type", source);

   if (Type == "operator") {

      const ObjectAttrOperatorEnum Op (
         local_string_to_operator (config_to_string ("mode", source)));

      ObjectAttrOperator *opResult (new ObjectAttrOperator (Op));

      if (opResult) {

         ConfigIterator it;
         Config arg;

         while (source.get_next_config (it, arg)) {

            ObjectAttributeCalculator *calc (
               local_build_calc_tree (arg, defs, context, log));

            if (calc) { opResult->add_calculator (*calc); }
         }
      }

      result = opResult;
   }
   else if (Type == "attribute") {

      const String AttrName (config_to_string ("name", source));

      if (AttrName) {

         const Handle AttrHandle (defs.create_named_handle (AttrName));

         result = new ObjectAttrValue (AttrHandle);
      }
      else if (log) {

         log->error << "Object Attribute Calculator of type Value missing attribute "
            << "name." << endl;
      }
   }
   else if (Type == "const") {

      const Float64 Value (config_to_float64 ("value", source, 1.0));
      result = new ObjectAttrConst (Value);
   }
   else if (Type == "link") {

      const String AttrName (config_to_string ("name", source));

      if (AttrName) {

         const Handle AttrHandle (defs.create_named_handle (AttrName));

         UInt32 mask (0);

         if (config_to_boolean ("super", source, False)) { mask |= SuperMask; }
         if (config_to_boolean ("sub", source, False)) { mask |= SubMask; }

         result = new ObjectAttrLinkCount (AttrHandle, mask);
      }
      else if (log) {

         log->error << "Object Attribute Calculator of type Link Count missing "
            << "attribute name." << endl;
      }
   }
   else if (Type == "object-type") {

      const String AttrName (config_to_string ("name", source));

      if (AttrName) {

         const ObjectTypeSet Set = config_to_object_type_set (source, context);

         result = new ObjectAttrObjectTypeCount (Set);
      }
      else if (log) {

         log->error << "Object Attribute Calculator of type Link Count missing "
            << "attribute name." << endl;
      }
   }
   else if (log) { log->error << "Unknown calculator type: " << Type << endl; }

   return result;
}

};

//! \endcond

/*!

\class dmz::ObjectAttributeCalculator
\ingroup Object
\brief Interface for calculating a scalar based on multiple object attribute values.
\sa dmz::config_to_object_attribute_calculator ()

\fn dmz::ObjectAttributeCalculator::ObjectAttributeCalculator ()
\brief Constructor.

\fn dmz::ObjectAttributeCalculator::~ObjectAttributeCalculator ()
\brief Destructor.

\fn void dmz::ObjectAttributeCalculator::store_object_module (ObjectModule *module)
\brief Stores the ObjectModule in the calculator tree.

\fn dmz::Float64 dmz::ObjectAttributeCalculator::calculate (const Handle ObjectHandle)
\brief Performs calculation on specified object handle.

*/

/*!

\brief Creates an Object Attribute Calculator tree.
\ingroup Object
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to dmz::Log to use for log reporting.
\return Returns a pointer to the created ObjectAttributeCalculator tree.

*/
dmz::ObjectAttributeCalculator *
dmz::config_to_object_attribute_calculator (
      const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Log *log) {

   Definitions defs (context);

   ObjectAttributeCalculator *result (0);

   Config root;
   String realName ("calculator");

   if (Name) { realName = Name + "." + realName; }

   Source.lookup_config (realName, root);

   if (!root.is_empty ()) { result = local_build_calc_tree (root, defs, context, log); }
   else if (log) { log->error << "Calculator Config object is empty." << endl; }

   return result;
}

