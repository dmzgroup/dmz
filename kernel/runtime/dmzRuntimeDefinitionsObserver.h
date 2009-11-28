#ifndef DMZ_RUNTIME_DEFINITIONS_OBSERVER_DOT_H
#define DMZ_RUNTIME_DEFINITIONS_OBSERVER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

class Mask;
class ObjectType;
class EventType;
class PluginInfo;
class RuntimeContext;

//! \addtogroup Runtime
//! @{

//! \brief Activate define_named_handle callback in dmz::DefinitionsObserver
//! \details Defined in dmzRuntimeDefinitionsObserver.h
//! \sa dmz::DefinitionsObserver::set_definitions_observer_callback_mask
const UInt32 RuntimeNamedHandleMask = 0x01;
//! \brief Activate define_state callback in dmz::DefinitionsObserver
//! \details Defined in dmzRuntimeDefinitionsObserver.h
//! \sa dmz::DefinitionsObserver::set_definitions_observer_callback_mask
const UInt32 RuntimeStateMask = 0x02;
//! \brief Activate define_object_type callback in dmz::DefinitionsObserver
//! \details Defined in dmzRuntimeDefinitionsObserver.h
//! \sa dmz::DefinitionsObserver::set_definitions_observer_callback_mask
const UInt32 RuntimeObjectTypeMask = 0x04;
//! \brief Activate define_event_type callback in dmz::DefinitionsObserver
//! \details Defined in dmzRuntimeDefinitionsObserver.h
//! \sa dmz::DefinitionsObserver::set_definitions_observer_callback_mask
const UInt32 RuntimeEventTypeMask = 0x08;
//! \brief Activate all callbacks in dmz::DefinitionsObserver
//! \details Defined in dmzRuntimeDefinitionsObserver.h
//! \sa dmz::DefinitionsObserver::set_definitions_observer_callback_mask
const UInt32 RuntimeAllMask = 0xFF;

//! @}

class DMZ_KERNEL_LINK_SYMBOL DefinitionsObserver {

   public:
      Handle get_definitions_observer_handle () const;
      String get_definitions_observer_name () const;

      UInt32 get_definitions_observer_callback_mask ();
      UInt32 set_definitions_observer_callback_mask (const UInt32 Mask);

      virtual void define_named_handle (const Handle TheHandle, const String &Name);
      virtual void define_state (const Mask &TheState, const String &Name);
      virtual void define_object_type (const ObjectType &Type);
      virtual void define_event_type (const EventType &Type);

   protected:
      DefinitionsObserver (RuntimeContext *context);
      DefinitionsObserver (const PluginInfo &Info);
      virtual ~DefinitionsObserver ();

   private:
      struct State;
      State &__state;
};

};

#endif // DMZ_RUNTIME_DEFINITIONS_OBSERVER_DOT_H
