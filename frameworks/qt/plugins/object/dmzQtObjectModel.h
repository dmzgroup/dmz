#ifndef DMZ_QT_OBJECT_MODULE_DOT_H
#define DMZ_QT_OBJECT_MODULE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QStandardItemModel>



namespace dmz {
   
   // class ObjectModule;
   
   class QtObjectModel : public QStandardItemModel {

      Q_OBJECT

      public:
         QtObjectModel (RuntimeContext *context, QObject *parent = 0);
         ~QtObjectModel ();
         
         // void set_state_names (const QStringList &StateList);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);
            
         virtual void update_object_uuid (
            const UUID &Identity,
            const UUID &PrevIdentity);

         virtual void remove_object_attribute (
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual void update_object_locality (
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
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_minimum (
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_maximum (
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_alternate_type (
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         virtual void update_object_state (
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_time_stamp (
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_position (
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_velocity (
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_acceleration (
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scale (
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_vector (
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scalar (
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_text (
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         virtual void update_object_data (
            const Handle AttributeHandle,
            const Data &Value,
            const Data *PreviousValue);
            
      protected:
         struct RowStruct;
         struct GroupStruct;
         struct State;
         State &_state;

      private:
         QtObjectModel ();
         QtObjectModel (const QtObjectModel &);
         QtObjectModel &operator= (const QtObjectModel &);
   };
};

#endif // DMZ_QT_OBJECT_MODULE_DOT_H
