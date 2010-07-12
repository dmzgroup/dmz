#include <dmzObjectMaskConsts.h>
#include "dmzObjectModuleBasic.h"
#include "dmzObjectModuleBasicPrivate.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzSystem.h>
#include <dmzTypesBase.h>
#include <dmzTypesMask.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>

/*!

\class dmz::ObjectModuleBasic
\ingroup Object
\brief Basic ObjectModule implementation.
\details This provides a basic implementation of the ObjectModule. At this time the
ObjectModuleBasic has no configuration parameters.
\sa ObjectModule

*/

namespace {

   static const dmz::Mask CreateMask (0, dmz::ObjectAttributeCreateObject);
   static const dmz::Mask DestroyMask (0, dmz::ObjectAttributeDestroyObject);
   static const dmz::Mask LocalityMask (0, dmz::ObjectAttributeLocality);
   static const dmz::Mask UUIDMask (0, dmz::ObjectAttributeUUID);
   static const dmz::Mask RemoveMask (0, dmz::ObjectAttributeRemove);
   static const dmz::Mask LinkMask (0, dmz::ObjectAttributeLink);
   static const dmz::Mask UnlinkMask (0, dmz::ObjectAttributeUnlink);
   static const dmz::Mask LinkAttributeMask (0, dmz::ObjectAttributeLinkObject);
   static const dmz::Mask CounterMask (0, dmz::ObjectAttributeCounter);
   static const dmz::Mask MinCounterMask (0, dmz::ObjectAttributeCounterMin);
   static const dmz::Mask MaxCounterMask (0, dmz::ObjectAttributeCounterMax);
   static const dmz::Mask AltObjectTypeMask (0, dmz::ObjectAttributeAltObjectType);
   static const dmz::Mask StateMask (0, dmz::ObjectAttributeState);
   static const dmz::Mask FlagMask (0, dmz::ObjectAttributeFlag);
   static const dmz::Mask TimeStampMask (0, dmz::ObjectAttributeTimeStamp);
   static const dmz::Mask PositionMask (0, dmz::ObjectAttributePosition);
   static const dmz::Mask OrientationMask (0, dmz::ObjectAttributeOrientation);
   static const dmz::Mask VelocityMask (0, dmz::ObjectAttributeVelocity);
   static const dmz::Mask AccelerationMask (0, dmz::ObjectAttributeAcceleration);
   static const dmz::Mask ScaleMask (0, dmz::ObjectAttributeScale);
   static const dmz::Mask VectorMask (0, dmz::ObjectAttributeVector);
   static const dmz::Mask ScalarMask (0, dmz::ObjectAttributeScalar);
   static const dmz::Mask TextMask (0, dmz::ObjectAttributeText);
   static const dmz::Mask DataMask (0, dmz::ObjectAttributeData);
   static const dmz::Mask AllMask (0, dmz::ObjectAttributeAll);

   static const dmz::Boolean AddObserver = dmz::True;
   static const dmz::Boolean RemoveObserver = dmz::False;
}

//! \cond
dmz::ObjectModuleBasic::ObjectModuleBasic (const PluginInfo &Info) :
      Plugin (Info),
      ObjectModule (Info),
      _log (Info),
      _inObsUpdate (False),
      _inStoredObsUpdate (False),
      _obsUpdateList (0),
      _obsUpdateListTail (0),
      _objectCache (0),
      _recycleList (0),
      _globalCount (0),
      _handleConverter (Info.get_context ()),
      _defaultHandle (0) {

   Definitions defs (Info, &_log);

   defs.create_message (ObjectCreateMessageName, _createObjMsg);
   defs.create_message (ObjectDestroyMessageName, _removeObjMsg);
   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);
}


dmz::ObjectModuleBasic::~ObjectModuleBasic () {

   _objectCache = 0;

   if (_recycleList) { delete _recycleList; _recycleList = 0; }

   HashTableHandleIterator it;

   SubscriptionStruct *ss (_subscriptionTable.get_first (it));

   RuntimeContext *context (_PluginInfoData.get_context ());

   while (ss) {

      if (ObjectObserver::is_valid (ss->SubHandle, context)) {

         ss->obs.remove_object_module (_PluginInfoData.get_name (), *this);
      }

      ss  = _subscriptionTable.get_next (it);
   }

   _globalTable.clear ();

   _subscriptionTable.empty ();

   _uuidObjTable.clear ();
   _objectTable.empty ();

   _linkTable.empty ();

   _linkObsTable.empty ();
   _unlinkObsTable.empty ();
   _linkAttrTable.empty ();
   _counterTable.empty ();
   _minCounterTable.empty ();
   _maxCounterTable.empty ();
   _altTypeTable.empty ();
   _stateTable.empty ();
   _flagTable.empty ();
   _timeStampTable.empty ();
   _positionTable.empty ();
   _orientationTable.empty ();
   _velocityTable.empty ();
   _accelerationTable.empty ();
   _scaleTable.empty ();
   _vectorTable.empty ();
   _scalarTable.empty ();
   _textTable.empty ();
   _dataTable.empty ();
}


// Plugin Interface
void
dmz::ObjectModuleBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateShutdown) {

      HashTableHandleIterator it;
      ObjectStruct *os (0);

      while (_objectTable.get_next (it, os)) {

         destroy_object (os->handle);
      }
   }
}


void
dmz::ObjectModuleBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      ObjectObserver *obs (ObjectObserver::cast (PluginPtr));

      if (obs) { obs->store_object_module (_PluginInfoData.get_name (), *this); }
   }
   else if (Mode == PluginDiscoverRemove) {

      ObjectObserver *obs (ObjectObserver::cast (PluginPtr));

      if (obs) { obs->remove_object_module (_PluginInfoData.get_name (), *this); }
   }
}


// ObjectModule Interface
dmz::Boolean
dmz::ObjectModuleBasic::register_global_object_observer (ObjectObserver &observer) {

   const Handle ObsHandle (observer.get_object_observer_handle ());

   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (sub) {

      HashTableHandleIterator it;
      Mask *attrMaskPtr (sub->table.get_first (it));

      while (attrMaskPtr) {

         if (*attrMaskPtr) {

            _update_subscription (
               it.get_hash_key (),
               *attrMaskPtr,
               RemoveObserver,
               observer);
         }

         attrMaskPtr = sub->table.get_next (it);
      }
   }

   Boolean result (_globalTable.store (ObsHandle, &observer));

   _globalCount = _globalTable.get_count ();

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::release_global_object_observer (ObjectObserver &observer) {

   Boolean result (False);

   if (!_inObsUpdate) { result = immediate_release_global_object_observer (observer); }
   else {

      _add_observer_update (new ReleaseGlobalObserverStruct (observer));

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::register_object_observer (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      ObjectObserver &obs) {

   Boolean result (False);

   const Handle ObsHandle (obs.get_object_observer_handle ());
   const Boolean IsGlobal (_globalTable.lookup (ObsHandle) == &obs);
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));
   Mask origMask;

   if (!sub) {

      sub = new SubscriptionStruct (obs);

      if (!_subscriptionTable.store (ObsHandle, sub)) { delete sub; sub = 0; }
   }

   if (sub) {

      Mask *attrMaskPtr (sub->table.lookup (AttributeHandle));

      if (!attrMaskPtr) {

         attrMaskPtr = new Mask (AttributeMask);

         if (attrMaskPtr) {

            if (!sub->table.store (AttributeHandle, attrMaskPtr)) {

               delete attrMaskPtr; attrMaskPtr = 0;
            }
         }
      }
      else { origMask = *attrMaskPtr; *attrMaskPtr |= AttributeMask; }

      if (!IsGlobal && attrMaskPtr) {

         result = True;

         Mask modMask (AttributeMask);
         // remove all masks that have been previously activated
         modMask.unset (origMask);
         _update_subscription (AttributeHandle, modMask, AddObserver, obs);
         _dump_all_objects (AttributeHandle, modMask, obs);
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::release_object_observer (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      ObjectObserver &observer) {

   Boolean result (False);

   if (!_inObsUpdate) {

      result = immediate_release_object_observer (
         AttributeHandle,
         AttributeMask,
         observer);
   }
   else {

      _add_observer_update (new ReleaseObserverStruct (
         AttributeHandle,
         AttributeMask,
         observer));

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::release_object_observer_all (ObjectObserver &observer) {

   Boolean result (False);

   if (!_inObsUpdate) { result = immediate_release_object_observer_all (observer); }
   else {

      _add_observer_update (new ReleaseObserverAllStruct (observer));

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::is_object (const Handle ObjectHandle) {

   return _lookup_object (ObjectHandle) != 0;
}


dmz::Boolean
dmz::ObjectModuleBasic::is_activated (const Handle ObjectHandle) {

   ObjectStruct *os = _lookup_object (ObjectHandle);

   return os ? os->active : False;
}


dmz::Boolean
dmz::ObjectModuleBasic::is_link (const Handle LinkHandle) {

   return _linkTable.lookup (LinkHandle) != 0;
}


dmz::Boolean
dmz::ObjectModuleBasic::get_object_handles (HandleContainer &container) {

   container.clear ();

   HashTableHandleIterator it;

   ObjectStruct *obj (_objectTable.get_first (it));

   while (obj) {

      if (obj->active) { container.add_handle (obj->handle); }
      obj = _objectTable.get_next (it);
   }

   return container.get_count () > 0;
}


dmz::Boolean
dmz::ObjectModuleBasic::dump_object_attributes (
      const Handle ObjectHandle,
      ObjectObserver &observer) {

   Boolean result (False);

   SubscriptionStruct *sub (
      _subscriptionTable.lookup (observer.get_object_observer_handle ()));

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (sub && obj) {

      Mask *mask (sub->table.lookup (_defaultHandle));

      if (mask) { _dump_object_create (*obj, *mask, sub->obs); }

      HashTableHandleIterator it;

      mask = sub->table.get_first (it);

      while (mask) {

         _dump_object_attributes_to_observer (
            *obj,
            it.get_hash_key (),
            *mask,
            sub->obs);

         mask = sub->table.get_next (it);
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::dump_all_object_attributes (
      const Handle ObjectHandle,
      ObjectObserver &observer) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      _dump_object_create (*obj, AllMask, observer);

      HashTableHandleIterator it;

      void *mask (obj->attrTable.get_next (it));

      while (mask) {

         const Handle AttributeHandle (it.get_hash_key ());

         _dump_object_attributes_to_observer (
            *obj,
            AttributeHandle,
            AllMask,
            observer);

         mask = obj->attrTable.get_next (it);
      }

      result = True;
   }

   return result;
}


void
dmz::ObjectModuleBasic::dump_attributes (
      const Handle AttribueHandle,
      const Mask &AttributeMask,
      ObjectObserver &observer) {

   HandleContainer objects;

   if (get_object_handles (objects)) {

      HandleContainerIterator it;
      Handle handle (0);

      while (objects.get_next (it, handle)) {

         ObjectStruct *obj (_lookup_object (handle));

         if (obj) {

            _dump_object_attributes_to_observer (
               *obj,
               AttribueHandle,
               AttributeMask,
               observer);
         }
      }
   }
}


dmz::Handle
dmz::ObjectModuleBasic::create_object (
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   Handle result (0);

   if (Type) {

      ObjectStruct *obj (_recycleList);

      if (obj) { _recycleList = _recycleList->next; obj->reset (); }
      else { obj = new ObjectStruct; }

      if (obj) {

         result = obj->get_handle (Type.get_name (), _PluginInfoData.get_context ());

         if (result) {

            if (_objectTable.store (result, obj)) {

               _objectCache = obj;
               obj->type = Type;
               obj->attrTable.store (_defaultHandle, (void *)this);
               store_locality (result, Locality);
            }
            else {

               result = 0;
               obj->next = _recycleList; _recycleList = obj;
            }
         }
         else {

            result = 0;
            obj->next = _recycleList; _recycleList = obj;
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::activate_object (const Handle ObjectHandle) {

   Boolean result (False);

   ObjectStruct *obj (_objectTable.lookup (ObjectHandle));

   if (obj) {

      if (!obj->active) {

//         obj->active = True;

         if ((obj->locality == ObjectLocal) && !obj->uuid) {

            if (create_uuid (obj->uuid)) {

               if (!_uuidObjTable.store (obj->uuid, obj)) { obj->uuid.clear (); }
            }
         }

         if (!_inObsUpdate) { activate_created_object (obj->handle); }
         else { _add_observer_update (new CreateObjectStruct (obj->handle)); }

         result = True;
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::destroy_object (const Handle ObjectHandle) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      result = True;

      if (!_inObsUpdate) { immediate_destroy_object (ObjectHandle); }
      else { _add_observer_update (new DestroyObjectStruct (obj->handle)); }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_uuid (const Handle ObjectHandle, const UUID &Value) {

   Boolean result (False);

   ObjectStruct *objUUID (_uuidObjTable.lookup (Value));
   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && !objUUID && (obj->uuid != Value)) {

      const UUID OldUUID (obj->uuid);
      obj->uuid = Value;

      _uuidObjTable.remove (OldUUID);

      if (Value) { _uuidObjTable.store (Value, obj); }

      if (obj->active) {

         result = True;

         if (!_inObsUpdate) { update_object_uuid (ObjectHandle, obj->uuid, OldUUID); }
         else {

            _add_observer_update (
               new ObjectUUIDStruct (ObjectHandle, obj->uuid, OldUUID));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_uuid (const Handle ObjectHandle, UUID &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && obj->uuid) { value = obj->uuid; result = True; }

   return result;
}


dmz::Handle
dmz::ObjectModuleBasic::lookup_handle_from_uuid (const UUID &Value) {

   Handle result (0);

   ObjectStruct *obj (_uuidObjTable.lookup (Value));

   if (obj) { result = obj->handle; _objectCache = obj; }

   return result;
}


dmz::Handle
dmz::ObjectModuleBasic::clone_object (
      const Handle ObjectHandle,
      const ObjectLinkRetentionEnum LinkRetention) {

   Handle result (0);

   ObjectStruct *obj (_objectTable.lookup (ObjectHandle));

   if (obj) {

      ObjectStruct *next (_recycleList);

      if (next) { _recycleList = next->next; next->reset (); }

      ObjectStruct *clone (obj->clone (next));

      if (clone) {

         result = clone->get_handle (
            clone->type.get_name (),
            _PluginInfoData.get_context ());

         if (result) {

            if (_objectTable.store (result, clone)) {

               if (LinkRetention == ObjectRetainLinks) {

                  HashTableHandleIterator it;

                  LinkTable *lt (obj->subTable.get_first (it));

                  while (lt) {

                     HashTableHandleIterator linkIt;

                     LinkStruct *ls (lt->get_first (linkIt));

                     while (ls) {

                        if (ls->SuperHandle != result) {

                           const Handle LinkHandle (
                              link_objects (ls->AttributeHandle, result, ls->SubHandle));

                           if (ls->attrObjectHandle && LinkHandle) {

                              store_link_attribute_object (
                                 LinkHandle,
                                 ls->attrObjectHandle);
                           }
                        }

                        ls = lt->get_next (linkIt);
                     }

                     lt = obj->subTable.get_next (it);
                  }

                  lt = obj->superTable.get_first (it);

                  while (lt) {

                     HashTableHandleIterator linkIt;

                     LinkStruct *ls (lt->get_first (linkIt));

                     while (ls) {

                        if (ls->SubHandle != result) {

                           const Handle LinkHandle (
                              link_objects (
                                 ls->AttributeHandle,
                                 ls->SuperHandle,
                                 result));

                           if (ls->attrObjectHandle && LinkHandle) {

                              store_link_attribute_object (
                                 LinkHandle,
                                 ls->attrObjectHandle);
                           }
                        }

                        ls = lt->get_next (linkIt);
                     }

                     lt = obj->superTable.get_next (it);
                  }
               }
            }
            else { clone->next = _recycleList; _recycleList = clone; result = 0; }
         }
      }
   }

   return result;
}


dmz::ObjectType
dmz::ObjectModuleBasic::lookup_object_type (const Handle ObjectHandle) {

   ObjectType result;

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) { result = obj->type; }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::remove_attribute (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttributeMask) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Mask foundMask;

      if (CounterMask & AttributeMask) {

         if (obj->counterTable.lookup (AttributeHandle)) { foundMask |= CounterMask; }
      }

      if (AltObjectTypeMask & AttributeMask) {

         if (obj->altTypeTable.lookup (AttributeHandle)) {

            foundMask |= AltObjectTypeMask;
         }
      }

      if (StateMask & AttributeMask) {

         if (obj->stateTable.lookup (AttributeHandle)) { foundMask |= StateMask; }
      }

      if (FlagMask & AttributeMask) {

         if (obj->flagTable.lookup (AttributeHandle)) { foundMask |= FlagMask; }
      }

      if (TimeStampMask & AttributeMask) {

         if (obj->timeStampTable.lookup (AttributeHandle)) {

            foundMask |= TimeStampMask;
         }
      }

      if (PositionMask & AttributeMask) {

         if (obj->positionTable.lookup (AttributeHandle)) { foundMask |= PositionMask; }
      }

      if (OrientationMask & AttributeMask) {

         if (obj->orientationTable.lookup (AttributeHandle)) {

            foundMask |= OrientationMask;
         }
      }

      if (VelocityMask & AttributeMask) {

         if (obj->velocityTable.lookup (AttributeHandle)) { foundMask |= VelocityMask; }
      }

      if (AccelerationMask & AttributeMask) {

         if (obj->accelerationTable.lookup (AttributeHandle)) {

            foundMask |= AccelerationMask;
         }
      }

      if (ScaleMask & AttributeMask) {

         if (obj->scaleTable.lookup (AttributeHandle)) { foundMask |= ScaleMask; }
      }

      if (VectorMask & AttributeMask) {

         if (obj->vectorTable.lookup (AttributeHandle)) { foundMask |= VectorMask; }
      }

      if (ScalarMask & AttributeMask) {

         if (obj->scalarTable.lookup (AttributeHandle)) { foundMask |= ScalarMask; }
      }

      if (TextMask & AttributeMask) {

         if (obj->textTable.lookup (AttributeHandle)) { foundMask |= TextMask; }
      }

      if (DataMask & AttributeMask) {

         if (obj->dataTable.lookup (AttributeHandle)) { foundMask |= DataMask; }
      }

      if (foundMask.is_set ()) {

         result = True;

         if (!_inObsUpdate) {

            remove_object_attribute (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               foundMask);
         }
         else {

            _add_observer_update (new RemoveObjectAttrStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               foundMask));
         }
      }
   }

   return result;
}


dmz::Handle
dmz::ObjectModuleBasic::link_objects (
      const Handle AttributeHandle,
      const Handle SuperHandle,
      const Handle SubHandle) {

   Handle result (0);

   ObjectStruct *super (_lookup_object (SuperHandle));
   ObjectStruct *sub (_lookup_object (SubHandle));

   if ((super != sub) && super && sub && AttributeHandle) {

      super->attrTable.store (AttributeHandle, (void *)this);
      sub->attrTable.store (AttributeHandle, (void *)this);

      LinkTable *subTable = super->subTable.lookup (AttributeHandle);

      if (!subTable) {

         subTable = new LinkTable;

         if (!super->subTable.store (AttributeHandle, subTable)) {

            delete subTable; subTable = 0;
         }
      }

      LinkTable *superTable = sub->superTable.lookup (AttributeHandle);

      if (!superTable) {

         superTable = new LinkTable;

         if (!sub->superTable.store (AttributeHandle, superTable)) {

            delete superTable; superTable = 0;
         }
      }

      LinkTable *superSuperTable = super->superTable.lookup (AttributeHandle);
      LinkTable *subSubTable = sub->subTable.lookup (AttributeHandle);

      // Make sure the sub isn't a super of the super and that the super isn't
      // a sub of the sub so we don't get two links each going in opposite directions.
      if (superSuperTable &&
            subSubTable &&
            subSubTable->lookup (SuperHandle) &&
            superSuperTable->lookup (SubHandle)) { subTable = superTable = 0; }

      if (subTable &&
            superTable &&
            !subTable->lookup (SubHandle) &&
            !superTable->lookup (SuperHandle)) {

         LinkStruct *ptr (new LinkStruct (
            AttributeHandle,
            SuperHandle,
            SubHandle,
            get_plugin_runtime_context ()));

         if (ptr && _linkTable.store (ptr->LinkHandle, ptr)) {

            subTable->store (SubHandle, ptr);
            superTable->store (SuperHandle, ptr);

            result = ptr->LinkHandle;

            if (super->active) {

               if (!_inObsUpdate) {

                  link_objects (
                     result,
                     AttributeHandle,
                     super->uuid,
                     SuperHandle,
                     sub->uuid,
                     SubHandle);
               }
               else {

                  _add_observer_update (new LinkObjectsStruct (
                     result,
                     AttributeHandle,
                     super->uuid,
                     SuperHandle,
                     sub->uuid,
                     SubHandle));
               }
            }
         }
         else if (ptr) { delete ptr; ptr = 0; }
      }
   }

   return result;
}


dmz::Handle
dmz::ObjectModuleBasic::lookup_link_handle (
      const Handle AttributeHandle,
      const Handle SuperHandle,
      const Handle SubHandle) {

   Handle result (0);

   ObjectStruct *super (_lookup_object (SuperHandle));

   if (super) {

      LinkTable *subTable (super->subTable.lookup (AttributeHandle));

      if (subTable) {

         LinkStruct *ls (subTable->lookup (SubHandle));

         if (ls) { result = ls->LinkHandle; }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_linked_objects (
      const Handle LinkHandle,
      Handle &attributeHandle,
      Handle &superHandle,
      Handle &subHandle) {

   Boolean result (False);

   LinkStruct *ls (_linkTable.lookup (LinkHandle));

   if (ls) {

      result = True;
      attributeHandle = ls->AttributeHandle;
      superHandle = ls->SuperHandle;
      subHandle = ls->SubHandle;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::unlink_objects (const Handle LinkHandle) {

   Boolean result (False);

   LinkStruct *ls (_linkTable.lookup (LinkHandle));

   if (ls && ls->attrObjectHandle) { store_link_attribute_object (LinkHandle, 0); }

   // Get the link struct again in case the link was deleted by callbacks in
   // store_link_attribute_object
   ls = _linkTable.lookup (LinkHandle);

   ObjectStruct *super (_lookup_object (ls ? ls->SuperHandle : 0));
   ObjectStruct *sub (_lookup_object (ls ? ls->SubHandle : 0));

   if (ls && super && sub) {

      result = True;

      if (super->active && sub->active) {

         if (!_inObsUpdate) {

            unlink_objects (
               ls->LinkHandle,
               ls->AttributeHandle,
               super->uuid,
               super->handle,
               sub->uuid,
               sub->handle);
         }
         else {

            _add_observer_update (new UnlinkObjectsStruct (
               ls->LinkHandle,
               ls->AttributeHandle,
               super->uuid,
               super->handle,
               sub->uuid,
               sub->handle));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::unlink_super_links (
      const Handle ObjectHandle,
      const Handle AttributeHandle) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      LinkTable *lt (obj->superTable.lookup (AttributeHandle));

      if (lt) { _unlink_table (*lt); }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::unlink_sub_links (
      const Handle ObjectHandle,
      const Handle AttributeHandle) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      LinkTable *lt (obj->subTable.lookup (AttributeHandle));

      if (lt) { _unlink_table (*lt); }

      result = True;
   }

   return result;
}



dmz::Boolean
dmz::ObjectModuleBasic::store_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeObjectHandle) {

   Boolean result (False);

   LinkStruct *ls (_linkTable.lookup (LinkHandle));

   ObjectStruct *attr (_lookup_object (AttributeObjectHandle));
   ObjectStruct *super (_lookup_object (ls ? ls->SuperHandle : 0));
   ObjectStruct *sub (_lookup_object (ls ? ls->SubHandle : 0));

   if (ls && super && sub) {

      super->attrTable.store (ls->AttributeHandle, (void *)this);
      sub->attrTable.store (ls->AttributeHandle, (void *)this);

      LinkTable *subTable = super->subTable.lookup (ls->AttributeHandle);

      if (AttributeObjectHandle != ls->attrObjectHandle) {

         UUID prevUUID;
         const Handle Prev (ls->attrObjectHandle);

         if (Prev) {

            ObjectStruct *os (_lookup_object (Prev));
            if (os) {

               os->linkTable.remove (ls->LinkHandle);
               prevUUID = os->uuid;
            }
         }

         if (attr) { attr->linkTable.store (ls->LinkHandle, (void *)this); }

         ls->attrObjectHandle = AttributeObjectHandle;

         if (!_inObsUpdate) {

            update_link_attribute_object (
               ls->LinkHandle,
               ls->AttributeHandle,
               super->uuid,
               super->handle,
               sub->uuid,
               sub->handle,
               attr ? attr->uuid : UUID (),
               AttributeObjectHandle,
               prevUUID,
               Prev);
         }
         else {

            _add_observer_update (new LinkObjectAttrStruct (
               ls->LinkHandle,
               ls->AttributeHandle,
               super->uuid,
               super->handle,
               sub->uuid,
               sub->handle,
               attr ? attr->uuid : UUID (),
               AttributeObjectHandle,
               prevUUID,
               Prev));
         }
      }
   }

   return result;
}


dmz::Handle
dmz::ObjectModuleBasic::lookup_link_attribute_object (const Handle LinkHandle) {

   Handle result (0);

   LinkStruct *ls (_linkTable.lookup (LinkHandle));

   if (ls) { result = ls->attrObjectHandle; }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_attribute_object_links (
      const Handle ObjectHandle,
      HandleContainer &container) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      if (obj->linkTable.get_count () > 0) {

         result = True;

         HashTableHandleIterator it;

         while (obj->linkTable.get_next (it)) {

            container.add_handle (it.get_hash_key ());
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_super_links (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      HandleContainer &container) {

   container.clear ();

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      LinkTable *table = obj->superTable.lookup (AttributeHandle);

      if (table) {

         HashTableHandleIterator it;

         LinkStruct *ptr (table->get_first (it));

         while (ptr) {

            container.add_handle (ptr->SuperHandle);
            ptr = table->get_next (it);
         }
      }
   }

   return container.get_count () > 0;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_sub_links (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      HandleContainer &container) {

   container.clear ();

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      LinkTable *table = obj->subTable.lookup (AttributeHandle);

      if (table) {

         HashTableHandleIterator it;

         LinkStruct *ptr (table->get_first (it));

         while (ptr) {

            container.add_handle (ptr->SubHandle);
            ptr = table->get_next (it);
         }
      }
   }

   return container.get_count () > 0;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_locality (
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && (obj->locality != Locality)) {

      const ObjectLocalityEnum OldLocality (obj->locality);
      obj->locality = Locality;

      if (obj->active) {

         result = True;

         if (!_inObsUpdate) {

            update_object_locality (obj->uuid, ObjectHandle, Locality, OldLocality);
         }
         else {

            _add_observer_update (new ObjectLocalityStruct (
               obj->uuid,
               ObjectHandle,
               Locality,
               OldLocality));
         }
      }
   }

   return result;
}


dmz::ObjectLocalityEnum
dmz::ObjectModuleBasic::lookup_locality (const Handle ObjectHandle) {

   ObjectLocalityEnum result (ObjectLocalityUnknown);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) { result = obj->locality; }

   return result;
}


dmz::ObjectLocalityEnum
dmz::ObjectModuleBasic::lookup_locality (const UUID &Identity) {

   ObjectLocalityEnum result (ObjectLocalityUnknown);

   ObjectStruct *obj (_uuidObjTable.lookup (Identity));

   if (obj) { result = obj->locality; _objectCache = obj; }

   return result;
}

dmz::Boolean
dmz::ObjectModuleBasic::store_counter (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      Int64 prevValue (0);
      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      result = True;

      CounterStruct *cs (obj->counterTable.lookup (AttributeHandle));

      if (cs) {

         Int64 realValue (Value);

         if (cs->min && (Value < *(cs->min))) { realValue = *(cs->min); }
         if (cs->max && (Value > *(cs->max))) { realValue = *(cs->max); }

         if (realValue != cs->counter) {

            prevValue = cs->counter; 
            cs->counter = realValue;
         }
         else { updateObservers = False; }
      }
      else {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         cs = new CounterStruct;

         if (!obj->counterTable.store (AttributeHandle, cs)) {

            result = False;
            delete cs; cs = 0;
         }
         else { cs->counter = Value; }
      }

      if (updateObservers && obj->active && cs) {

         if (!_inObsUpdate) {

            update_object_counter (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new UpdateCounterStruct (
               CounterValue,
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_counter (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Int64 &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      if (ptr) { value = ptr->counter; result = True; }
   }

   return result;
}



dmz::Int64
dmz::ObjectModuleBasic::add_to_counter (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value) {

   Int64 result (0);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      // If the counter struct doesn't exist, create it with a call to store_counter
      if (!ptr) {

         store_counter (ObjectHandle, AttributeHandle, 0);
         ptr = obj->counterTable.lookup (AttributeHandle);
      }

      if (ptr) {

         Int64 newCounter (ptr->counter);

         const Int64 Min (ptr->min ? *(ptr->min) : MinInt64);
         const Int64 Max (ptr->max ? *(ptr->max) : MaxInt64);

         if (Value > 0) {

            if (ptr->counter > 0) {

               const Int64 Diff (Max - ptr->counter);

               if (Value > Diff) {

                  if (ptr->rollover) { newCounter = Min + (Value - Diff) - 1; }
                  else { newCounter = Max; }
               }
               else { newCounter += Value; }
            }
            else { newCounter += Value; }
         }
         else {

            if (ptr->counter < 0) {

               const Int64 Diff (Min - ptr->counter);

               if (Value < Diff) {

                  if (ptr->rollover) { newCounter = Max + (Value - Diff) + 1; }
                  else { newCounter = Min; }
               }
               else { newCounter += Value; }
            }
            else { newCounter += Value; }
         }

         if (store_counter (ObjectHandle, AttributeHandle, newCounter)) {

            result = ptr->counter;
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_counter_minimum (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Int64 prevValue;
      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (!(ptr->min)) { ptr->min = new Int64 (Value); prevValueExists = False; }
         else if (Value != *(ptr->min)) { prevValue = *(ptr->min); *(ptr->min) = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new CounterStruct;

         if (obj->counterTable.store (AttributeHandle, ptr)) {

            ptr->min = new Int64 (Value);
         }
         else {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_counter_minimum (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new UpdateCounterStruct (
               CounterMin,
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }

      if (ptr->counter < Value) { store_counter (ObjectHandle, AttributeHandle, Value); }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_counter_minimum (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Int64 &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      if (ptr && ptr->min) { value = *(ptr->min); result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_counter_maximum (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Int64 prevValue;
      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (!(ptr->max)) { ptr->max = new Int64 (Value); prevValueExists = False; }
         else if (Value != *(ptr->max)) { prevValue = *(ptr->max); *(ptr->max) = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new CounterStruct;

         if (obj->counterTable.store (AttributeHandle, ptr)) {

            ptr->max = new Int64 (Value);
         }
         else {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_counter_maximum (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new UpdateCounterStruct (
               CounterMax,
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }

      if (ptr->counter > Value) { store_counter (ObjectHandle, AttributeHandle, Value); }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_counter_maximum (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Int64 &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      if (ptr && ptr->max) { value = *(ptr->max); result = True; }
   }

   return result;
}

dmz::Boolean
dmz::ObjectModuleBasic::store_counter_rollover (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      if (ptr) { ptr->rollover = Value; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_counter_rollover (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Boolean &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      CounterStruct *ptr (obj->counterTable.lookup (AttributeHandle));

      if (ptr) { value = ptr->rollover; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_alternate_object_type (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      ObjectType prevValue;
      ObjectType *ptr (obj->altTypeTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new ObjectType (Value);

         if (!obj->altTypeTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_alternate_type (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new AltObjectTypeStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_alternate_object_type (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      ObjectType &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      ObjectType *ptr (obj->altTypeTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_state (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Mask prevValue;
      Mask *ptr (obj->stateTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Mask (Value);

         if (!obj->stateTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_state (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectStateStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_state (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Mask &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Mask *ptr (obj->stateTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_flag (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Boolean prevValue;
      Boolean *ptr (obj->flagTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Boolean (Value);

         if (!obj->flagTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_flag (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectFlagStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_flag (
      const Handle ObjectHandle,
      const Handle AttributeHandle) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Boolean *ptr (obj->flagTable.lookup (AttributeHandle));

      if (ptr) { result = *ptr; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_time_stamp (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Float64 prevValue;
      Float64 *ptr (obj->timeStampTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Float64 (Value);

         if (!obj->timeStampTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_time_stamp (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectTimeStampStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_time_stamp (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Float64 &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Float64 *ptr (obj->timeStampTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_position (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Vector prevValue;
      Vector *ptr (obj->positionTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Vector (Value);

         if (!obj->positionTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_position (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectPositionStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_position (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Vector *ptr (obj->positionTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_orientation (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Matrix prevValue;
      Matrix *ptr (obj->orientationTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Matrix (Value);

         if (!obj->orientationTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_orientation (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectOrientationStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_orientation (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Matrix &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Matrix *ptr (obj->orientationTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_velocity (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Vector prevValue;
      Vector *ptr (obj->velocityTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Vector (Value);

         if (!obj->velocityTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_velocity (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectVelocityStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_velocity (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Vector *ptr (obj->velocityTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_acceleration (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Vector prevValue;
      Vector *ptr (obj->accelerationTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Vector (Value);

         if (!obj->accelerationTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_acceleration (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectAccelerationStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_acceleration (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Vector *ptr (obj->accelerationTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_scale (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Vector prevValue;
      Vector *ptr (obj->scaleTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Vector (Value);

         if (!obj->scaleTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_scale (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectScaleStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_scale (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Vector *ptr (obj->scaleTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_vector (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Vector prevValue;
      Vector *ptr (obj->vectorTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Vector (Value);

         if (!obj->vectorTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_vector (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectVectorStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_vector (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Vector *ptr (obj->vectorTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_scalar (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Float64 prevValue (0.0);
      Float64 *ptr (obj->scalarTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Float64 (Value);

         if (!obj->scalarTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_scalar (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectScalarStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_scalar (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Float64 &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Float64 *ptr (obj->scalarTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_text (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      String prevValue;
      String *ptr (obj->textTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new String (Value);

         if (!obj->textTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_text (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectTextStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_text (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      String &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      String *ptr (obj->textTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::store_data (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && AttributeHandle) {

      result = True;

      Data prevValue;
      Data *ptr (obj->dataTable.lookup (AttributeHandle));

      Boolean prevValueExists (True);
      Boolean updateObservers (True);

      if (ptr) {

         if (Value != *ptr) { prevValue = *ptr; *ptr = Value; }
         else { updateObservers = False; }
      }
      else if (!ptr) {

         obj->attrTable.store (AttributeHandle, (void *)this);

         prevValueExists = False;

         ptr = new Data (Value);

         if (!obj->dataTable.store (AttributeHandle, ptr)) {

            result = False;
            delete ptr; ptr = 0;
         }
      }

      if (updateObservers && obj->active && ptr) {

         if (!_inObsUpdate) {

            update_object_data (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0);
         }
         else {

            _add_observer_update (new ObjectDataStruct (
               obj->uuid,
               ObjectHandle,
               AttributeHandle,
               Value,
               prevValueExists ? &prevValue : 0));
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::lookup_data (
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      Data &value) {

   Boolean result (False);

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      Data *ptr (obj->dataTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


// ObjectModuleBasic Interface
dmz::Boolean
dmz::ObjectModuleBasic::immediate_release_global_object_observer (
      ObjectObserver &observer) {

   _inObsUpdate = True;

   const Handle ObsHandle (observer.get_object_observer_handle ());

   Boolean result (_globalTable.remove (ObsHandle) == &observer);

   _globalCount = _globalTable.get_count ();

   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (sub) {

      HashTableHandleIterator it;
      Mask *attrMaskPtr (sub->table.get_first (it));

      while (attrMaskPtr) {

         if (*attrMaskPtr) {

            _update_subscription (
               it.get_hash_key (),
               *attrMaskPtr,
               AddObserver,
               observer);
         }

         attrMaskPtr = sub->table.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::immediate_release_object_observer (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      ObjectObserver &observer) {

   _inObsUpdate = True;

   Boolean result (False);

   const Handle ObsHandle (observer.get_object_observer_handle ());
   const Boolean IsGlobal (_globalTable.lookup (ObsHandle) == &observer);
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (sub) {

      Mask *attrMaskPtr (sub->table.lookup (AttributeHandle));

      if (attrMaskPtr) {

         result = True;

         if (!IsGlobal) {

            _update_subscription (
               AttributeHandle,
               AttributeMask,
               RemoveObserver,
               observer);
         }

         attrMaskPtr->unset (AttributeMask);

         if (!attrMaskPtr->is_set ()) {

            attrMaskPtr = sub->table.remove (AttributeHandle);

            if (attrMaskPtr) { delete attrMaskPtr; attrMaskPtr = 0; }

            if (sub->table.get_count () <= 0) {

               sub = _subscriptionTable.remove (ObsHandle);
               if (sub) { delete sub; sub = 0; }
            }
         }
      }
   }

   _inObsUpdate = False;

   _update_observers ();

   return result;
}


dmz::Boolean
dmz::ObjectModuleBasic::immediate_release_object_observer_all (ObjectObserver &observer) {

   _inObsUpdate = True;

   Boolean result (False);

   const Handle ObsHandle (observer.get_object_observer_handle ());
   SubscriptionStruct *sub (_subscriptionTable.remove (ObsHandle));

   if (sub) {

      HashTableHandleIterator it;

      Mask *attrMaskPtr (sub->table.get_first (it));

      while (attrMaskPtr) {

         _update_subscription (
            it.get_hash_key (),
            *attrMaskPtr,
            RemoveObserver,
            observer);

         attrMaskPtr->clear ();

         attrMaskPtr = sub->table.get_next (it);
      }

      release_global_object_observer (observer);

      delete sub; sub = 0;

      result = True;
   }

   _inObsUpdate = False;

   _update_observers ();

   return result;
}


void
dmz::ObjectModuleBasic::activate_created_object (const Handle ObjectHandle) {

   _inObsUpdate = True;

   ObjectStruct *obj (_lookup_object (ObjectHandle));
   if (obj) { obj->active = True; }

   _dump_object (ObjectHandle);

   Data out (_handleConverter.to_data (ObjectHandle));
   _createObjMsg.send (&out);

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::immediate_destroy_object (const Handle ObjectHandle) {

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) { _unlink_object (*obj); }

   // Check to see if the object was deleted in _unlink_object
   obj = _lookup_object (ObjectHandle);

   if (obj) {

      if (obj->active) {

         HashTableHandleIterator attrObjIt;

         while (obj->linkTable.get_next (attrObjIt)) {

            store_link_attribute_object (attrObjIt.get_hash_key (), 0);
         }
      }
   }

   // Check to see if the object was deleted in store_link_attribute_object
   obj = _lookup_object (ObjectHandle);

   _inObsUpdate = True;

   if (obj) {

      if (obj->active) {

         HashTableHandleIterator it;

         ObjectObserver *obs (_destroyTable.get_first (it));

         while (obs) {

            obs->destroy_object (obj->uuid, obj->handle);
            obs = _destroyTable.get_next (it);
         }

         if (_globalCount > 0) {

            obs = _globalTable.get_first (it);

            while (obs) {

               obs->destroy_object (obj->uuid, obj->handle);
               obs = _globalTable.get_next (it);
            }
         }
      }

      _objectTable.remove (ObjectHandle);

      _uuidObjTable.remove (obj->uuid);
      obj->next = _recycleList; _recycleList = obj;
      if (obj == _objectCache) { _objectCache = 0; }

      Data out (_handleConverter.to_data (ObjectHandle));
      _removeObjMsg.send (&out);
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   _inObsUpdate = True;

   HashTableHandleIterator it;

   ObjectObserver *obs (_localityTable.get_first (it));

   while (obs) {

      obs->update_object_uuid (ObjectHandle, Identity, PrevIdentity);
      obs = _localityTable.get_next (it);
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_uuid (ObjectHandle, Identity, PrevIdentity);
         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_removeAttrTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->remove_object_attribute (
            Identity,
            ObjectHandle,
            AttributeHandle,
            AttrMask);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->remove_object_attribute (
            Identity,
            ObjectHandle,
            AttributeHandle,
            AttrMask);

         obs = _globalTable.get_next (it);
      }
   }

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj) {

      if (CounterMask & AttrMask) {

         CounterStruct *ptr (obj->counterTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (AltObjectTypeMask & AttrMask) {

         ObjectType *ptr (obj->altTypeTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (StateMask & AttrMask) {

         Mask *ptr (obj->stateTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (FlagMask & AttrMask) {

         Boolean *ptr (obj->flagTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (TimeStampMask & AttrMask) {

         Float64 *ptr (obj->timeStampTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (PositionMask & AttrMask) {

         Vector *ptr (obj->positionTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (OrientationMask & AttrMask) {

         Matrix *ptr (obj->orientationTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (VelocityMask & AttrMask) {

         Vector *ptr (obj->velocityTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (AccelerationMask & AttrMask) {

         Vector *ptr (obj->accelerationTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (ScaleMask & AttrMask) {

         Vector *ptr (obj->scaleTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (VectorMask & AttrMask) {

         Vector *ptr (obj->vectorTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (ScalarMask & AttrMask) {

         Float64 *ptr (obj->scalarTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (TextMask & AttrMask) {

         String *ptr (obj->textTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }

      if (DataMask & AttrMask) {

         Data *ptr (obj->dataTable.remove (AttributeHandle));
         if (ptr) { delete ptr; ptr = 0; }
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   _inObsUpdate = True;

   HashTableHandleIterator it;

   ObjectObserver *obs (_localityTable.get_first (it));

   while (obs) {

      obs->update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
      obs = _localityTable.get_next (it);
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_linkObsTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->link_objects (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->link_objects (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   _inObsUpdate = True;

   ObjectStruct *super (_lookup_object (SuperHandle));
   ObjectStruct *sub (_lookup_object (SubHandle));

   if (super) {

      LinkTable *subTable = super->subTable.lookup (AttributeHandle);
      if (subTable) { subTable->remove (SubHandle); }
   }

   if (sub) {

      LinkTable *superTable = sub->superTable.lookup (AttributeHandle);
      if (superTable) { superTable->remove (SuperHandle); }
   }

   ObjectObserverStruct *os (_unlinkObsTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->unlink_objects (
               LinkHandle,
               AttributeHandle,
               SuperIdentity,
               SuperHandle,
               SubIdentity,
               SubHandle);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->unlink_objects (
               LinkHandle,
               AttributeHandle,
               SuperIdentity,
               SuperHandle,
               SubIdentity,
               SubHandle);

         obs = _globalTable.get_next (it);
      }
   }

   LinkStruct *ls = _linkTable.remove (LinkHandle);

   if (ls) { delete ls; ls = 0; }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_linkAttrTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_link_attribute_object (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle,
            AttributeIdentity,
            AttributeObjectHandle,
            PrevAttributeIdentity,
            PrevAttributeObjectHandle);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_link_attribute_object (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle,
            AttributeIdentity,
            AttributeObjectHandle,
            PrevAttributeIdentity,
            PrevAttributeObjectHandle);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 &Value,
      const Int64 *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_counterTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_counter (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_counter (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 &Value,
      const Int64 *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_minCounterTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_counter_minimum (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_counter_minimum (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 &Value,
      const Int64 *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_maxCounterTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_counter_maximum (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_counter_maximum (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_altTypeTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_alternate_type (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_alternate_type (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_stateTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_state (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_state (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_flagTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_flag (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_flag (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_timeStampTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_time_stamp (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_time_stamp (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_positionTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_position (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_position (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_orientationTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_orientation (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_orientation (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_velocityTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_velocity (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_velocity (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_accelerationTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_acceleration (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_acceleration (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_scaleTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_scale (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_scale (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_vectorTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_vector (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_vector (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_scalarTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_scalar (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_scalar (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_textTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_text (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_text (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   _inObsUpdate = True;

   ObjectObserverStruct *os (_dataTable.lookup (AttributeHandle));

   if (os) {

      HashTableHandleIterator it;

      ObjectObserver *obs (os->get_first (it));

      while (obs) {

         obs->update_object_data (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = os->get_next (it);
      }
   }

   if (_globalCount > 0) {

      HashTableHandleIterator it;

      ObjectObserver *obs (_globalTable.get_first (it));

      while (obs) {

         obs->update_object_data (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PreviousValue);

         obs = _globalTable.get_next (it);
      }
   }

   _inObsUpdate = False;

   _update_observers ();
}


void
dmz::ObjectModuleBasic::_unlink_table (const LinkTable &Table) {

   HashTableHandleIterator linkIt;

   LinkStruct *ls (Table.get_first (linkIt));

   while (ls) {

      unlink_objects (ls->LinkHandle);
      ls = Table.get_next (linkIt);
   }
}


void
dmz::ObjectModuleBasic::_unlink_object (const ObjectStruct &Obj) {

   HandleContainer container;

   HashTableHandleIterator it;

   LinkTable *lt (Obj.subTable.get_first (it));

   while (lt) {

      _unlink_table (*lt);
      lt = Obj.subTable.get_next (it);
   }

   lt = Obj.superTable.get_first (it);

   while (lt) {

      _unlink_table (*lt);
      lt = Obj.superTable.get_next (it);
   }
}


static inline void
local_update_os (
      const dmz::Boolean AddObs,
      const dmz::Handle ObsHandle,
      const dmz::Handle Attribute,
      dmz::HashTableHandleTemplate<dmz::ObjectObserverStruct> &table,
      dmz::ObjectObserver *obs) {

   dmz::ObjectObserverStruct *os (table.lookup (Attribute));

   if (!os && AddObs) {

      os = new dmz::ObjectObserverStruct;
      if (os) { if (!table.store (Attribute, os)) { delete os; os = 0;  } }
   }

   if (os) {

      if (AddObs) { os->store (ObsHandle, obs); }
      else { os->remove (ObsHandle); }
   }
}


void
dmz::ObjectModuleBasic::_update_subscription (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      const Boolean AddObs,
      ObjectObserver &obs) {

   const Handle ObsHandle (obs.get_object_observer_handle ());

   if ((CreateMask & AttributeMask) && (AttributeHandle == _defaultHandle)) {

      if (AddObs) { _createTable.store (ObsHandle, &obs); }
      else { _createTable.remove (ObsHandle); }
   }

   if ((DestroyMask & AttributeMask) && (AttributeHandle == _defaultHandle)) {

      if (AddObs) { _destroyTable.store (ObsHandle, &obs); }
      else { _destroyTable.remove (ObsHandle); }
   }

   if ((UUIDMask & AttributeMask) && (AttributeHandle == _defaultHandle)) {

      if (AddObs) { _uuidTable.store (ObsHandle, &obs); }
      else { _uuidTable.remove (ObsHandle); }
   }

   if (RemoveMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _removeAttrTable, &obs);
   }

   if ((LocalityMask & AttributeMask) && (AttributeHandle == _defaultHandle)) {

      if (AddObs) { _localityTable.store (ObsHandle, &obs); }
      else { _localityTable.remove (ObsHandle); }
   }

   if (LinkMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _linkObsTable, &obs);
   }

   if (UnlinkMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _unlinkObsTable, &obs);
   }

   if (LinkAttributeMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _linkAttrTable, &obs);
   }

   if (CounterMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _counterTable, &obs);
   }

   if (MinCounterMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _minCounterTable, &obs);
   }

   if (MaxCounterMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _maxCounterTable, &obs);
   }

   if (AltObjectTypeMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _altTypeTable, &obs);
   }

   if (StateMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _stateTable, &obs);
   }

   if (FlagMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _flagTable, &obs);
   }

   if (TimeStampMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _timeStampTable, &obs);
   }

   if (PositionMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _positionTable, &obs);
   }

   if (OrientationMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _orientationTable, &obs);
   }

   if (VelocityMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _velocityTable, &obs);
   }

   if (AccelerationMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _accelerationTable, &obs);
   }

   if (ScaleMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _scaleTable, &obs);
   }

   if (VectorMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _vectorTable, &obs);
   }

   if (ScalarMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _scalarTable, &obs);
   }

   if (TextMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _textTable, &obs);
   }

   if (DataMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _dataTable, &obs);
   }
}


void
dmz::ObjectModuleBasic::_dump_all_objects (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      ObjectObserver &Observer) {

   HashTableHandleIterator it;

   ObjectStruct *obj = _objectTable.get_first (it);

   while (obj) {

      if (AttributeHandle == _defaultHandle) {

         _dump_object_create (*obj, AttributeMask, Observer);
      }

      _dump_object_attributes_to_observer (
         *obj,
         AttributeHandle,
         AttributeMask,
         Observer);

      obj = _objectTable.get_next (it);
   }
}


void
dmz::ObjectModuleBasic::_dump_object (const Handle ObjectHandle) {

   ObjectStruct *obj (_lookup_object (ObjectHandle));

   if (obj && obj->active) {

      HashTableHandleIterator it;

      SubscriptionStruct *sub (_subscriptionTable.get_first (it));

      while (sub) {

         Mask *mask (sub->table.lookup (_defaultHandle));

         if (mask) {

            _dump_object_create (
               *obj,
               *mask,
               sub->obs);
         }

         sub = _subscriptionTable.get_next (it);
      }

      if (_globalCount > 0) {

         ObjectObserver *obs (_globalTable.get_first (it));

         while (obs) {

            _dump_object_create (
               *obj,
               AllMask,
               *obs);

            obs = _globalTable.get_next (it);
         }
      }

      HashTableHandleIterator handleIt;

      void *ptr (obj->attrTable.get_next (handleIt));

      while (ptr) {

         const Handle AttributeHandle (handleIt.get_hash_key ());

         sub = _subscriptionTable.get_first (it);

         while (sub) {

            Mask *mask (sub->table.lookup (AttributeHandle));

            if (mask) {

               _dump_object_attributes_to_observer (
                  *obj,
                  AttributeHandle,
                  *mask,
                  sub->obs);
            }

            sub = _subscriptionTable.get_next (it);
         }

         if (_globalCount > 0) {

            ObjectObserver *obs (_globalTable.get_first (it));

            while (obs) {

               _dump_object_attributes_to_observer (
                  *obj,
                  AttributeHandle,
                  AllMask,
                  *obs);

               obs = _globalTable.get_next (it);
            }
         }

         ptr = obj->attrTable.get_next (handleIt);
      }
   }
}


void
dmz::ObjectModuleBasic::_dump_object_create (
      const ObjectStruct &Obj,
      const Mask &AttributeMask,
      ObjectObserver &obs) {

   if (CreateMask & AttributeMask) {

      obs.create_object (Obj.uuid, Obj.handle, Obj.type, Obj.locality);
   }

   if (UUIDMask & AttributeMask) {

      if (Obj.uuid) {

         const UUID EmptyUUID;

         obs.update_object_uuid (Obj.handle, Obj.uuid, EmptyUUID);
      }
   }
}


void
dmz::ObjectModuleBasic::_dump_object_attributes_to_observer (
      const ObjectStruct &Obj,
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      ObjectObserver &obs) {

   if (AltObjectTypeMask & AttributeMask) {

      ObjectType *ptr (Obj.altTypeTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_alternate_type (
            Obj.uuid,
            Obj.handle,
            AttributeHandle,
            *ptr,
            0);
      }
   }

   if (StateMask & AttributeMask) {

      Mask *ptr (Obj.stateTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_state (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (FlagMask & AttributeMask) {

      Boolean *ptr (Obj.flagTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_flag (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if ((LocalityMask & AttributeMask) && (AttributeHandle == _defaultHandle)) {

      obs.update_object_locality (
         Obj.uuid,
         Obj.handle,
         Obj.locality,
         ObjectLocalityUnknown);
   }

   if (LinkMask & AttributeMask) {

      LinkTable *lt = Obj.subTable.lookup (AttributeHandle);

      if (lt) {

         HashTableHandleIterator it;

         LinkStruct *ptr (lt->get_first (it));

         while (ptr) {

            ObjectStruct *subObj (_objectTable.lookup (ptr->SubHandle));

            if (subObj) {

               obs.link_objects (
                  ptr->LinkHandle,
                  AttributeHandle,
                  Obj.uuid,
                  Obj.handle,
                  subObj->uuid,
                  subObj->handle);
            }

            ptr = lt->get_next (it);
         }
      }
   }

   if (LinkAttributeMask & AttributeMask) {

      LinkTable *lt = Obj.subTable.lookup (AttributeHandle);

      if (lt) {

         const UUID EmptyUUID;

         HashTableHandleIterator it;

         LinkStruct *ptr (lt->get_first (it));

         while (ptr) {

            if (ptr->attrObjectHandle) {

               ObjectStruct *attrObj (_objectTable.lookup (ptr->attrObjectHandle));
               ObjectStruct *subObj (_objectTable.lookup (ptr->SubHandle));

               if (attrObj && subObj) {

                  obs.update_link_attribute_object (
                     ptr->LinkHandle,
                     AttributeHandle,
                     Obj.uuid,
                     Obj.handle,
                     subObj->uuid,
                     subObj->handle,
                     attrObj->uuid,
                     attrObj->handle,
                     EmptyUUID,
                     0);
               }
            }

            ptr = lt->get_next (it);
         }
      }
   }

   if (CounterMask & AttributeMask) {

      CounterStruct *ptr (Obj.counterTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_counter (
            Obj.uuid,
            Obj.handle,
            AttributeHandle,
            ptr->counter,
            0);
      }
   }

   if (MinCounterMask & AttributeMask) {

      CounterStruct *ptr (Obj.counterTable.lookup (AttributeHandle));

      if (ptr && ptr->min) {

         obs.update_object_counter_minimum (
            Obj.uuid,
            Obj.handle,
            AttributeHandle,
            *(ptr->min),
            0);
      }
   }

   if (MaxCounterMask & AttributeMask) {

      CounterStruct *ptr (Obj.counterTable.lookup (AttributeHandle));

      if (ptr && ptr->max) {

         obs.update_object_counter_maximum (
            Obj.uuid,
            Obj.handle,
            AttributeHandle,
            *(ptr->max),
            0);
      }
   }

   if (TimeStampMask & AttributeMask) {

      Float64 *ptr (Obj.timeStampTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_time_stamp (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (PositionMask & AttributeMask) {

      Vector *ptr (Obj.positionTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_position (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (OrientationMask & AttributeMask) {

      Matrix *ptr (Obj.orientationTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_orientation (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (VelocityMask & AttributeMask) {

      Vector *ptr (Obj.velocityTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_velocity (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (AccelerationMask & AttributeMask) {

      Vector *ptr (Obj.accelerationTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_acceleration (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (ScaleMask & AttributeMask) {

      Vector *ptr (Obj.scaleTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_scale (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (VectorMask & AttributeMask) {

      Vector *ptr (Obj.vectorTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_vector (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (ScalarMask & AttributeMask) {

      Float64 *ptr (Obj.scalarTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_scalar (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (TextMask & AttributeMask) {

      String *ptr (Obj.textTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_text (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }

   if (DataMask & AttributeMask) {

      Data *ptr (Obj.dataTable.lookup (AttributeHandle));

      if (ptr) {

         obs.update_object_data (Obj.uuid, Obj.handle, AttributeHandle, *ptr, 0);
      }
   }
}


void
dmz::ObjectModuleBasic::_add_observer_update (ObsUpdateStruct *ptr) {

   if (ptr) {

      if (!_obsUpdateListTail) { _obsUpdateList = _obsUpdateListTail = ptr; }
      else { _obsUpdateListTail->next = ptr; _obsUpdateListTail = ptr; }
   }
}


void
dmz::ObjectModuleBasic::_update_observers () {

   if (!_inStoredObsUpdate && _obsUpdateList) {

      _inStoredObsUpdate = True;

      ObsUpdateStruct *current (_obsUpdateList);

      while (current) {

         current->update (*this);
         current = current->next;
      }

      delete _obsUpdateList; _obsUpdateList = 0;
      _obsUpdateListTail = 0;

      _inStoredObsUpdate = False;
   }
}
//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectModuleBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectModuleBasic (Info);
}

};

