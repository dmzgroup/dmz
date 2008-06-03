#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzObjectModuleSelect.h>
#include "dmzObjectPluginSelectMove.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesVector.h>

/*!

\class dmz::ObjectPluginSelectMove
\ingroup Object
\brief Keeps selected objects in sync when one is moved.
\details When one selected object position is updated, all other selected object
positions are moved by the same delta.

*/

//! \cond
dmz::ObjectPluginSelectMove::ObjectPluginSelectMove (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _defaultHandle (0),
      _inUpdate (False),
      _selectMod (0),
      _log (Info) {

   _init (local);
}


dmz::ObjectPluginSelectMove::~ObjectPluginSelectMove () {

}


// Plugin Interface
void
dmz::ObjectPluginSelectMove::discover_plugin (const Plugin *PluginPtr) {

   if (!_selectMod) { _selectMod = ObjectModuleSelect::cast (PluginPtr); }
}


void
dmz::ObjectPluginSelectMove::start_plugin () {

}


void
dmz::ObjectPluginSelectMove::stop_plugin () {

}


void
dmz::ObjectPluginSelectMove::shutdown_plugin () {

}


void
dmz::ObjectPluginSelectMove::remove_plugin (const Plugin *PluginPtr) {

   if (_selectMod && (_selectMod == ObjectModuleSelect::cast (PluginPtr))) {

      _selectMod = 0;
   }
}


// Object Observer Interface
void
dmz::ObjectPluginSelectMove::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectModule *objMod (get_object_module ());

   if (!_inUpdate &&
         objMod &&
         PreviousValue &&
         _selectMod &&
         _selectMod->is_selected (ObjectHandle)) {

      _inUpdate = True;

      const Vector Diff (Value - *PreviousValue);

      HandleContainer container;

      _selectMod->get_selected_objects (container);

      Handle current (container.get_first ());

      while (current) {

         if (ObjectHandle != current) {

            Vector pos;

            if (objMod->lookup_position (current, _defaultHandle, pos)) {

               pos += Diff;

               objMod->store_position (current, _defaultHandle, pos);
            }
         }

         current = container.get_next ();
      }

      _inUpdate = False;
   }
}


void
dmz::ObjectPluginSelectMove::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   // Need to implement.
}


void
dmz::ObjectPluginSelectMove::_init (Config &local) {

   _defaultHandle =
      activate_default_object_attribute (ObjectPositionMask | ObjectOrientationMask);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginSelectMove (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginSelectMove (Info, local);
}

};
