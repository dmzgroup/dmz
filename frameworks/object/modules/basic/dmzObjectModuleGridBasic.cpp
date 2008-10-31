#include <dmzObjectAttributeMasks.h>
#include "dmzObjectModuleGridBasic.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesVolume.h>

dmz::ObjectModuleGridBasic::ObjectModuleGridBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectModuleGrid (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _primaryAxis (VectorComponentX),
      _secondaryAxis (VectorComponentZ),
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

   _objTable.empty ();
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

   Vector origin, min, max;
   SearchSpace.get_extents (origin, min, max);
   Int32 minX = 0, minY = 0, maxX = 0, maxY = 0;
   _map_point_to_coord (min, minX, minY);
   _map_point_to_coord (max, maxX, maxY);

   ObjectStruct *list (0);

//   HandleContainer found;

   for (Int32 ix = minX; ix <= maxX; ix++) {

      for (Int32 jy = minY; jy <= maxY; jy++) {

         ObjectStruct *current (_grid[_map_coord (ix, jy)].objList);

         while (current) {

            Boolean test (True);

            if (IncludeTypes && !IncludeTypes->contains_type (current->Type)) {

               test = False;
            }
            else if (ExcludeTypes && ExcludeTypes->contains_type (current->Type)) {

               test = False;
            }

            if (test && SearchSpace.contains_point (current->pos)) {

#if 0
if (!found.add_handle (current->Object)) {

_log.error << "origin: " << origin << endl
   << "minX: " << minX << endl
   << "minY: " << minY << endl
   << "maxX: " << maxX << endl
   << "maxY: " << maxY << endl;


HandleContainer cc;
   for (Int32 xx = minX; xx <= maxX; xx++) {

      for (Int32 yy = minY; yy <= maxY; yy++) {
Boolean s = cc.add_handle ((Handle)_map_coord (xx, yy));
_log.error << (s ? "" : "##### NOT UNIQUE ") << xx << " " << yy << " = " << _map_coord (xx, yy) << endl;
      }
   }
}
#endif // 0

               current->distanceSquared = (origin - current->pos).magnitude_squared ();

               current->node = 0;

               if (!list) { list = current; }
               else {

                  ObjectStruct *p = 0, *c = list;

                  Boolean done (False);

                  while (!done) {

                     if (c->distanceSquared > current->distanceSquared) {

                        if (!c->node) { c->node = current; done = True; }
                        else {

                           p = c; c = c->node;

                           if (!c) { p->node = current; done = True; }
                        }
                     }
                     else {

                        if (p) { p->node = current; }
                        else { list = current; }

                        current->node = c;
                        done = True;
                     }
                  }
               }
            }

            current = current->next;
         }
      }
   }

   while (list) {

      if (!objects.add_handle (list->Object)) {

         _log.error << "Loop detected in object list for object: " << list->Object
            << endl;
         list = 0;
      }
      else { list = list->node; }
   }
}


// Object Observer Interface
void
dmz::ObjectModuleGridBasic::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   ObjectStruct *os = new ObjectStruct (ObjectHandle, Type);

   if (!_objTable.store (ObjectHandle, os)) { delete os; os = 0; }
}


void
dmz::ObjectModuleGridBasic::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objTable.remove (ObjectHandle));

   if (os) {

      _remove_object_from_grid (*os);

      delete os; os = 0;
   }
}


void
dmz::ObjectModuleGridBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os (_objTable.lookup (ObjectHandle));

   if (os && _grid) {

      os->pos = Value;

      const Int32 Place = _map_point (Value);

      if (Place != os->place) {

         _remove_object_from_grid (*os);
         os->place = Place;
         os->next = _grid[Place].objList;
         if (_grid[Place].objList) { _grid[Place].objList->prev = os; }
         _grid[Place].objList = os;
      }
   }
}


void
dmz::ObjectModuleGridBasic::_remove_object_from_grid (ObjectStruct &obj) {

   if (_grid && obj.place >= 0) {

      if (obj.prev) {

         obj.prev->next = obj.next;
      }
      else { _grid[obj.place].objList = obj.next; }

      if (obj.next) { obj.next->prev = obj.prev; }

      obj.next = obj.prev = 0;
   }
}


void
dmz::ObjectModuleGridBasic::_init (Config &local) {

   _xCoordMax = config_to_int32 ("grid.cell.x", local, _xCoordMax);
   _yCoordMax = config_to_int32 ("grid.cell.y", local, _yCoordMax);
   _minGrid = config_to_vector ("grid.min", local, _minGrid);
   _maxGrid = config_to_vector ("grid.max", local, _maxGrid);

   _log.info << "grid: " << _xCoordMax << "x" << _yCoordMax << endl;
   _log.info << "extents:" << endl
      << "\t" << _minGrid << endl
      << "\t" << _maxGrid << endl;

   Vector vec (_maxGrid - _minGrid);
   _xCellSize = vec.get (_primaryAxis) / (Float64)(_xCoordMax);
   _yCellSize = vec.get (_secondaryAxis) / (Float64)(_yCoordMax);

   _log.info << "cell count: " << _xCoordMax * _yCoordMax << endl;

   if (is_zero64 (_xCellSize)) { _xCellSize = 1.0; }
   if (is_zero64 (_yCellSize)) { _yCellSize = 1.0; }

   _grid = new GridStruct[_xCoordMax * _yCoordMax];

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectPositionMask);
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
