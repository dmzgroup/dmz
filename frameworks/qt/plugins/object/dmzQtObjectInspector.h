#ifndef DMZ_QT_OBJECT_INSPECTOR_DOT_H
#define DMZ_QT_OBJECT_INSPECTOR_DOT_H

#include <dmzObjectObserverUtil.h>
#include "dmzQtPropertyManager.h"
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QFrame>


class QtProperty;


namespace dmz {

   class QtObjectInspector : public QFrame {

      Q_OBJECT

      public:
         QtObjectInspector (
            const Handle ObjHandle,
            ObjectObserverUtil &observer,
            RuntimeContext *context,
            QWidget *parent = 0);

         ~QtObjectInspector ();
         
         void set_state_names (const QStringList &StateList);

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
            
      Q_SIGNALS:
         void finished (const Handle);

      protected slots:
         void _value_changed (QtProperty *property, const QVariant &value);
         void _value_changed (QtProperty *property, const Vector &Value);
         void _value_changed (QtProperty *property, const Mask &Value);

      protected:
         virtual void closeEvent (QCloseEvent *event);
         void _init ();

         struct GroupStruct;
         struct State;
         State &_state;

      private:
         QtObjectInspector ();
         QtObjectInspector (const QtObjectInspector &);
         QtObjectInspector &operator= (const QtObjectInspector &);
   };
};

#endif // DMZ_QT_OBJECT_INSPECTOR_DOT_H
