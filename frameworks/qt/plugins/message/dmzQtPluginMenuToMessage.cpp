#include "dmzQtPluginMenuToMessage.h"
#include <dmzObjectModule.h>
#include <dmzQtModuleCanvas.h>
#include <dmzRenderModulePick.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <QtGui/QtGui>


dmz::QtPluginMenuToMessage::QtPluginMenuToMessage (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _defs (Info),
      _objectModule (0),
      _objectModuleName (),
      _pickModule (0),
      _pickModuleName (),
      _defaultAttrHandle (0),
      _showMessage (0),
      _source (0),
      _objectAttrHandle (0),
      _positionAttrHandle (0),
      _object (0),
      _objectSet (),
      _actionGroup (0),
      _messageList (),
      _menu (0) {

   _actionGroup = new QActionGroup (this);

   connect (
      _actionGroup, SIGNAL (triggered (QAction *)),
      this, SLOT (_slot_send (QAction *)));

   _menu = new QMenu (get_plugin_name ().get_buffer ());
   _menu->setObjectName (get_plugin_name ().get_buffer ());

   _init (local);
}


dmz::QtPluginMenuToMessage::~QtPluginMenuToMessage () {

   if (_menu) { delete _menu; _menu = 0; }

   qDeleteAll (_messageList);
   _messageList.clear ();
}


// Plugin Interface
void
dmz::QtPluginMenuToMessage::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_objectModule) {

         _objectModule = ObjectModule::cast (PluginPtr, _objectModuleName);
      }

      if (!_pickModule) {

         _pickModule = RenderModulePick::cast (PluginPtr, _pickModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_objectModule && (_objectModule == ObjectModule::cast (PluginPtr))) {

         _objectModule = 0;
      }

      if (_pickModule && (_pickModule == RenderModulePick::cast (PluginPtr))) {

         _pickModule = 0;
      }
   }
}


// Message Observer Interface
void
dmz::QtPluginMenuToMessage::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Msg == _showMessage) {

      if (InData && _pickModule && _objectModule && _menu) {

         Handle obj;
         Vector pos;

         if (InData->lookup_handle (_objectAttrHandle, 0, obj) &&
             InData->lookup_vector (_positionAttrHandle, 0, pos)) {

            const ObjectType Type (_objectModule->lookup_object_type (obj));

            if (_objectSet.contains_exact_type (Type)) {

               _object = obj;

               Int32 screenX, screenY;

               pos.set_xyz (pos.get_x (), pos.get_z (), 0.0);

               if (_pickModule->world_to_screen (_source, pos, screenX, screenY)) {

                  _menu->popup (QPoint (screenX, screenY));
               }
            }
         }
      }
   }
}


void
dmz::QtPluginMenuToMessage::_slot_send (QAction *action) {

   if (action && _object) {

      foreach (MessageStruct *ms, _messageList) {

         if (ms->action == action) {

            if (ms->message) {

               Data data;

               data.store_handle (_objectAttrHandle, 0, _object);

               Handle target = ms->targets.get_first ();

               while (target) {

                  ms->message.send (target, &data, 0);

                  target = ms->targets.get_next ();
               }
            }

            break;
         }
      }

      _object = 0;
   }
}


void
dmz::QtPluginMenuToMessage::_get_targets (
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
}


void
dmz::QtPluginMenuToMessage::_get_type_set (
      const String &Name,
      Config &config,
      ObjectTypeSet &set) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Config typeList;

   if (config.lookup_all_config (Name + ".type", typeList)) {

      ConfigIterator it;
      Config cd;

      while (typeList.get_next_config (it, cd)) {

         set.add_object_type (config_to_string ("name", cd), context);
      }
   }
}


void
dmz::QtPluginMenuToMessage::_build_menu (const String &Name, Config &config) {

   Config messageList;

   if (config.lookup_all_config_merged (Name, messageList)) {

      ConfigIterator it;
      Config cd;

      while (messageList.get_next_config (it, cd)) {

         const String Text (config_to_string ("text", cd));

         const String StatusTip (config_to_string ("statusTip", cd, Text));

         Message message (config_create_message_type (
            "message", cd, "", get_plugin_runtime_context ()));

         if (Text && message && _menu && _actionGroup) {

            MessageStruct *next (new MessageStruct ());

            if (next) {

               next->action = new QAction (Text.get_buffer (), this);

               if (next->action) {

                  next->action->setStatusTip (StatusTip.get_buffer ());
                  _actionGroup->addAction (next->action);
               }

               next->message = message;
               _get_targets ("target", cd, next->targets);

               if (config_to_boolean ("separator", cd, False)) { _menu->addSeparator (); }

               _menu->addAction (next->action);

               _messageList.append (next);
            }
         }
      }
   }
}


void
dmz::QtPluginMenuToMessage::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _objectModuleName = config_to_string ("module.object.name", local);
   _pickModuleName = config_to_string ("module.pick.name", local);

   _defaultAttrHandle = _defs.create_named_handle (ObjectAttributeDefaultName);

   _source = config_to_named_handle (
      "source.name", local, "dmzQtModuleCanvasBasic", context);

   _objectAttrHandle = config_to_named_handle (
      "attribute.object.name", local, "object", context);

   _positionAttrHandle = config_to_named_handle (
      "attribute.position.name", local, "position", context);

   _showMessage = config_create_message_type (
      "message.show",
      local,
      "ShowObjectMenuMessage",
      context);

   subscribe_to_message (_showMessage);

   _get_type_set ("object", local, _objectSet);

   _build_menu ("menu", local);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMenuToMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMenuToMessage (Info, local);
}

};
