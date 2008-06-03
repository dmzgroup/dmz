#ifndef DMZ_OBJECT_MODULE_BASIC_DOT_H
#define DMZ_OBJECT_MODULE_BASIC_DOT_H

#include <dmzObjectModule.h>
#include <dmzObjectObserver.h>
#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUUIDTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class Data;
   class Mask;
   class ObjectObserver;
   class ObjectType;
   class RuntimeContext;
   class String;
   class Vector;

   //! \cond
   typedef HashTableHandleTemplate<ObjectObserver> ObjectObserverStruct;
   //! \endcond

   class ObjectModuleBasic : public Plugin, protected ObjectModule {

      public:
         //! \cond
         struct ObsUpdateStruct {

            ObsUpdateStruct *next;

            ObsUpdateStruct () : next (0) {;}
            virtual ~ObsUpdateStruct () { if (next) { delete next; next = 0; } }

            virtual void update (ObjectModuleBasic &module) = 0;
         };

         ObjectModuleBasic (const PluginInfo &Info);
         ~ObjectModuleBasic ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // ObjectModule Interface
         virtual Boolean register_global_object_observer (ObjectObserver &observer);
         virtual Boolean release_global_object_observer (ObjectObserver &observer);

         virtual Boolean register_object_observer (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            ObjectObserver &observer);

         virtual Boolean release_object_observer (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            ObjectObserver &observer);

         virtual Boolean release_object_observer_all (ObjectObserver &Observer);

         virtual Boolean is_object (const Handle ObjectHandle);
         virtual Boolean is_link (const Handle ObjectHandle);

         virtual Boolean get_object_handles (HandleContainer &container);

         virtual Boolean dump_object_attributes (
            const Handle ObjectHandle,
            ObjectObserver &Observer);

         virtual Boolean dump_all_object_attributes (
            const Handle ObjectHandle,
            ObjectObserver &Observer);

         virtual Handle create_object (
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual Boolean activate_object (const Handle ObjectHandle);

         virtual Boolean destroy_object (const Handle ObjectHandle);

         virtual Handle clone_object (
            const Handle ObjectHandle,
            const ObjectLinkRetentionEnum LinkRetention);

         virtual Boolean store_uuid (const Handle ObjectHandle, const UUID &Value);
         virtual Boolean lookup_uuid (const Handle ObjectHandle, UUID &value);
         virtual Handle lookup_handle_from_uuid (const UUID &Value);

         virtual Boolean remove_attribute (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual Handle link_objects (
            const Handle AttributeHandle,
            const Handle SuperHandle,
            const Handle SubHandle);

         virtual Handle lookup_link_handle (
            const Handle AttributeHandle,
            const Handle SuperHandle,
            const Handle SubHandle);

         virtual Boolean lookup_linked_objects (
            const Handle LinkHandle,
            Handle &attributeHandle,
            Handle &superHandle,
            Handle &subHandle);

         virtual Boolean unlink_objects (const Handle LinkHandle);

         virtual Boolean store_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeObjectHandle);

         virtual Handle lookup_link_attribute_object (const Handle LinkHandle);

         virtual Boolean lookup_super_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            HandleContainer &container);

         virtual Boolean lookup_sub_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            HandleContainer &container);

         virtual Boolean store_locality (
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality);

         virtual ObjectLocalityEnum lookup_locality (const Handle ObjectHandle);
         virtual ObjectLocalityEnum lookup_locality (const UUID &Identity);

         virtual Boolean store_object_type (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value);

         virtual Boolean lookup_object_type (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            ObjectType &value);

         virtual Boolean store_state (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value);

         virtual Boolean lookup_state (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Mask &value);

         virtual Boolean store_flag (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean &Value);

         virtual Boolean lookup_flag (
            const Handle ObjectHandle,
            const Handle AttributeHandle);

         virtual Boolean store_time_stamp (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value);

         virtual Boolean lookup_time_stamp (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Float64 &value);

         virtual Boolean store_position (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_position (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_orientation (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value);

         virtual Boolean lookup_orientation (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Matrix &value);

         virtual Boolean store_velocity (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_velocity (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_acceleration (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_acceleration (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_scale (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_scale (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_vector (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_vector (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_scalar (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value);

         virtual Boolean lookup_scalar (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Float64 &value);

         virtual Boolean store_text (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value);

         virtual Boolean lookup_text (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            String &value);

         virtual Boolean store_data (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value);

         virtual Boolean lookup_data (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Data &value);

         // ObjectModuleBasic Interface
         void activate_created_object (const Handle ObjectHandle);

         void immediate_destroy_object (const Handle ObjectHandle);

         void update_object_uuid (
            const Handle ObjectHandle,
            const UUID &Identity,
            const UUID &PrevIdentity);

         void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         void update_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle,
            const UUID &AttributeIdentity,
            const Handle AttributeObjectHandle,
            const UUID &PrevAttributeIdentity,
            const Handle PrevAttributeObjectHandle);

         void update_object_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         void update_object_acceleration (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         void update_object_scale (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         void update_object_vector (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         void update_object_data (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value,
            const Data *PreviousValue);

      protected:
         struct LinkStruct {

            const Handle AttributeHandle;
            const Handle SuperHandle;
            const Handle SubHandle;
            const RuntimeHandle LinkHandleInstance;
            const Handle LinkHandle;
            Handle attrObjectHandle;

            LinkStruct (
                  const Handle TheAttributeHandle,
                  const Handle TheSuperHandle,
                  const Handle TheSubHandle,
                  RuntimeContext *context) :
                  AttributeHandle (TheAttributeHandle),
                  SuperHandle (TheSuperHandle),
                  SubHandle (TheSubHandle),
                  LinkHandleInstance ("ObjectLink", context),
                  LinkHandle (LinkHandleInstance.get_runtime_handle ()),
                  attrObjectHandle (0) {;}
         };

         typedef HashTableHandleTemplate<LinkStruct> LinkTable;

         struct ObjectStruct {

            ObjectStruct *next;

            Handle handle;

            UUID uuid;

            RuntimeHandle *handlePtr;

            Boolean active;

            ObjectLocalityEnum locality;

            HashTableHandle attrTable;

            HashTableHandle linkTable;

            HashTableHandleTemplate<LinkTable> superTable;
            HashTableHandleTemplate<LinkTable> subTable;

            HashTableHandleTemplate<ObjectType> typeTable;
            HashTableHandleTemplate<Mask> stateTable;
            HashTableHandleTemplate<Boolean> flagTable;
            HashTableHandleTemplate<Float64> timeStampTable;
            HashTableHandleTemplate<Vector> positionTable;
            HashTableHandleTemplate<Matrix> orientationTable;
            HashTableHandleTemplate<Vector> velocityTable;
            HashTableHandleTemplate<Vector> accelerationTable;
            HashTableHandleTemplate<Vector> scaleTable;
            HashTableHandleTemplate<Vector> vectorTable;
            HashTableHandleTemplate<Float64> scalarTable;
            HashTableHandleTemplate<String> textTable;
            HashTableHandleTemplate<Data> dataTable;

            Handle get_handle (const String &TypeName, RuntimeContext *context) {

               if (!handlePtr) {

                  String name (TypeName ? TypeName + "." : "UnknownType.");

                  handlePtr = new RuntimeHandle (name + "EntityObject", context);
               }

               if (handlePtr) { handle = handlePtr->get_runtime_handle (); }
               else { handle = 0; }

               return handle;
            }

            void reset () {

               if (handlePtr) { delete handlePtr; handlePtr = 0; }

               next = 0;
               handle = 0;
               uuid.clear ();
               active = False;
               locality = ObjectLocalityUnknown;
               attrTable.clear ();
               linkTable.clear ();
               superTable.clear ();
               subTable.clear ();
               typeTable.empty ();
               stateTable.empty ();
               flagTable.empty ();
               timeStampTable.empty ();
               positionTable.empty ();
               orientationTable.empty ();
               velocityTable.empty ();
               accelerationTable.empty ();
               scaleTable.empty ();
               vectorTable.empty ();
               scalarTable.empty ();
               textTable.empty ();
               dataTable.empty ();
            }

            ObjectStruct *clone (ObjectStruct *next) {

               ObjectStruct *result (next);

               if (!result) { result = new ObjectStruct; }
               else { result->reset (); }

               if (result) {

                  result->active = False;
                  result->locality = locality;

                  HashTableHandleIterator it;
                  void *ptr = attrTable.get_next (it);

                  while (ptr) { 

                     result->attrTable.store (it.get_hash_key (), ptr);
                     ptr = attrTable.get_next (it);
                  }

                  // Link cloning must happen externally

                  result->typeTable.copy (typeTable);
                  result->stateTable.copy (stateTable);
                  result->flagTable.copy (flagTable);
                  result->timeStampTable.copy (timeStampTable);
                  result->positionTable.copy (positionTable);
                  result->orientationTable.copy (orientationTable);
                  result->velocityTable.copy (velocityTable);
                  result->accelerationTable.copy (accelerationTable);
                  result->scaleTable.copy (scaleTable);
                  result->vectorTable.copy (vectorTable);
                  result->scalarTable.copy (scalarTable);
                  result->textTable.copy (textTable);
                  result->dataTable.copy (dataTable);
               }

               return result;
            }

            ObjectStruct () :
                  next (0),
                  handle (0),
                  handlePtr (0),
                  active (False),
                  locality (ObjectLocalityUnknown) {;}

            ~ObjectStruct () { if (next) { delete next; next = 0; } reset (); }
         };

         struct SubscriptionStruct {

            const Handle SubHandle;
            HashTableHandleTemplate<Mask> table;
            ObjectObserver &obs;

            SubscriptionStruct (ObjectObserver &theObs) :
                  SubHandle (theObs.get_object_observer_handle ()),
                  obs (theObs) {;}

            ~SubscriptionStruct () { table.empty (); }
         };

         ObjectStruct *_lookup_object (const Handle ObjectHandle);

         void _unlink_object (const ObjectStruct &Obj);

         void _update_subscription (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            const Boolean AddObs,
            ObjectObserver &observer);

         void _dump_all_objects (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            ObjectObserver &observer);

         void _dump_object (const Handle ObjectHandle);

         void _dump_object_attributes_to_observer (
            const ObjectStruct &Obj,
            const Handle AttibuteHandle,
            const Mask &AttributeMask,
            ObjectObserver &obs);

         void _add_observer_update (ObsUpdateStruct *ptr);
         void _update_observers ();

         Log _log;

         Boolean _inObsUpdate;
         Boolean _inStoredObsUpdate;
         ObsUpdateStruct *_obsUpdateList;
         ObsUpdateStruct *_obsUpdateListTail;

         ObjectStruct *_objectCache;
         ObjectStruct *_recycleList;

         Int32 _globalCount;
         HashTableHandleTemplate<ObjectObserver> _globalTable;

         HashTableHandleTemplate<SubscriptionStruct> _subscriptionTable;

         LinkTable _linkTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         HashTableUUIDTemplate<ObjectStruct> _uuidObjTable;

         HashTableHandleTemplate<ObjectObserver> _createTable;
         HashTableHandleTemplate<ObjectObserver> _destroyTable;
         HashTableHandleTemplate<ObjectObserver> _uuidTable;
         HashTableHandleTemplate<ObjectObserverStruct> _removeAttrTable;
         HashTableHandleTemplate<ObjectObserver> _localityTable;
         HashTableHandleTemplate<ObjectObserverStruct> _linkObsTable;
         HashTableHandleTemplate<ObjectObserverStruct> _unlinkObsTable;
         HashTableHandleTemplate<ObjectObserverStruct> _linkAttrTable;
         HashTableHandleTemplate<ObjectObserverStruct> _objectTypeTable;
         HashTableHandleTemplate<ObjectObserverStruct> _stateTable;
         HashTableHandleTemplate<ObjectObserverStruct> _flagTable;
         HashTableHandleTemplate<ObjectObserverStruct> _timeStampTable;
         HashTableHandleTemplate<ObjectObserverStruct> _positionTable;
         HashTableHandleTemplate<ObjectObserverStruct> _orientationTable;
         HashTableHandleTemplate<ObjectObserverStruct> _velocityTable;
         HashTableHandleTemplate<ObjectObserverStruct> _accelerationTable;
         HashTableHandleTemplate<ObjectObserverStruct> _scaleTable;
         HashTableHandleTemplate<ObjectObserverStruct> _vectorTable;
         HashTableHandleTemplate<ObjectObserverStruct> _scalarTable;
         HashTableHandleTemplate<ObjectObserverStruct> _textTable;
         HashTableHandleTemplate<ObjectObserverStruct> _dataTable;

         DataConverterHandle _handleConverter;
         Message _createObjMsg;
         Message _removeObjMsg;

         Handle _defaultHandle;
         //! \endcond

      private:
         ObjectModuleBasic ();
         ObjectModuleBasic (const ObjectModuleBasic &);
         ObjectModuleBasic &operator= (const ObjectModuleBasic &);
   };
};


//! \cond
inline dmz::ObjectModuleBasic::ObjectStruct *
dmz::ObjectModuleBasic::_lookup_object (const Handle ObjectHandle) {

   if (!_objectCache || (ObjectHandle != _objectCache->handle)) { 

      _objectCache = _objectTable.lookup (ObjectHandle);
   }

   return _objectCache;
}
//! \endcond

#endif // DMZ_OBJECT_MODULE_BASIC_DOT_H
