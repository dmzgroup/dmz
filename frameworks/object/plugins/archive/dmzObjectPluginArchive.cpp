#include <dmzArchiveModule.h>
#include "dmzObjectPluginArchive.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeObjectType.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMask.h>
#include <dmzTypesUUID.h>

/*!

\class dmz::ObjectPluginArchive
\ingroup Object
\brief Creates and process archives of objects stored in the ObjectModule.
\details The archive XML format:
\code
<dmz>
<dmzObjectPluginArchive>
   <!-- If name is not specified, the UUID is used in its place -->
   <object type="Type Name" uuid="UUID String" name="unique name">
      <!-- If name is not specified, the default attribute handle is used. -->
      <attributes name="Attribute Name">
         <links>
            <!-- name may be unique name or the UUID -->
            <!-- attribute is optional and may be either name or UUID -->
            <object name="First Sub Object Name" attribute="Attribute Object Name"/>
            <!-- more objects -->
            <object name="Last Sub Object Name" attribute="Attribute Object Name"/>
         </links>
         <type value="Type Name"/>
         <state value="State Names"/>
         <flag value="Boolean Value"/>
         <timestamp value="0.0"/>
         <position x="0.0" y="0.0" z="0.0"/>
         <orientation v0="0.0" v1="0.0" v2="0.0"
                      v3="0.0" v4="0.0" v5="0.0"
                      v6="0.0" v7="0.0" v8="0.0"/>
         <euler x="0.0" y="0.0" z="0.0"/>
         <velocity x="0.0" y="0.0" z="0.0"/>
         <acceleration x="0.0" y="0.0" z="0.0"/>
         <scale x="0.0" y="0.0" z="0.0"/>
         <vector x="0.0" y="0.0" z="0.0"/>
         <scalar value="0.0"/>
         <text value="text value"/>
         <data>
            <attribute name="Attribute Name" type="BaseType">
               <element value="First Value"/>
               <!-- more elements -->
               <element value="Last Value"/>
            </attribute>
            <!-- more data attributes -->
         </data>
      </attributes>
      <!-- more object attributes -->
   </object>
</dmzObjectPluginArchive>
</dmz>
\endcode
The filter XML format:
\code
<dmz>
<dmzObjectPluginArchive>
   <archive name="Archive Name" import="Boolean Value" export="Boolean Value">
      <objects>
         <type name="Object Type Name" exclude="Boolean Value"/>
         <!-- more object types -->
         <type name="Last Object Type Name"/>
      </objects>
      <attribute name="Object Attribute Name" contains="Boolean Value">
         <mask name="Mask Name"/>
         <!-- more mask names -->
      </attribute>
      <state name="State Names" attribute="Attribute Name"/>
      <!-- more object attributes -->
   </archive>
</dmzObjectPluginArchive>
</dmz>
\endcode

*/

using namespace dmz;

namespace {

static Mask
local_config_to_mask (Config config, Log &log) {

   Mask result;

   ConfigIterator it;
   Config mask;
   Boolean error (False);

   while (config.get_next_config (it, mask)) {

      if (mask.get_name () == "mask") {

         result |=
            string_to_object_attribute_mask (config_to_string ("name", mask), &log);
      }
   }

   if (!result && !error) {

      log.info << "Filtering all attribute types." << endl;
      result = ObjectAllMask;
   }

   return result;
}

}


//! \cond
dmz::ObjectPluginArchive::ObjectPluginArchive (
      const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _defs (Info, &_log),
      _defaultHandle (0),
      _currentFilter (0),
      _log (Info) {

   _init (local);
}


dmz::ObjectPluginArchive::~ObjectPluginArchive () {

   _filterTable.empty ();
   _linkTable.empty ();
   _attrTable.empty ();
}


// ArchiveObserver Interface.
void
dmz::ObjectPluginArchive::create_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      _currentFilter = _filterTable.lookup (ArchiveHandle);

      if (_currentFilter && !_currentFilter->exportArchive) { _currentFilter = 0; }

      HandleContainer container;

      objMod->get_object_handles (container);

      Handle object = container.get_first ();

      while (object) {

         Config objArchive = _archive_object (object);

         if (!objArchive.is_empty ()) {

            local.add_config (objArchive);
         }

         object = container.get_next ();
      }

      _currentFilter = 0;
   }
}


void
dmz::ObjectPluginArchive::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   Config objList;

   if (local.lookup_all_config ("object", objList)) {

      _currentFilter = _filterTable.lookup (ArchiveHandle);

      if (_currentFilter && !_currentFilter->importArchive) { _currentFilter = 0; }

      _create_objects (objList);

      _currentFilter = 0;
   }
}


void
dmz::ObjectPluginArchive::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectLinkMask, config)) {

      Config links;

      if (!config.lookup_config ("links", links)) {

         Config tmp ("links");

         links = tmp;
	
         config.add_config (links);
      }

      Config obj ("object");
      obj.store_attribute ("name", SubIdentity.to_string ());

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         const Handle LinkAttrHandle = objMod->lookup_link_attribute_object (LinkHandle);

         UUID uuid;

         if (LinkAttrHandle && objMod->lookup_uuid (LinkAttrHandle, uuid)) {

            obj.store_attribute ("attribute", uuid.to_string ());
         }
      }

      links.add_config (obj);
   }
}


void
dmz::ObjectPluginArchive::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   // Do nothing.
}


void
dmz::ObjectPluginArchive::update_link_attribute_object (
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

   // Do nothing.
}


void
dmz::ObjectPluginArchive::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   Config config;

   if ((AttributeHandle != _defaultHandle) &&
         _get_attr_config (AttributeHandle, ObjectTypeMask, config)) {

      Config type ("type");
      type.store_attribute ("value", Value.get_name ());
      config.add_config (type);
   }
}


void
dmz::ObjectPluginArchive::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   Config config;

   if (Value && _get_attr_config (AttributeHandle, ObjectStateMask, config)) {

      const Mask FilteredValue (_filter_state (AttributeHandle, Value));

      if (FilteredValue) {

         Config state ("state");
         String name;
         _defs.lookup_state_name (FilteredValue, name);

         if (name) {

            state.store_attribute ("value", name);
            config.add_config (state);
         }
      }
   }
}


void
dmz::ObjectPluginArchive::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectFlagMask, config)) {

      config.add_config (boolean_to_config ("flag", "value", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectTimeStampMask, config)) {

      config.add_config (float64_to_config ("timestamp", "value", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectPositionMask, config)) {

      config.add_config (vector_to_config ("position", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectOrientationMask, config)) {

      config.add_config (matrix_to_config ("orientation", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectVelocityMask, config)) {

      config.add_config (vector_to_config ("velocity", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectAccelerationMask, config)) {

      config.add_config (vector_to_config ("acceleration", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectScaleMask, config)) {

      config.add_config (vector_to_config ("scale", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectVectorMask, config)) {

      config.add_config (vector_to_config ("vector", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectScalarMask, config)) {

      config.add_config (float64_to_config ("scalar", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectTextMask, config)) {

      config.add_config (string_to_config ("text", "value", Value));
   }
}


void
dmz::ObjectPluginArchive::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   Config config;

   if (_get_attr_config (AttributeHandle, ObjectDataMask, config)) {

      config.add_config (data_to_config (Value, get_plugin_runtime_context (), &_log));
   }
}


dmz::Config
dmz::ObjectPluginArchive::_archive_object (const Handle ObjectHandle) {

   Config result;

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      Config tmp ("object");

      ObjectType type;

      Boolean archiveObject (True);

      if (objMod->lookup_object_type (ObjectHandle, _defaultHandle, type)) {

         if (_currentFilter) {

            if (_currentFilter->exTypes.get_count () &&
                  _currentFilter->exTypes.contains_type (type)) { archiveObject = False; }

            if (archiveObject && _currentFilter->inTypes.get_count () &&
                  !_currentFilter->inTypes.contains_type (type)) {

               archiveObject = False;
            }
         }
      }
      else { archiveObject = False; }

      if (archiveObject) {

         tmp.store_attribute ("type", type.get_name ());

         UUID uuid;

         if (objMod->lookup_uuid (ObjectHandle, uuid)) {

            tmp.store_attribute ("uuid", uuid.to_string ());
         }

         _currentConfig = result = tmp;

         objMod->dump_all_object_attributes (ObjectHandle, *this);
      }
   }

   _attrTable.empty ();
   _currentConfig.set_config_context (0);

   return result;
}


dmz::Boolean
dmz::ObjectPluginArchive::_get_attr_config (
      const Handle AttrHandle,
      const Mask &AttrMask,
      Config &config) {

   Boolean result (False);

   const Mask FilterMask (_find_attr_filter_mask (AttrHandle));

   if (!FilterMask.contains (AttrMask)) {

      Config *ptr = _attrTable.lookup (AttrHandle);

      if (!ptr) {

         ptr = new Config ("attributes");

         if (ptr) {

            if (AttrHandle != _defaultHandle) {

               const String Name (_defs.lookup_named_handle_name (AttrHandle));

               ptr->store_attribute ("name", Name);
            }

            if (!_attrTable.store (AttrHandle, ptr)) { delete ptr; ptr = 0; }

            if (ptr) { _currentConfig.add_config (*ptr); }
         }
      }

      if (ptr) { config = *ptr; result = True; }
   }

   return result;
}


void
dmz::ObjectPluginArchive::_create_objects (Config &objList) {

   ConfigIterator it;

   Config objData;

   _linkTable.empty ();

   while (objList.get_next_config (it, objData)) { _config_to_object (objData); }

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      HashTableStringIterator objIt;

      ObjectLinkStruct *current (_linkTable.get_first (objIt));

      while (current) {

         HashTableHandleIterator linkIt;

         LinkGroupStruct *ls (current->table.get_first (linkIt));

         while (ls) {

            HashTableStringIterator subIt;

            LinkStruct *link (ls->table.get_first (subIt));

            while (link) {

               ObjectLinkStruct *sub (_linkTable.lookup (link->name));

               if (sub) {

                  const Handle LinkHandle (objMod->link_objects (
                     ls->LinkHandle,
                     current->ObjectHandle,
                     sub->ObjectHandle));

                  if (link->attr) {

                     ObjectLinkStruct *attr (_linkTable.lookup (link->attr));

                     if (attr) {

                        objMod->store_link_attribute_object (
                           LinkHandle,
                           attr->ObjectHandle);
                     }
                  }
               }

               link = ls->table.get_next (subIt);
            }

            ls = current->table.get_next (linkIt);
         }

         current = _linkTable.get_next (objIt);
      }
   }

   _linkTable.empty ();
}


void
dmz::ObjectPluginArchive::_config_to_object (Config &objData) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      const UUID ObjUUID (config_to_string ("uuid", objData));
      String objectName (config_to_string ("name", objData));
      if (!objectName) { objectName = ObjUUID.to_string (); }

      const String TypeName (config_to_string ("type", objData));
      const ObjectType Type (TypeName, get_plugin_runtime_context ());

      Boolean filterObject (False);

      if (_currentFilter) {

         if (_currentFilter->exTypes.get_count () &&
               _currentFilter->exTypes.contains_type (Type)) { filterObject = True; }

         if (!filterObject && _currentFilter->inTypes.get_count () &&
               !_currentFilter->inTypes.contains_type (Type)) {

            filterObject = True;
         }
      }

      if (Type && !filterObject) {

         Handle objectHandle (0);

         if (ObjUUID) { objectHandle = objMod->lookup_handle_from_uuid (ObjUUID); }

         if (objectHandle) {

            objMod->store_object_type (objectHandle,_defaultHandle, Type);
         }
         else { objectHandle = objMod->create_object (Type, ObjectLocal); }

         if (objectHandle) {

            if (ObjUUID) { objMod->store_uuid (objectHandle, ObjUUID); }

            ObjectLinkStruct *links (0);

            if (objectName) {

               links = new ObjectLinkStruct (objectHandle);

               if (links && !_linkTable.store (objectName, links)) {

                  delete links; links = 0;
               }
            }

            Config attrList;

            if (objData.lookup_all_config ("attributes", attrList)) {

               ConfigIterator it;

               Config attrData;

               Boolean found (attrList.get_first_config (it, attrData));

               while (found) {

                  _store_object_attributes (objectHandle, attrData, links);
                  found = attrList.get_next_config (it, attrData);
               }
            }

            objMod->activate_object (objectHandle);
         }
         else {

            _log.error << "Unable to create object of type: " << TypeName << endl;
         }
      }
      else if (!Type) {

         _log.error << "Unable to create object of unknown type: " << TypeName << endl;
      }
      else {

         _log.info << "Filtering object with type: " << TypeName << endl;
      }
   }
}


void
dmz::ObjectPluginArchive::_store_object_attributes (
      const Handle ObjectHandle,
      Config &attrData,
      ObjectLinkStruct *links) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      const String AttributeName (
         config_to_string ("name", attrData, ObjectAttributeDefaultName));

      const Handle AttrHandle (_defs.create_named_handle (AttributeName));

      const Mask FilterMask (_find_attr_filter_mask (AttrHandle));

      ConfigIterator it;

      Config data;

      Boolean found (attrData.get_first_config (it, data));

      while (found) {

         const String DataName (data.get_name ().to_lower ());

         if (DataName == "links") {

            const Boolean Filter (FilterMask.contains (ObjectLinkMask));

            if (!Filter && links) {

               LinkGroupStruct *ls (links->table.lookup (AttrHandle));

               if (!ls) {

                  ls = new LinkGroupStruct (AttrHandle);
                  if (!links->table.store (AttrHandle, ls)) { delete ls; ls = 0; }
               }

               if (ls) {

                  Config linkList;

                  data.lookup_all_config ("object", linkList);

                  ConfigIterator it;
                  Config obj;

                  Boolean found (linkList.get_first_config (it, obj));

                  while (found) {

                     const String Name (config_to_string ("name", obj));
                     LinkStruct *link (new LinkStruct (Name));

                     if (link) {

                        if (ls->table.store (Name, link)) {

                           link->attr = config_to_string ("attribute", obj);
                        }
                        else { delete link; link = 0; }
                     }

                     found = linkList.get_next_config (it, obj);
                  }
               }
            }
            else if (!Filter && !links) {

               _log.error << "Unable to link unnamed object with link attribute type: "
                  << AttributeName << endl;
            }
         }
         else if (DataName == "type") {

            const ObjectType Type (
               config_to_string ("value", data),
               get_plugin_runtime_context ());

            if (!FilterMask.contains (ObjectTypeMask) && Type) {

               objMod->store_object_type (ObjectHandle, AttrHandle, Type);
            }
         }
         else if (DataName == "state") {

            if (!FilterMask.contains (ObjectStateMask)) {

               Mask value;

               _defs.lookup_state (config_to_string ("value", data), value);

               objMod->store_state (ObjectHandle, AttrHandle, value);
            }
         }
         else if (DataName == "flag") {

            if (!FilterMask.contains (ObjectFlagMask)) {

               objMod->store_flag (
                  ObjectHandle,
                  AttrHandle,
                  config_to_boolean ("value", data));
            }
         }
         else if (DataName == "timestamp") {

            if (!FilterMask.contains (ObjectTimeStampMask)) {

               objMod->store_time_stamp (
                  ObjectHandle,
                  AttrHandle,
                  config_to_float64 (data));
            }
         }
         else if (DataName == "position") {

            if (!FilterMask.contains (ObjectPositionMask)) {

               objMod->store_position (ObjectHandle, AttrHandle, config_to_vector (data));
            }
         }
         else if (DataName == "orientation") {

            if (!FilterMask.contains (ObjectOrientationMask)) {

               objMod->store_orientation (
                  ObjectHandle,
                  AttrHandle,
                  config_to_matrix (data));
            }
         }
         else if (DataName == "euler") {

            if (!FilterMask.contains (ObjectOrientationMask)) {

               const Vector Euler (config_to_vector (data));
               const Matrix Value (Euler.get_x (), Euler.get_y (), Euler.get_z ());

               objMod->store_orientation (ObjectHandle, AttrHandle, Value);
            }
         }
         else if (DataName == "velocity") {

            if (!FilterMask.contains (ObjectVelocityMask)) {

               objMod->store_velocity (ObjectHandle, AttrHandle, config_to_vector (data));
            }
         }
         else if (DataName == "acceleration") {

            if (!FilterMask.contains (ObjectAccelerationMask)) {

               objMod->store_acceleration (
                  ObjectHandle,
                  AttrHandle,
                  config_to_vector (data));
            }
         }
         else if (DataName == "scale") {

            if (!FilterMask.contains (ObjectScaleMask)) {

               objMod->store_scale (ObjectHandle, AttrHandle, config_to_vector (data));
            }
         }
         else if (DataName == "vector") {

            if (!FilterMask.contains (ObjectVectorMask)) {

               objMod->store_vector (ObjectHandle, AttrHandle, config_to_vector (data));
            }
         }
         else if (DataName == "scalar") {

            if (!FilterMask.contains (ObjectScalarMask)) {

               Float64 value (config_to_float64 (data));
               objMod->store_scalar (ObjectHandle, AttrHandle, value);
            }
         }
         else if (DataName == "text") {

            if (!FilterMask.contains (ObjectTextMask)) {

               objMod->store_text (ObjectHandle, AttrHandle, config_to_string (data));
            }
         }
         else if (DataName == "data") {

            if (!FilterMask.contains (ObjectDataMask)) {

               Data value;

               if (config_to_data (data, get_plugin_runtime_context (), value, &_log)) {

                  objMod->store_data (ObjectHandle, AttrHandle, value);
               }
            }
         }
         else {

            _log.error << "Unsupported attribute type: " << data.get_name () << endl;
         }

         found = attrData.get_next_config (it, data);
      }
   }
}


dmz::Mask
dmz::ObjectPluginArchive::_find_attr_filter_mask (const Handle AttrHandle) {

   Mask result;

   if (_currentFilter) {

      Mask *maskPtr = _currentFilter->attrTable.lookup (AttrHandle);

      if (!maskPtr) {

         FilterAttrStruct *current (_currentFilter->list);

         if (current) {

            const String AttrName (_defs.lookup_named_handle_name (AttrHandle));

            while (current && !maskPtr) {

               Int32 place (-1);

               if (AttrName.find_sub (current->Name, place)) {

                  maskPtr = new Mask (current->Attr);

                  if (maskPtr && !_currentFilter->attrTable.store (AttrHandle, maskPtr)) {

                     delete maskPtr; maskPtr = 0;
                  }
               }

               current = current->next;
            }
         }
      }

      if (maskPtr) { result = *maskPtr; }
   }

   return result;
}



dmz::Mask
dmz::ObjectPluginArchive::_filter_state (const Handle AttrHandle, const Mask &Value) {

   Mask result (Value);

   if (_currentFilter) {

      Mask *filterMask (_currentFilter->stateTable.lookup (AttrHandle));

      if (filterMask) { result.unset (*filterMask); }
   }

   return result;
}


void
dmz::ObjectPluginArchive::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _defaultHandle = _defs.create_named_handle (ObjectAttributeDefaultName);

   Config filterList;

   if (local.lookup_all_config ("archive", filterList)) {

      ConfigIterator it;
      Config filter;

      while (filterList.get_next_config (it, filter)) {

         const String ArchiveName (config_to_string ("name", filter, ArchiveDefaultName));

         const Handle ArchiveHandle (activate_archive (ArchiveName));

         _log.info << "Activating archive: " << ArchiveName << endl;

         FilterStruct *fs (filter.has_children () ? new FilterStruct : 0);

         if (fs) {

            fs->importArchive = config_to_boolean ("import", filter, True);

            fs->exportArchive = config_to_boolean ("export", filter, True);
            Config objects;

            if (filter.lookup_all_config ("objects.type", objects)) {

               ConfigIterator typesIt;
               Config typeConfig;

               while (objects.get_next_config (typesIt, typeConfig)) {

                  const String Name (config_to_string ("name", typeConfig));
                  const Boolean Exclude (config_to_boolean ("exclude", typeConfig, True));

                  if (Exclude) {

                     if (!fs->exTypes.add_object_type (Name, context)) {

                        _log.error << "Unable to add object type: '" << Name
                           << "' to archive filter for archive: "
                           << _defs.lookup_named_handle_name (ArchiveHandle) << endl;
                     }
                     else {

                        _log.info << "Excluding object type: " << Name << endl;
                     }
                  }
                  else if (!fs->inTypes.add_object_type (Name, context)) {

                     _log.error << "Unable to add object type: '" << Name
                        << "' to archive filter for archive: "
                        << _defs.lookup_named_handle_name (ArchiveHandle) << endl;
                  }
                  else {

                     _log.info << "Including object type: " << Name << endl;
                  }
               }
            }
            else { _log.info << "No object types filtered for: " << ArchiveName << endl; }

            Config attrConfig;

            if (filter.lookup_all_config ("attribute", attrConfig)) {

               ConfigIterator attrIt;
               Config currentAttr;

               while (attrConfig.get_next_config (attrIt, currentAttr)) {

                  const String Name (
                     config_to_string ("name", currentAttr, ObjectAttributeDefaultName));

                  const Boolean Contains (
                     config_to_boolean ("contains", currentAttr, False));

                  const Mask AttrMask (local_config_to_mask (currentAttr, _log));

                  if (Contains) {

                     FilterAttrStruct *fas (new FilterAttrStruct (Name, AttrMask));

                     if (fas) { fas->next = fs->list; fs->list = fas; }
                  }
                  else {

                     const Handle AttrHandle (_defs.create_named_handle (Name));

                     Mask *ptr (new Mask (AttrMask));

                     if (ptr && !fs->attrTable.store (AttrHandle, ptr)) {

                        delete ptr; ptr = 0;
                        _log.error << "Unable to store mask for object attribute: "
                           << Name << ". Possible duplicate?" << endl;
                     }
                  }
               }
            }

            Config stateConfig;

            if (filter.lookup_all_config ("state", stateConfig)) {

               ConfigIterator stateIt;
               Config state;

               while (stateConfig.get_next_config (stateIt, state)) {

                  Handle attrHandle (_defaultHandle);

                  const String AttrName (config_to_string ("attribute", state));

                  if (AttrName) { attrHandle = _defs.create_named_handle (AttrName); }

                  Mask stateMask;
                  _defs.lookup_state (config_to_string ("name", state), stateMask);

                  if (stateMask) {

                     Mask *ptr (fs->stateTable.lookup (attrHandle));

                     if (ptr) { (*ptr) |= stateMask; }
                     else {

                        ptr = new Mask (stateMask);

                        if (ptr && !fs->stateTable.store (attrHandle, ptr)) {

                           delete ptr; ptr = 0;
                        }
                     }
                  }
               }
            }

            if (!_filterTable.store (ArchiveHandle, fs)) {

               delete fs; fs = 0;
               _log.error << "Unable to store filter for archive: " << ArchiveName
                  << ". Possible duplicate?" << endl;
            }
         }
      }
   }
   else {

      _log.info << "Activating default archive" << endl;
      activate_default_archive ();
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginArchive (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginArchive (Info, local);
}

};
