#ifndef DMZ_ARCHIVE_PLUGIN_OBJECT_DOT_H
#define DMZ_ARCHIVE_PLUGIN_OBJECT_DOT_H

#include <dmzArchiveObserverUtil.h>
#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesStringContainer.h>

namespace dmz {

   class ArchivePluginObject :
         public Plugin,
         public ArchiveObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         ArchivePluginObject (const PluginInfo &Info, Config &local);
         ~ArchivePluginObject ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // ArchiveObserver Interface.
         virtual void create_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global);

         virtual void process_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global);

         // ObjectObserver Interface.
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (
            const UUID &Identity,
            const Handle ObjectHandle);

         virtual void update_object_uuid (
            const Handle ObjectHandle,
            const UUID &Identity,
            const UUID &PrevIdentity);

         virtual void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void update_link_attribute_object (
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

         virtual void update_object_counter (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_minimum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_maximum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_alternate_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_acceleration (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scale (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_vector (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         virtual void update_object_data (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value,
            const Data *PreviousValue);

      protected:
         struct FilterAttrStruct {

            const String Name;
            const Mask Attr;
            FilterAttrStruct *next;

            FilterAttrStruct (const String TheName, const Mask TheAttr) :
                  Name (TheName),
                  Attr (TheAttr),
                  next (0) {;}

            ~FilterAttrStruct () { if (next) { delete next; next = 0; } }
         };

         struct FilterStruct {

            Boolean importArchive;
            Boolean exportArchive;
            ObjectTypeSet inTypes;
            ObjectTypeSet exTypes;
            HashTableHandleTemplate<Mask> attrTable;
            HashTableHandleTemplate<Mask> stateTable;
            FilterAttrStruct *list;

            FilterStruct () : importArchive (True), exportArchive (True), list (0) {;}
            ~FilterStruct () {

               if (list) { delete list; list = 0; }
               attrTable.empty ();
               stateTable.empty ();
            }
         };

         struct LinkStruct {

            String name;
            String attr;

            LinkStruct (const String &TheName, const String &TheAttr) :
                  name (TheName),
                  attr (TheAttr) {;}

            LinkStruct (const String &TheName) : name (TheName) {;}
         };

         struct LinkGroupStruct {

            const Handle LinkHandle;
            HashTableStringTemplate<LinkStruct> table;
            LinkGroupStruct (const Handle TheHandle) : LinkHandle (TheHandle) {;}
            ~LinkGroupStruct () { table.empty (); }
         };

         struct ObjectLinkStruct {

            const Handle ObjectHandle;
            HashTableHandleTemplate<LinkGroupStruct> table;

            ObjectLinkStruct (const Handle TheHandle) : ObjectHandle (TheHandle) {;}
            ~ObjectLinkStruct () { table.empty (); }
         };

         Config _archive_object (const Handle ObjectHandle);

         Boolean _get_attr_config (
            const Handle AttrHandle,
            const Mask &AttrMask,
            Config &config);

         void _create_objects (Config &objList);
         void _config_to_object (Config &objData);

         void _store_object_attributes (
            const Handle ObjectHandle,
            Config &attrData,
            ObjectLinkStruct *links);

         Mask _find_attr_filter_mask (const Handle AttrHandle);

         Mask _filter_state (const Handle AttrHandle, const Mask &Value);

         void _init (Config &local);

         Definitions _defs;
         Handle _defaultHandle;

         HashTableHandleTemplate<FilterStruct> _filterTable;
         FilterStruct *_currentFilter;

         HashTableStringTemplate<ObjectLinkStruct> _linkTable;

         Config _currentConfig;
         HashTableHandleTemplate<Config> _attrTable;

         StringContainer _scope;

         Log _log;
         //! \endcond

      private:
         ArchivePluginObject ();
         ArchivePluginObject (const ArchivePluginObject &);
         ArchivePluginObject &operator= (const ArchivePluginObject &);
   };
};

#endif // DMZ_ARCHIVE_PLUGIN_OBJECT_DOT_H
