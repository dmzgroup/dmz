#include <dmzObjectAttributeMasks.h>
#include "dmzObjectModuleGridBasic.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
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
   _obsTable.empty ();
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

   ObserverStruct *os (_obsTable.lookup (observer.get_object_observer_grid_handle ()));

   if (!os) {

      os = new ObserverStruct (observer);

      if (!_obsTable.store (os->ObsHandle, os)) { delete os; os = 0; }
      result = update_object_observer_grid (observer);
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleGridBasic::update_object_observer_grid (ObjectObserverGrid &observer) {

   Boolean result (False);

   ObserverStruct *os (_obsTable.lookup (observer.get_object_observer_grid_handle ()));

   if (os) {

      const Volume &SearchSpace = os->obs.get_observer_volume ();

      Vector origin, min, max;
      SearchSpace.get_extents (origin, min, max);
      Int32 minX = 0, minY = 0, maxX = 0, maxY = 0;
      _map_point_to_coord (min, minX, minY);
      _map_point_to_coord (max, maxX, maxY);

      Boolean updateExtents = False;

      if (
            (minX != os->minX) ||
            (maxX != os->maxX) ||
            (minY != os->minY) ||
            (maxY != os->maxY)) { updateExtents = True; }

      if (updateExtents && (os->minX >= 0)) {

         for (Int32 ix = os->minX; ix <= os->maxX; ix++) {

            for (Int32 jy = os->minY; jy <= os->maxY; jy++) {

               const Int32 Place = _map_coord (ix, jy);

               _grid[Place].obsTable.remove (os->ObsHandle);

               HashTableHandleIterator it;
               GridStruct *cell = &(_grid[Place]);
               ObjectStruct *current = cell->objTable.get_first (it);

               while (current) {

                  if (!SearchSpace.contains_point (current->pos) &&
                        os->objects.remove_handle (current->Object)) {

                     observer.update_object_grid_state (
                        current->Object,
                        current->Type,
                        current->pos,
                        ObjectGridStateExit);
                  }

                  current = cell->objTable.get_next (it);
               }
            }
         }
      }

      os->minX = minX;
      os->maxX = maxX;
      os->minY = minY;
      os->maxY = maxX;

      for (Int32 ix = os->minX; ix <= os->maxX; ix++) {

         for (Int32 jy = os->minY; jy <= os->maxY; jy++) {

            const Int32 Place = _map_coord (ix, jy);

            GridStruct *cell = &(_grid[Place]);

            if (updateExtents) { cell->obsTable.store (os->ObsHandle, os); }

            HashTableHandleIterator it;
            ObjectStruct *current = cell->objTable.get_first (it);

            while (current) {

               _update_observer (SearchSpace, *current, *os);

               current = cell->objTable.get_next (it);
            }
         }
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleGridBasic::release_object_observer_grid (ObjectObserverGrid &observer) {

   Boolean result (False);

   ObserverStruct *os (_obsTable.remove (observer.get_object_observer_grid_handle ()));

   if (os) {

      for (Int32 ix = os->minX; ix <= os->maxX; ix++) {

         for (Int32 jy = os->minY; jy <= os->maxY; jy++) {

            const Int32 Place = _map_coord (ix, jy);

            _grid[Place].obsTable.remove (os->ObsHandle);
         }
      }

      delete os; os = 0;
      result = True;
   }
 
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

         HashTableHandleIterator it;
         GridStruct *cell = &(_grid[_map_coord (ix, jy)]);
         ObjectStruct *current = cell->objTable.get_first (it);

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

               current->next = 0;

               if (!list) { list = current; }
               else {

                  ObjectStruct *p = 0, *c = list;

                  Boolean done (False);

                  while (!done) {

                     if (c->distanceSquared > current->distanceSquared) {

                        if (!c->next) { c->next = current; done = True; }
                        else {

                           p = c; c = c->next;

                           if (!c) { p->next = current; done = True; }
                        }
                     }
                     else {

                        if (p) { p->next = current; }
                        else { list = current; }

                        current->next = c;
                        done = True;
                     }
                  }
               }
            }

            cell->objTable.get_next (it);
         }
      }
   }

   while (list) {

      if (!objects.add_handle (list->Object)) {

         _log.error << "Loop detected in object list for object: " << list->Object
            << endl;
         list = 0;
      }
      else { list = list->next; }
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

      if (os->place >= 0) {

         HashTableHandleIterator it;

         ObserverStruct *obs = _grid[os->place].obsTable.get_first (it);

         while (obs) {

            obs->objects.remove_handle (ObjectHandle);
            obs = _grid[os->place].obsTable.get_next (it);
         }
      }

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

   ObjectStruct *current (_objTable.lookup (ObjectHandle));

   if (current && _grid) {

      current->pos = Value;

      const Int32 OldPlace = current->place;
      const Int32 Place = _map_point (Value);

      if (Place != current->place) {

         _remove_object_from_grid (*current);
         current->place = Place;
         _grid[Place].objTable.store (current->Object, current);
      }

      if ((Place != OldPlace) && (OldPlace >= 0)) {

         HashTableHandleIterator it;
         GridStruct *cell = &(_grid[OldPlace]);
         ObserverStruct *os = cell->obsTable.get_first (it);
         HandleContainer tested;

         while (os) {

            tested.add_handle (os->ObsHandle);
            _update_observer (
               os->obs.get_observer_volume (),
               *current,
               *os);

            os = cell->obsTable.get_next (it);
         }

         cell = &(_grid[Place]);
         os = cell->obsTable.get_first (it);

         while (os) {

            if (!tested.contains (os->ObsHandle)) {

               _update_observer (
                  os->obs.get_observer_volume (),
                  *current,
                  *os);
            }

            os = cell->obsTable.get_next (it);
         }
      }
      else {

         HashTableHandleIterator it;
         GridStruct *cell = &(_grid[Place]);
         ObserverStruct *os = cell->obsTable.get_first (it);

         while (os) {

            _update_observer (
               os->obs.get_observer_volume (),
               *current,
               *os);

            os = cell->obsTable.get_next (it);
         }
      }
   }
}


void
dmz::ObjectModuleGridBasic::_remove_object_from_grid (ObjectStruct &obj) {

   if (_grid && obj.place >= 0) {

      _grid[obj.place].objTable.remove (obj.Object);
   }
}


void
dmz::ObjectModuleGridBasic::_update_observer (
      const Volume &SearchSpace,
      const ObjectStruct &Obj,
      ObserverStruct &os) {

   const Boolean Contains = SearchSpace.contains_point (Obj.pos);

   if (Contains && os.objects.add_handle (Obj.Object)) {

      os.obs.update_object_grid_state (
         Obj.Object,
         Obj.Type,
         Obj.pos,
         ObjectGridStateEnter);
   }
   else if (!Contains && os.objects.remove_handle (Obj.Object)) {

      os.obs.update_object_grid_state (
         Obj.Object,
         Obj.Type,
         Obj.pos,
         ObjectGridStateExit);
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
