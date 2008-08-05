#ifndef DMZ_RUNTIME_OBJECT_TYPE_DOT_H
#define DMZ_RUNTIME_OBJECT_TYPE_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeConfig.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesBooleanOperator.h>

namespace dmz {

   class Config;
   class RuntimeContext;
   class TypeContext;

   class DMZ_KERNEL_LINK_SYMBOL ObjectTypeIterator {

      public:
         ObjectTypeIterator ();
         ~ObjectTypeIterator ();

         struct State;
         State &state; //!< Internal state.

      private:
         ObjectTypeIterator (const ObjectTypeIterator &);
         ObjectTypeIterator &operator= (const ObjectTypeIterator &);
   };

   class DMZ_KERNEL_LINK_SYMBOL ObjectType {

      public:
         ObjectType ();
         ObjectType (const ObjectType &Type);
         ObjectType (TypeContext *context);
         ObjectType (const String &Name, RuntimeContext *context);
         ObjectType (const Handle TypeHandle, RuntimeContext *context);
         ~ObjectType ();

         ObjectType &operator= (const ObjectType &Type);
         Boolean operator== (const ObjectType &Type) const;
         Boolean operator!= (const ObjectType &Type) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         Boolean set_type (const String &Name, RuntimeContext *context);
         Boolean set_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean is_of_type (const ObjectType &Type) const;
         Boolean is_of_exact_type (const ObjectType &Type) const;

         String get_name () const;
         Handle get_handle () const;

         ObjectType get_parent ();
         Boolean become_parent ();

         Boolean get_first_child (ObjectTypeIterator &it, ObjectType &type);
         Boolean get_next_child (ObjectTypeIterator &it, ObjectType &type);

         Config get_config () const;

         void set_type_context (TypeContext *context);
         TypeContext *get_type_context ();

      protected:
         TypeContext *_context; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL ObjectTypeSet {

      public:
         ObjectTypeSet ();
         ObjectTypeSet (const ObjectTypeSet &Value);
         ~ObjectTypeSet ();

         ObjectTypeSet &operator= (const ObjectTypeSet &Value);

         Int32 get_count () const;

         Boolean add_object_type (const ObjectType &Type);
         Boolean add_object_type (const String &Name, RuntimeContext *context);
         Boolean add_object_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean remove_object_type (const ObjectType &Type);
         Boolean remove_object_type (const String &Name, RuntimeContext *context);
         Boolean remove_object_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean contains_type (const ObjectType &Type) const;
         Boolean contains_exact_type (const ObjectType &Type) const;

         Boolean get_first (ObjectTypeIterator &it, ObjectType &type) const;
         Boolean get_next (ObjectTypeIterator &it, ObjectType &type) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   DMZ_KERNEL_LINK_SYMBOL ObjectType
   config_to_object_type (
      const String &Name,
      const Config &Source,
      const String &DefaultObjectTypeName,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL ObjectType
   config_to_object_type (
      const String &Name,
      const Config &Source,
      const ObjectType &DefaultObjectType,
      RuntimeContext *context);

   ObjectType
   config_to_object_type (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL ObjectTypeSet
   config_to_object_type_set (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);
};


inline dmz::ObjectType
dmz::config_to_object_type (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   const ObjectType EmptyType;
   return config_to_object_type (Name, Source, EmptyType, context);
}

#endif // DMZ_RUNTIME_OBJECT_TYPE_DOT_H

