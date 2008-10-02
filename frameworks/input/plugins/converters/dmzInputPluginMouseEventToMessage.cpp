#include <dmzInputEventMasks.h>
#include <dmzInputEventMouse.h>
#include "dmzInputPluginMouseEventToMessage.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDataBinder.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

#include <qdb.h>
static dmz::qdb out;

/*!

\class dmz::InputPluginMouseEventToMessage
\ingroup Input
\brief Converters mouse events to messages.
\details The Plugin is a state machine that uses the values of the dmz::InputEventMouse
object to move from state to state. Each state is able to send messages when predefined
conditions are met. The state machine XML format:
\code
<dmz>
<dmzInputPluginMouseEventToMessage>
   <input>
      <!-- As many channels and source may be listed as needed -->
      <channel name="Channel Name"/>
      <source name="Event Source Name"/>
   </input>
   <!--
      Attribute: "start" specifies the name of the starting state.
   -->
   <converter start="first">
      <!--
         A converter may have multiple states.
         Attribute: "name" specifies the name of the state.
         Attribute: "type" specifies the type of state.
         Only type "basic" is supported and is the default.
      -->
      <state name="first" type="basic">
         <!--
            A state may have multiple conditions. Each condition is evaluated until
            The defined conditions are met and a next state is defined by the condition.
            Attribute: "action" specifies the mouse action that triggers the state.
            Actions are: "pressed", "released", "moved", and "any".
            Attribute: "button" specifies the mouse button number.
            The "button" attribute is only used with "pressed" and "released" actions.
            Attribute: "result" specifies the result of the action.
            Results are: "object", "position", and "none".
            Attribute: "message" specifies the message that should be sent if the
            action and result conditions are met.
            Attribute: "next" specifies the name of the next state if the action and
            result conditions are met.
         -->
         <condition
               action="pressed"
               button="1"
               result="object"
               message="MessageName"
               next ="second">
            <!--
               A condition may have multiple targets defined.
               Attribute: "name" specifies the name of the target to send the message.
            -->
            <target name="Target Name"/>
            <!--
               A condition may have multiple attributes defined.
               Attribute: "type" specifies the attribute type to store in Data object.
               Supported types are: "object" and "position".
               Attribute: "name" specifies the name of the named handle to use when
               storing the attribute in the Data object.
            -->
            <attribute type="object" name="object"/>
         </condition>
      </state>
      <state name="second">
         <condition action="released" button="1" next="first"/>
      </state>
   </converter>
</dmzInputPluginMouseEventToMessage>
</dmz>
\endcode
*/

using namespace dmz;

namespace {

typedef RenderModulePick * PickPtr;
typedef InputPluginMouseEventToMessage::ConverterStruct cstruct;
typedef InputPluginMouseEventToMessage::AttrStruct astruct;

enum MouseConditionEnum { Pressed, Released, Moved, Any };
enum ResultConditionEnum { Object, Position, None };

static MouseConditionEnum
local_config_to_mouse_condition (
      const String Name,
      Config &config,
      Log &log) {

   MouseConditionEnum result (Any);

   const String ConditionOrig (config_to_string (Name, config));
   const String Condition (ConditionOrig.get_lower ());

   if (!Condition) {

      log.debug << "Using default button condition type: Pressed" << endl;
   }
   else if (Condition == "pressed") { result = Pressed; }
   else if (Condition == "released") { result = Released; }
   else if (Condition == "moved") { result = Moved; }
   else if (Condition == "any") { result = Any; }
   else { log.error << "Unknown button condition type: " << ConditionOrig << endl; }

   return result;
}


static ResultConditionEnum
local_config_to_result_condition (
      const String Name,
      Config &config,
      Log &log) {

   ResultConditionEnum result (None);

   const String ConditionOrig (config_to_string (Name, config));
   const String Condition (ConditionOrig.get_lower ());

   if (!Condition) {

      log.debug << "Using default result condition type: None" << endl;
   }
   else if (Condition == "object") { result = Object; }
   else if (Condition == "position") { result = Position; }
   else if (Condition == "none") { result = None; }
   else { log.error << "Unknown result condition type: " << ConditionOrig << endl; }

   return result;
}

} // namespace

struct InputPluginMouseEventToMessage::AttrStruct {

   const Handle AttrHandle;
   AttrStruct *next;

   AttrStruct (const Handle TheAttrHandle) :
         AttrHandle (TheAttrHandle),
         next (0) {;}

   virtual ~AttrStruct () { if (next) { delete next; next = 0; } }

   virtual void bind (const String Name, const DataBinder &binder) = 0;
   virtual void write (const InputEventMouse &Event) = 0;

   void write_all (const InputEventMouse &Event) {

      write (Event);

      AttrStruct *current (next);

      while (current) {

         current->write (Event);
         current = current->next;
      }
   }
};

struct InputPluginMouseEventToMessage::ConverterStruct {

   const String Name;
   PickPtr &pickMod;

   ConverterStruct (const String TheName, PickPtr &ptr) :
      Name (TheName),
      pickMod (ptr) {;}

   virtual ConverterStruct *process_results (
      const InputEventMouse &Event,
      const Vector &WorldPosition,
      const Handle ObjectHandle) = 0;

   virtual ConverterStruct *process_event (const InputEventMouse &Event) {

      Vector pos;
      Handle obj (0);

      ConverterStruct *result (0);

      if (pickMod) {

         pickMod->source_to_world (
            Event.get_source_handle (),
            Event.get_mouse_x (),
            Event.get_mouse_y (),
            pos,
            obj);

         pos.set_xyz (pos.get_x (), 0.0, pos.get_y ());

         result = process_results (Event, pos, obj);
      }

      return result;
   }
};


namespace {

struct ConditionStruct {

   // Both button and result must be true for condition to pass.
   const UInt32 Button;
   const MouseConditionEnum MouseCondition;
   const ResultConditionEnum ResultCondition;
   const Message Msg;
   const HandleContainer Targets;
   DataBinder binder;
   astruct *attrList;
   Vector position;
   Handle objectHandle;
   ConditionStruct *nextCondition;
   String nextStateName;
   cstruct *nextState;

   ConditionStruct (
         const UInt32 TheButton,
         const MouseConditionEnum TheMouseCondition,
         const ResultConditionEnum TheResultCondition,
         const Message &TheMessage,
         const HandleContainer &TheTargets,
         const String &NextStateName,
         RuntimeContext *context) :
         Button (TheButton),
         MouseCondition (TheMouseCondition),
         ResultCondition (TheResultCondition),
         Msg (TheMessage),
         Targets (TheTargets),
         binder (context),
         attrList (0),
         objectHandle (0),
         nextCondition (0),
         nextStateName (NextStateName),
         nextState (0) {;}

   ~ConditionStruct () { if (nextCondition) { delete nextCondition; nextCondition = 0; } }
};


struct ConverterBasicStruct : cstruct {

   ConditionStruct *list;

   ConverterBasicStruct (
         const String &Name,
         PickPtr &ptr) :
         ConverterStruct (Name, ptr),
         list (0) {;}

   ~ConverterBasicStruct () { if (list) { delete list; list = 0; } }

   virtual cstruct *process_results (
      const InputEventMouse &Event,
      const Vector &WorldPosition,
      const Handle ObjectHandle);
};


cstruct *
ConverterBasicStruct::process_results (
      const InputEventMouse &Event,
      const Vector &WorldPosition,
      const Handle ObjectHandle) {

   cstruct *result (0);

   ConditionStruct *current (list);

   while (!result && current) {

      Boolean passed = True;

      // Test if the button state is correct for sending a message.
      if (current->MouseCondition == Any) {

         // Do nothing
      }
      else if (current->MouseCondition == Moved) {

         passed =
            ((Event.get_mouse_delta_x () != 0) || (Event.get_mouse_delta_y () != 0));
      }
      else {

         const UInt32 Button (current->Button);

         const Boolean IsPressed (Event.is_button_pressed (Button));
         const Boolean IsChanged (Event.is_button_changed (Button));

         if ((current->MouseCondition == Pressed) && (!IsPressed || !IsChanged)) {

            passed = False;
         }
         else if ((current->MouseCondition == Released) && (IsPressed || !IsChanged)) {

            passed = False;
         }
      }

      // Test if the result is correct for sending a message.
      if (passed) {

         if ((current->ResultCondition == Object) && !ObjectHandle) { passed = False; }
      }

      if (passed) {

         const Message &Msg (current->Msg);

         if (Msg) {

// out << "Sending message: " << Message.get_name () << endl;

            // Set values so the DataBinder can write their values to the data object.
            current->position = WorldPosition;
            current->objectHandle = ObjectHandle;

            // Set any values from the Mouse Event that need to be stored in the
            // Data object.
            if (current->attrList) { current->attrList->write_all (Event); }

            Data data;

            current->binder.write_data (data);

            Data *dataPtr (data.get_attribute_count () > 0 ? &data : 0);

            const HandleContainer &Targets (current->Targets);

            Handle target = Targets.get_first ();

            while (target) {

               Msg.send (target, dataPtr, 0);

               target = Targets.get_next ();
            }
         }

         result = current->nextState;
      }

      current = current->nextCondition;
   }

   if (!result) { result = this; }

   return result;
}

}; // namespace


//! \cond
dmz::InputPluginMouseEventToMessage::InputPluginMouseEventToMessage (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _start (0),
      _current (0),
      _pickMod (0),
      _defs (Info),
      _activeCount (0),
      _log (Info) {

   _init (local);
}


dmz::InputPluginMouseEventToMessage::~InputPluginMouseEventToMessage () {

   _current = 0;
   _start = 0;
   _table.empty ();
}


// Plugin Interface
void
dmz::InputPluginMouseEventToMessage::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_pickMod) { _pickMod = RenderModulePick::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_pickMod && (_pickMod == RenderModulePick::cast (PluginPtr))) { _pickMod = 0; }
   }
}


// Input Observer Interface
void
dmz::InputPluginMouseEventToMessage::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) { _activeCount++; } else { _activeCount--; }

   if (0 == _activeCount) {

      _current = 0;

      _send (_deactivateMessage, _deactivateTargetTable);
   }
   else if (1 == _activeCount) {

      _send (_activateMessage, _activateTargetTable);
   }
}


void
dmz::InputPluginMouseEventToMessage::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

   if (_sourceTable.contains (Value.get_source_handle ())) {

      if (!_current) { _current = _start; }

      if (_current) { _current = _current->process_event (Value); }
   }
}


void
dmz::InputPluginMouseEventToMessage::_send (
      const Message &Msg,
      HandleContainer &targets) {

   if (Msg) {

      Handle target (targets.get_first ());

      while (target) {

         Msg.send (target, 0, 0);
         target = targets.get_next ();
      }
   }
}


dmz::InputPluginMouseEventToMessage::AttrStruct *
dmz::InputPluginMouseEventToMessage::_create_attributes (
      Config &config,
      DataBinder &binder,
      Vector *position,
      Handle *object) {

   AttrStruct *result (0);

   Config attrList;

   if (config.lookup_all_config ("attribute", attrList)) {

      ConfigIterator it;
      Config attrConfig;

      AttrStruct *current;

      while (attrList.get_next_config (it, attrConfig)) {

         AttrStruct *next (0);

         const String Type (config_to_string ("type", attrConfig).get_lower ());
         const String Name (config_to_string ("name", attrConfig).get_lower ());
         const Int32 Index (config_to_int32 ("index", attrConfig, 0));

         if (Type == "position") {

            if (position) { binder.bind (Name, Index, *position); }
         }
         else if (Type == "object") {

            if (object) { binder.bind (Name, Index, *object); }
         }

         if (next) {

            if (!current) { result = current = next; }
            else { current->next = current; current = next; }
         }
      }
   }

   return result;
}


void
dmz::InputPluginMouseEventToMessage::_get_targets (
      const String &Name,
      Config &config,
      HandleContainer &targets) {

   Config targetList;

   if (config.lookup_all_config (Name, targetList)) {

      ConfigIterator it;
      Config targetConfig;

      while (targetList.get_next_config (it, targetConfig)) {

         const String TargetName (config_to_string ("name", targetConfig));

         if (TargetName) {

            const Handle Target (_defs.create_named_handle (TargetName));
            targets.add_handle (Target);
         }
         else { _log.error << "Unable to add unnamed target" << endl; }
      }
   }
   else if ((&targets) != (&_targetTable)) { targets = _targetTable; }
}


dmz::InputPluginMouseEventToMessage::ConverterStruct *
dmz::InputPluginMouseEventToMessage::_create_converter_basic (
      HashTableStringTemplate<ConverterStruct> &nameTable,
      ConfigIterator &it,
      Config &converterConfig,
      Config &config) {

   const String Name (config_to_string ("name", config));

   ConverterBasicStruct *result (new ConverterBasicStruct (
         Name,
         _pickMod));

   if (!_table.store (Name, result)) { delete result; result = 0; }

   if (!nameTable.store (Name, result)) { delete result; result = 0; }

   if (result) {

      _log.debug << "Creating state: " << Name << endl;

      Config conditionList;
      config.lookup_all_config ("condition", conditionList);

      ConfigIterator it;
      Config condition;

      ConditionStruct *current (0);

      while (conditionList.get_next_config (it, condition)) {

         const UInt32 Button (config_to_uint32 ("button", condition, 0));

         const MouseConditionEnum MouseCondition (
            local_config_to_mouse_condition ("action", condition, _log));

         const ResultConditionEnum ResultCondition (
            local_config_to_result_condition ("result", condition, _log));

         Message message (config_create_message_type (
            "message",
            condition,
            "",
            get_plugin_runtime_context ()));

         HandleContainer targets;

         if (message) { _get_targets ("target", condition, targets); }

         const String NextStateName (config_to_string ("next", condition));

         ConditionStruct *next (new ConditionStruct (
            Button,
            MouseCondition,
            ResultCondition,
            message,
            targets,
            NextStateName,
            get_plugin_runtime_context ()));

         if (next) {

            next->attrList = _create_attributes (
               condition,
               next->binder,
               &(next->position),
               &(next->objectHandle));

            if (!current) { result->list = current = next; }
            else { current->nextCondition = next; current = next; }
         }
      }
   }

   if (result) {

     _create_converter (nameTable, it, converterConfig);

     ConditionStruct *current (result->list);

     while (current) {

        if (current->nextStateName) {

           current->nextState = nameTable.lookup (current->nextStateName);

           if (!current->nextState) {

              _log.error << "Unable to find next state named: " << current->nextStateName
                 << endl;
           }

           current->nextStateName.empty ();
        }

        current = current->nextCondition;
     }
   }

   return result;
}


dmz::InputPluginMouseEventToMessage::ConverterStruct *
dmz::InputPluginMouseEventToMessage::_create_converter (
      HashTableStringTemplate<ConverterStruct> &nameTable,
      ConfigIterator &it,
      Config &converterConfig) {

   ConverterStruct *result (0);

   Config config;

   if (converterConfig.get_next_config (it, config)) {

      const String TypeOrig (config_to_string ("type", config, "basic"));
      const String Type (TypeOrig.get_lower ());

      if (!Type || Type == "basic") {

         result = _create_converter_basic (nameTable, it, converterConfig, config);
      }
      else {

         _log.error << "Unknown converter sub component type: " << TypeOrig << endl;
      }
   }

   return result;
}


dmz::InputPluginMouseEventToMessage::ConverterStruct *
dmz::InputPluginMouseEventToMessage::_create_converter (Config &local) {

   ConverterStruct *result (0);

   Config converterConfig;
   local.lookup_all_config_merged ("converter", converterConfig);

   HashTableStringTemplate<ConverterStruct> nameTable;
   ConfigIterator it;
   result = _create_converter (nameTable, it, converterConfig);

   const String StartName (config_to_string ("start", converterConfig));

   if (StartName) {

      ConverterStruct *start (nameTable.lookup (StartName));

      if (start) { result = start; }
      else { _log.error << "Unable to find starting state: " << StartName << endl; }
   }

   return result;
}


void
dmz::InputPluginMouseEventToMessage::_init (Config &local) {

   Config sourceList;

   if (local.lookup_all_config ("input.source", sourceList)) {

      ConfigIterator it;
      Config sourceConfig;

      while (sourceList.get_next_config (it, sourceConfig)) {

         const String SourceName (config_to_string ("name", sourceConfig));

         if (SourceName) {

            _sourceTable.add_handle (_defs.create_named_handle (SourceName));
         }
      }
   }

   if (_sourceTable.get_count ()) {

      init_input_channels (
         local,
         InputEventChannelStateMask | InputEventMouseMask,
         &_log);

      Config targetList;

      _get_targets ("target", local, _targetTable);

      _start = _create_converter (local);
   }
   else {

      _log.error << "No input source defined." << endl;
   }

   _activateMessage = config_create_message_type (
      "activate.message",
      local,
      "",
      get_plugin_runtime_context ());

   if (_activateMessage) {

      _get_targets ("activate.target", local, _activateTargetTable);
   }

   _deactivateMessage = config_create_message_type (
      "deactivate.message",
      local,
      "",
      get_plugin_runtime_context ());

   if (_deactivateMessage) {

      _get_targets ("deactivate.target", local, _deactivateTargetTable);
   }
}

//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginMouseEventToMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginMouseEventToMessage (Info, local);
}

};
