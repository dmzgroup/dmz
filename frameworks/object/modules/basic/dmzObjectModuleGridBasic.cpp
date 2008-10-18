#include "dmzObjectModuleGridBasic.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::ObjectModuleGridBasic::ObjectModuleGridBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectModuleGrid (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _primaryAxis (VectorComponentX),
      _secondaryAxis (VectorComponentY),
      _xCoordMax (100),
      _yCoordMax (100),
      _maxGrid (100000.0, 0.0, 100000.0),
      _xCellSize (0.0),
      _yCellSize (0.0),
      _grid (0) {

   _init (local);
}


dmz::ObjectModuleGridBasic::~ObjectModuleGridBasic () {

   if (_grid) { delete []_grid; _grid = 0; }
}


// Plugin Interface
void
dmz::ObjectModuleGridBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::ObjectModuleGridBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}

// ObjectModuleGrid Interface
dmz::Boolean
dmz::ObjectModuleGridBasic::register_object_observer_grid (ObjectObserverGrid &observer) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::ObjectModuleGridBasic::update_object_observer_grid (ObjectObserverGrid &observer) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::ObjectModuleGridBasic::release_object_observer_grid (ObjectObserverGrid &observer) {

   Boolean result (False);

   return result;
}


void
dmz::ObjectModuleGridBasic::find_objects (
      const Volume &SearchSpace,
      HandleContainer &objects,
      const ObjectTypeSet *IncludeTypes,
      const ObjectTypeSet *ExcludeTypes) {

}

// Object Observer Interface
void
dmz::ObjectModuleGridBasic::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

}


void
dmz::ObjectModuleGridBasic::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::ObjectModuleGridBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::ObjectModuleGridBasic::_init (Config &local) {

   _xCoordMax = config_to_int32 ("grid.cell.x", local, _xCoordMax);
   _yCoordMax = config_to_int32 ("grid.cell.y", local, _yCoordMax);
   _minGrid = config_to_vector ("grid.min", local, _minGrid);
   _maxGrid = config_to_vector ("grid.max", local, _maxGrid);

   Vector vec (_maxGrid - _minGrid);
   _xCellSize = (Int32)(vec.get (_primaryAxis) / (Float64)(_xCoordMax));
   _yCellSize = (Int32)(vec.get (_secondaryAxis) / (Float64)(_yCoordMax));

   if (is_zero64 (_xCellSize)) { _xCellSize = 1.0; }
   if (is_zero64 (_yCellSize)) { _yCellSize = 1.0; }

   _grid = new GridStruct[_xCoordMax * _yCoordMax];
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectModuleGridBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectModuleGridBasic (Info, local);
}

};
