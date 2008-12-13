#ifndef DMZ_OBJECT_MODULE_GRID_BASIC_DOT_H
#define DMZ_OBJECT_MODULE_GRID_BASIC_DOT_H

#include <dmzObjectModuleGrid.h>
#include <dmzObjectObserverGrid.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesVector.h>

namespace dmz {

   class ObjectModuleGridBasic :
         public Plugin,
         public ObjectModuleGrid,
         public ObjectObserverUtil {

      public:
         //! \cond
         ObjectModuleGridBasic (const PluginInfo &Info, Config &local);
         ~ObjectModuleGridBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // ObjectModuleGrid Interface
         virtual Boolean register_object_observer_grid (ObjectObserverGrid &observer);
         virtual Boolean update_object_observer_grid (ObjectObserverGrid &observer);
         virtual Boolean release_object_observer_grid (ObjectObserverGrid &observer);

         virtual void find_objects (
            const Volume &SearchSpace,
            HandleContainer &objects,
            const ObjectTypeSet *IncludeTypes,
            const ObjectTypeSet *ExcludeTypes);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
         struct ObjectStruct {

            const Handle Object;
            const ObjectType Type;
            Vector pos;

            Int32 place;

            Float64 distanceSquared;
            ObjectStruct *next;

            ObjectStruct (const Handle TheObject, const ObjectType &TheType) :
                  Object (TheObject),
                  Type (TheType),
                  place (-1),
                  distanceSquared (0.0),
                  next (0) {;}
         };

         struct ObserverStruct {

            const Handle ObsHandle;
            ObjectObserverGrid &obs;
            HandleContainer objects;
            Int32 minX;
            Int32 minY;
            Int32 maxX;
            Int32 maxY;

            ObserverStruct (ObjectObserverGrid &theObs) :
                  ObsHandle (theObs.get_object_observer_grid_handle ()),
                  obs (theObs),
                  minX (-1),
                  minY (-1),
                  maxX (-1),
                  maxY (-1) {;}
         };

         struct GridStruct {

            HashTableHandleTemplate<ObjectStruct> objTable;
            HashTableHandleTemplate<ObserverStruct> obsTable;
            GridStruct () {;}
            ~GridStruct () { objTable.clear (); obsTable.clear (); }
         };

         Int32 _map_coord (const Int32 X, const Int32 Y);
         void _map_point_to_coord (const Vector &Point, Int32 &x, Int32 &y);
         Int32 _map_point (const Vector &Point);
         void _remove_object_from_grid (ObjectStruct &obj);

         void _update_observer (
            const Volume &SearchSpace,
            const ObjectStruct &Obj,
            ObserverStruct &os);

         void _init (Config &local);

         Log _log;

         VectorComponentEnum _primaryAxis;
         VectorComponentEnum _secondaryAxis;

         Int32 _xCoordMax;
         Int32 _yCoordMax;

         Vector _minGrid;
         Vector _maxGrid;
         Float64 _xCellSize;
         Float64 _yCellSize;

         HashTableHandleTemplate<ObjectStruct> _objTable;
         HashTableHandleTemplate<ObserverStruct> _obsTable;

         GridStruct *_grid;
         //! \endcond

      private:
         ObjectModuleGridBasic ();
         ObjectModuleGridBasic (const ObjectModuleGridBasic &);
         ObjectModuleGridBasic &operator= (const ObjectModuleGridBasic &);
   };
};


//! \cond
inline dmz::Int32
dmz::ObjectModuleGridBasic::_map_coord (const Int32 X, const Int32 Y) {

   return ((_xCoordMax) * (Y < 0 ? 0 : (Y >= _yCoordMax ? _yCoordMax - 1 : Y))) +
      (X < 0 ? 0 : (X >= _xCoordMax ? _xCoordMax - 1 : X));
}


inline void
dmz::ObjectModuleGridBasic::_map_point_to_coord (
      const Vector &Point,
      Int32 &x,
      Int32 &y) {

   Vector vec (Point - _minGrid);
   x = (Int32)(vec.get (_primaryAxis) / _xCellSize);
   if (x < 0) { x = 0; } else if (x >= _xCoordMax) { x = _xCoordMax - 1; }
   y = (Int32)(vec.get (_secondaryAxis) / _yCellSize);
   if (y < 0) { y = 0; } else if (y >= _yCoordMax) { y = _yCoordMax - 1; }
}


inline dmz::Int32
dmz::ObjectModuleGridBasic::_map_point (const Vector &Point) {

   Int32 x = 0, y = 0;
   _map_point_to_coord (Point, x, y);
   return _map_coord (x, y);
}
//! \endcond

#endif // DMZ_OBJECT_MODULE_GRID_BASIC_DOT_H
