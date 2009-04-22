#include "dmzInputModuleBasic.h"
#include <dmzRuntimeInit.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimeRTTI.h>
#include <dmzInputConsts.h>
#include <dmzInputMaskConsts.h>
#include <dmzRuntimeData.h>
#include <dmzTypesMask.h>

#include <ctype.h>

/*!

\class dmz::InputModuleBasic
\ingroup Input
\brief Basic InputModule implementation.
\details This provides a basic implementation of the InputModule. At this time the
InputModuleBasic has no configuration parameters.
\sa InputModule

*/

using namespace dmz;

namespace {

static const Mask LocalStateMask (0, InputEventChannelStateConst);
static const Mask LocalAxisMask (0, InputEventAxisConst);
static const Mask LocalButtonMask (0, InputEventButtonConst);
static const Mask LocalKeyMask (0, InputEventKeyConst);
static const Mask LocalMouseMask (0, InputEventMouseConst);
static const Mask LocalSwitchMask (0, InputEventSwitchConst);
static const Mask LocalDataMask (0, InputEventDataConst);

typedef InputModuleBasic::EventQueStruct estruct;

struct AxisQueStruct : public estruct {

   const InputEventAxis Event;
   AxisQueStruct (const InputEventAxis &TheEvent) : Event (TheEvent) {;}

   virtual void send_event (InputModule &module) { module.send_axis_event (Event); }
};

struct ButtonQueStruct : public estruct {

   const InputEventButton Event;
   ButtonQueStruct (const InputEventButton &TheEvent) : Event (TheEvent) {;}

   virtual void send_event (InputModule &module) { module.send_button_event (Event); }
};

struct SwitchQueStruct : public estruct {

   const InputEventSwitch Event;
   SwitchQueStruct (const InputEventSwitch &TheEvent) : Event (TheEvent) {;}

   virtual void send_event (InputModule &module) { module.send_switch_event (Event); }
};

struct KeyQueStruct : public estruct {

   const InputEventKey Event;
   KeyQueStruct (const InputEventKey &TheEvent) : Event (TheEvent) {;}

   virtual void send_event (InputModule &module) { module.send_key_event (Event); }
};

struct MouseQueStruct : public estruct {

   const InputEventMouse Event;
   MouseQueStruct (const InputEventMouse &TheEvent) : Event (TheEvent) {;}

   virtual void send_event (InputModule &module) { module.send_mouse_event (Event); }
};

struct DataQueStruct : public estruct {

   const Handle Source;
   const Data Event;

   DataQueStruct (const Handle TheSource, const Data &TheEvent) :
         Source (TheSource),
         Event (TheEvent) {;}

   virtual void send_event (InputModule &module) {

      module.send_data_event (Source, Event);
   }
};

}; // namespace


//! \cond
dmz::InputModuleBasic::InputModuleBasic (const PluginInfo &Info) :
      Plugin (Info),
      InputModule (Info),
      _inEvent (False),
      _inQue (False),
      _eventQueHead (0),
      _eventQueTail (0),
      _eventCount (0),
      _log (Info),
      _defs (Info, &_log) {;}


dmz::InputModuleBasic::~InputModuleBasic () {

   if (_eventQueHead) { delete _eventQueHead; _eventQueHead = _eventQueTail = 0; }

   HashTableHandleIterator it;

   ObsStruct *os (_obsTable.get_first (it));

   RuntimeContext *context (_PluginInfoData.get_context ());

   while (os) {

      if (InputObserver::is_valid (os->ObsHandle, context)) {

         os->obs.remove_input_module (_PluginInfoData.get_name (), *this);
      }

      os  = _obsTable.get_next (it);
   }

   _obsTable.empty ();

   _channelTable.empty ();

   _keyCache.empty ();
   _controllerCache.empty ();
}


// Plugin Interface
void
dmz::InputModuleBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      InputObserver *obs (InputObserver::cast (PluginPtr));

      if (obs) { obs->store_input_module (_PluginInfoData.get_name (), *this); }
   }
   else if (Mode == PluginDiscoverRemove) {

      InputObserver *obs (InputObserver::cast (PluginPtr));

      if (obs) { obs->remove_input_module (_PluginInfoData.get_name (), *this); }
   }
}


// Input Module Interface
void
dmz::InputModuleBasic::register_input_observer (
      const Handle Channel,
      const Mask &EventMask,
      InputObserver &obs) {

   ChannelStruct *cs (_create_channel (Channel));

   if (cs) {

      const Handle ObsHandle (obs.get_input_observer_handle ());

      ObsStruct *os (_obsTable.lookup (ObsHandle));

      if (!os) {

         os = new ObsStruct (obs);

         if (os) { if (!_obsTable.store (ObsHandle, os)) { delete os; os = 0; } }
      }

      Mask *obsMask (0);

      if (os) {

         obsMask = os->table.lookup (Channel);

         if (!obsMask) {

            obsMask = new Mask;

            if (!os->table.store (Channel, obsMask)) { delete obsMask; obsMask = 0; }
         }

         if (obsMask) {

            Mask updateMask;

            if (EventMask & LocalStateMask) {

               if (cs->stateTable.store (ObsHandle, &obs)) {

                  if (cs->active) { obs.update_channel_state (Channel, True); }
                  *obsMask |= LocalStateMask;
               }
            }

            if (EventMask & LocalAxisMask) {

               if (cs->axisTable.store (ObsHandle, os)) {

                  *obsMask |= LocalAxisMask;
                  updateMask |= LocalAxisMask;
               }
            }

            if (EventMask & LocalButtonMask) {

               if (cs->buttonTable.store (ObsHandle, os)) {

                  *obsMask |= LocalButtonMask;
                  updateMask |= LocalButtonMask;
               }
            }

            if (EventMask & LocalSwitchMask) {

               if (cs->switchTable.store (ObsHandle, os)) {

                  *obsMask |= LocalSwitchMask;
                  updateMask |= LocalSwitchMask;
               }
            }

            if (EventMask & LocalKeyMask) {

               if (cs->keyTable.store (ObsHandle, os)) {

                  *obsMask |= LocalKeyMask;
                  updateMask |= LocalKeyMask;
               }
            }

            if (EventMask & LocalMouseMask) {

               if (cs->mouseTable.store (ObsHandle, os)) {

                  *obsMask |= LocalMouseMask;
                  updateMask |= LocalMouseMask;
               }
            }

            if (EventMask & LocalDataMask) {

               if (cs->dataTable.store (ObsHandle, os)) {

                  *obsMask |= LocalDataMask;
                  updateMask |= LocalDataMask;
               }
            }

            if (obsMask->is_set ()) {

               cs->activeTable.store (os->ObsHandle, os);

               if (cs->active && updateMask.is_set ()) {

                  _increment_active_count (cs->ChannelHandle, updateMask, *os);
               }
            }
            else if (os->table.remove (Channel)) {

               delete obsMask; obsMask = 0;
               cs->activeTable.remove (os->ObsHandle);
            }
         }
      }
   }
}


void
dmz::InputModuleBasic::release_input_observer (
      const Handle Channel,
      const Mask &EventMask,
      InputObserver &obs) {

   ChannelStruct *cs (_channelTable.lookup (Channel));
   const Handle ObsHandle (obs.get_input_observer_handle ());
   ObsStruct *os (_obsTable.lookup (ObsHandle));

   if (cs && os) {

      Mask *obsMask (os->table.lookup (Channel));

      if (obsMask) {

         Mask updateMask;

         if (EventMask & LocalStateMask) {

            if (cs->stateTable.remove (ObsHandle)) { obsMask->unset (LocalStateMask); }
         }

         if (EventMask & LocalAxisMask) {

            if (cs->axisTable.remove (ObsHandle)) {

               obsMask->unset (LocalAxisMask);
               updateMask |= LocalAxisMask;
            }
         }

         if (EventMask & LocalButtonMask) {

            if (cs->buttonTable.remove (ObsHandle)) {

               obsMask->unset (LocalButtonMask);
               updateMask |= LocalButtonMask;
            }
         }

         if (EventMask & LocalSwitchMask) {

            if (cs->switchTable.remove (ObsHandle)) {

               obsMask->unset (LocalSwitchMask);
               updateMask |= LocalSwitchMask;
            }
         }

         if (EventMask & LocalKeyMask) {

            if (cs->keyTable.remove (ObsHandle)) {

               obsMask->unset (LocalKeyMask);
               updateMask |= LocalKeyMask;
            }
         }

         if (EventMask & LocalMouseMask) {

            if (cs->mouseTable.remove (ObsHandle)) {

               obsMask->unset (LocalMouseMask);
               updateMask |= LocalMouseMask;
            }
         }

         if (EventMask & LocalDataMask) {

            if (cs->dataTable.remove (ObsHandle)) {

               obsMask->unset (LocalDataMask);
               updateMask |= LocalDataMask;
            }
         }

         if (cs->active && updateMask.is_set ()) {

            _decrement_active_count (cs->ChannelHandle, updateMask, *os);
         }

         if (!obsMask->is_set ()) {

            cs->activeTable.remove (os->ObsHandle);

            if (os->table.remove (Channel)) { delete obsMask; obsMask = 0; }

            if (os->table.get_count () == 0) {

               if (_obsTable.remove (ObsHandle)) { delete os; os = 0; }
            }
         }
      }
   }
}


dmz::Boolean
dmz::InputModuleBasic::create_channel (const Handle Channel) {

   return _create_channel (Channel) != 0;
}


dmz::Boolean
dmz::InputModuleBasic::set_channel_state (
      const Handle Channel,
      const Boolean Value) {

   Boolean result (False);

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && !cs->locked) {

      if (cs->active != Value) {

         cs->active = Value;
         cs->locked = True;

         HashTableHandleIterator it;

         InputObserver *obs (cs->stateTable.get_first (it));

         while (obs) {

            obs->update_channel_state (cs->ChannelHandle, Value);
            obs = cs->stateTable.get_next (it);
         }

         cs->locked = False;

         ObsStruct *os = cs->activeTable.get_first (it);

         while (os) {

            Mask *ptr (os->table.lookup (cs->ChannelHandle));

            if (ptr) {

               if (Value) { _increment_active_count (Channel, *ptr, *os); }
               else { _decrement_active_count (Channel, *ptr, *os); }
            }

            os = cs->activeTable.get_next (it);
         }
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::InputModuleBasic::get_channel_state (const Handle Channel) const {

   Boolean result (False);

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs) { result = cs->active; }

   return result;
}


void
dmz::InputModuleBasic::send_axis_event (const InputEventAxis &Event) {

   if (_inEvent) { _que_event (new AxisQueStruct (Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      ControllerStruct *controller (_get_controller (Event.get_source_handle ()));

      if (controller) {

         InputEventAxis *tmp (controller->axisTable.lookup (Event.get_axis_id ()));

         if (tmp) {

            if (*tmp != Event) { *tmp = Event; }
            else { cs = 0; } // don't send duplicate events
         }
         else {

            tmp = new InputEventAxis (Event);

            if (tmp && !controller->axisTable.store (Event.get_axis_id (), tmp)) {

               delete tmp; tmp = 0;
            }
         }
      }

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->axisTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_axis_event (cs->ChannelHandle, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->axisTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::send_button_event (const InputEventButton &Event) {

   if (_inEvent) { _que_event (new ButtonQueStruct (Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      ControllerStruct *controller (_get_controller (Event.get_source_handle ()));

      if (controller) {

         InputEventButton *tmp (controller->buttonTable.lookup (Event.get_button_id ()));

         if (tmp) {

            if (*tmp != Event) { *tmp = Event; }
            else { cs = 0; } // event already sent; don't send again
         }
         else {

            tmp = new InputEventButton (Event);

            if (tmp && !controller->buttonTable.store (Event.get_button_id (), tmp)) {

               delete tmp; tmp = 0;
            }
         }
      }

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->buttonTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_button_event (cs->ChannelHandle, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->buttonTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::send_switch_event (const InputEventSwitch &Event) {

   if (_inEvent) { _que_event (new SwitchQueStruct (Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      ControllerStruct *controller (_get_controller (Event.get_source_handle ()));

      if (controller) {

         InputEventSwitch *tmp (controller->switchTable.lookup (Event.get_switch_id ()));

         if (tmp) {

            if (*tmp != Event) { *tmp = Event; }
            else { cs = 0; } // don't send duplicate events
         }
         else {

            tmp = new InputEventSwitch (Event);

            if (tmp && !controller->switchTable.store (Event.get_switch_id (), tmp)) {

               delete tmp; tmp = 0;
            }
         }
      }

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->switchTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_switch_event (cs->ChannelHandle, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->switchTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::send_key_event (const InputEventKey &Event) {

   if (_inEvent) { _que_event (new KeyQueStruct (Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      if (Event.get_key_state ()) {

         InputEventKey *tmp (new InputEventKey (Event));

         if (!_keyCache.store (Event.get_key (), tmp)) {

            if (tmp) { delete tmp; tmp = 0; }
            cs = 0; // Key has already been pushed no need to send event again
         }
      }
      else {

         InputEventKey *tmp (_keyCache.remove (Event.get_key ()));

         if (!tmp) {

            const UInt32 Key (Event.get_key ());

            if (Key < 256) {

               UInt32 modKey (0);

               const UInt32 Upper = toupper ((char)Key);

               if (Upper != Key) { modKey = Upper; }
               else {

                  const UInt32 Lower = tolower ((char)Key);
                  if (Lower != Key) { modKey = Lower; }
               }
          
               if (modKey) { tmp = _keyCache.remove (modKey); }
            }
         }

         if (tmp) { delete tmp; tmp = 0; }
         else { // Key release has already been sent, no need to send again

            cs = 0;
         }
      }

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->keyTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_key_event (cs->ChannelHandle, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->keyTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::send_mouse_event (const InputEventMouse &Event) {

   if (_inEvent) { _que_event (new MouseQueStruct (Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      if (_mouseCache != Event) { _mouseCache = Event; }
      else { cs = 0; } // don't send duplicate events

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->mouseTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_mouse_event (cs->ChannelHandle, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->mouseTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::send_data_event (const Handle Source, const Data &Event) {

   if (_inEvent) { _que_event (new DataQueStruct (Source, Event)); }
   else {

      _inEvent = True;

      _eventCount++;

      HashTableHandleIterator it;

      ChannelStruct *cs (_channelTable.get_first (it));

      while (cs) {

         if (cs->active) {

            HashTableHandleIterator osIt;

            ObsStruct *os (cs->dataTable.get_first (osIt));

            while (os) {

               if (os->eventCount != _eventCount) {

                  os->obs.receive_data_event (cs->ChannelHandle, Source, Event);
                  os->eventCount = _eventCount;
               }

               os = cs->dataTable.get_next (osIt);
            }
         }

         cs = _channelTable.get_next (it);
      }

      _inEvent = False;
      _do_qued_events ();
   }
}


void
dmz::InputModuleBasic::_increment_active_count (
      const Handle Channel,
      const Mask &EventMask,
      ObsStruct &os) {

   if (EventMask & LocalAxisMask) {

      os.axisActiveCount++;

      if (os.axisActiveCount == 1) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator axisIt;

            InputEventAxis *event (cs->axisTable.get_first (axisIt));

            while (event) {

               if (!is_zero32 (event->get_axis_value ())) {

                  os.obs.receive_axis_event (Channel, *event);
               }

               event = cs->axisTable.get_next (axisIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
   }

   if (EventMask & LocalButtonMask) {

      os.buttonActiveCount++;

      if (os.buttonActiveCount == 1) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator buttonIt;

            InputEventButton *event (cs->buttonTable.get_first (buttonIt));

            while (event) {

               if (event->get_button_value ()) {

                  os.obs.receive_button_event (Channel, *event);
               }

               event = cs->buttonTable.get_next (buttonIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
   }

   if (EventMask & LocalKeyMask) {

      os.keyActiveCount++;

      if (os.keyActiveCount == 1) {

         HashTableHandleIterator it;

         InputEventKey *event (_keyCache.get_first (it));

         while (event) {

            if (event->get_key_state ()) {

               os.obs.receive_key_event (Channel, *event);
            }

            event = _keyCache.get_next (it);
         }
      }
   }

   if (EventMask & LocalMouseMask) {

      os.mouseActiveCount++;

      if (os.mouseActiveCount == 1) {

         if (_mouseCache.get_button_mask ()) {

            os.obs.receive_mouse_event (Channel, _mouseCache);
         }
      }
   }

   if (EventMask & LocalSwitchMask) {

      os.switchActiveCount++;

      if (os.switchActiveCount == 1) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator switchIt;

            InputEventSwitch *event (cs->switchTable.get_first (switchIt));

            while (event) {

               if (event->get_switch_value ()) {

                  os.obs.receive_switch_event (Channel, *event);
               }

               event = cs->switchTable.get_next (switchIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
   }
}


void
dmz::InputModuleBasic::_decrement_active_count (
      const Handle Channel,
      const Mask &EventMask,
      ObsStruct &os) {

   if (EventMask & LocalAxisMask) {

      os.axisActiveCount--;

      if (os.axisActiveCount == 0) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator axisIt;

            InputEventAxis *event (cs->axisTable.get_first (axisIt));

            while (event) {

               InputEventAxis tmp (*event);

               if (tmp.update_axis_value (0.0f)) {

                  os.obs.receive_axis_event (Channel, tmp);
               }

               event = cs->axisTable.get_next (axisIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
      else if (os.axisActiveCount < 0) {

         _log.error << "Axis Active Count for: "
            << _defs.lookup_runtime_name (os.obs.get_input_observer_handle ())
            << " has a negative value" << endl;
      }
   }

   if (EventMask & LocalButtonMask) {

      os.buttonActiveCount--;

      if (os.buttonActiveCount == 0) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator buttonIt;

            InputEventButton *event (cs->buttonTable.get_first (buttonIt));

            while (event) {

               InputEventButton tmp (*event);

               if (tmp.update_button_value (False)) {

                  os.obs.receive_button_event (Channel, tmp);
               }

               event = cs->buttonTable.get_next (buttonIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
      else if (os.buttonActiveCount < 0) {

         _log.error << "Button Active Count for: "
            << _defs.lookup_runtime_name (os.obs.get_input_observer_handle ())
            << " has a negative value" << endl;
      }
   }

   if (EventMask & LocalSwitchMask) {

      os.switchActiveCount--;

      if (os.switchActiveCount == 0) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerCache.get_first (it));

         while (cs) {

            HashTableHandleIterator switchIt;

            InputEventSwitch *event (cs->switchTable.get_first (switchIt));

            while (event) {

               InputEventSwitch tmp (*event);

               if (tmp.update_switch_value (False)) {

                  os.obs.receive_switch_event (Channel, tmp);
               }

               event = cs->switchTable.get_next (switchIt);
            }

            cs = _controllerCache.get_next (it);
         }
      }
      else if (os.switchActiveCount < 0) {

         _log.error << "Switch Active Count for: "
            << _defs.lookup_runtime_name (os.obs.get_input_observer_handle ())
            << " has a negative value" << endl;
      }
   }

   if (EventMask & LocalKeyMask) {

      os.keyActiveCount--;

      if (os.keyActiveCount == 0) {

         HashTableHandleIterator it;

         InputEventKey *event (_keyCache.get_first (it));

         while (event) {

            InputEventKey tmp (*event);

            if (tmp.update_key_state (False)) {

               os.obs.receive_key_event (Channel, tmp);
            }

            event = _keyCache.get_next (it);
         }
      }
      else if (os.keyActiveCount < 0) {

         _log.error << "Key Active Count for: "
            << _defs.lookup_runtime_name (os.obs.get_input_observer_handle ())
            << " has a negative value" << endl;
      }
   }

   if (EventMask & LocalMouseMask) {

      os.mouseActiveCount--;

      if (os.mouseActiveCount == 0) {

         InputEventMouse event (_mouseCache);

         if (event.get_button_mask ()) {

            event.set_button_mask (0);

            os.obs.receive_mouse_event (Channel, event);
         }
      }
      else if (os.mouseActiveCount < 0) {

         _log.error << "Mouse Active Count for: "
            << _defs.lookup_runtime_name (os.obs.get_input_observer_handle ())
            << " has a negative value" << endl;
      }
   }
}


void
dmz::InputModuleBasic::_que_event (EventQueStruct *event) {

   if (event) {

      if (_eventQueTail) { _eventQueTail->next = event; _eventQueTail = event; }
      else { _eventQueHead = _eventQueTail = event; }
   }
}


void
dmz::InputModuleBasic::_do_qued_events () {

   if (!_inQue && _eventQueHead) {

      _inQue = True;

      EventQueStruct *current (_eventQueHead);

      while (current) { current->send_event (*this); current = current->next; }

      delete _eventQueHead; _eventQueHead = _eventQueTail = 0;

      _inQue = False;
   }
}


dmz::InputModuleBasic::ChannelStruct *
dmz::InputModuleBasic::_create_channel (const Handle Channel) {

   ChannelStruct *result (_channelTable.lookup (Channel));

   if (!result) {

      result = new ChannelStruct (Channel);

      if (result) {

         if (!_channelTable.store (result->ChannelHandle, result)) {

            delete result; result = 0;
         }
      }
   }

   return result;
}


dmz::InputModuleBasic::ControllerStruct *
dmz::InputModuleBasic::_get_controller (const Handle ControllerHandle) {

   ControllerStruct *cs (_controllerCache.lookup (ControllerHandle));

   if (!cs) {

      cs = new ControllerStruct (ControllerHandle);

      if (cs && !_controllerCache.store (ControllerHandle, cs)) { delete cs; cs =0; }
   }

   return cs;
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputModuleBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputModuleBasic (Info);
}

};

