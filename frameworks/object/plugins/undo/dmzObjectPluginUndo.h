#ifndef DMZ_OBJECT_PLUGIN_UNDO_DOT_H
#define DMZ_OBJECT_PLUGIN_UNDO_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class ObjectPluginUndo :
         public Plugin,
         public MessageObserver,
         public ObjectObserverUtil,
         public UndoObserver {

      public:
         ObjectPluginUndo (const PluginInfo &Info, Config &local);
         ~ObjectPluginUndo ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Message Observer Interface
         void receive_message (
            const MessageType &Msg,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

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

         virtual void update_object_type (
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
            const Float64 &Value,
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

         // UndoObserver Interface
         virtual void update_recording_state (
            const UndoRecordingStateEnum State,
            const UndoRecordingTypeEnum RecordingType,
            const UndoTypeEnum Type);

         virtual void update_current_undo_names (
            const String *NextUndoName,
            const String *NextRedoName);

      protected:
         void _remove_attribute (
            const UUID &Identity,
            const Handle AttrHandle,
            const Mask &RemoveMask);

         void _init (Config &local);

         Definitions _defs;

         MessageType _createObject;
         MessageType _activateObject;
         MessageType _destroyObject;
         MessageType _storeUUID;
         MessageType _removeAttribute;
         MessageType _storeLocality;
         MessageType _linkObjects;
         MessageType _unlinkObjects;
         MessageType _storeLinkAttributeObject;
         MessageType _storeType;
         MessageType _storeState;
         MessageType _storeFlag;
         MessageType _storeTimeStamp;
         MessageType _storePosition;
         MessageType _storeOrientation;
         MessageType _storeVelocity;
         MessageType _storeAcceleration;
         MessageType _storeScale;
         MessageType _storeVector;
         MessageType _storeScalar;
         MessageType _storeText;
         MessageType _storeData;

         Handle _defaultAttrHandle;

         Handle _handleHandle;
         Handle _stringHandle;
         Handle _uuidHandle;
         Handle _valueHandle;
         Handle _maskHandle;
         Handle _booleanHandle;
         Handle _float64Handle;
         Handle _vectorHandle;
         Handle _matrixHandle;

         Boolean _recording;
         Boolean _inDump;

         Undo _undo;
         Log _log;

      private:
         ObjectPluginUndo ();
         ObjectPluginUndo (const ObjectPluginUndo &);
         ObjectPluginUndo &operator= (const ObjectPluginUndo &);
   };
};

#endif // DMZ_OBJECT_PLUGIN_UNDO_DOT_H
