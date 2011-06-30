#ifndef DMZ_QT_OBJECT_INSPECTOR_DOT_H
#define DMZ_QT_OBJECT_INSPECTOR_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QFrame>

class QTreeWidgetItem;


namespace dmz {

   class QtObjectInspector : public QFrame {

      Q_OBJECT

      public:
         QtObjectInspector (
            ObjectObserverUtil &observer,
            RuntimeContext *context,
            QWidget *parent = 0);

         ~QtObjectInspector ();

         void set_state_names (const QStringList &StateList);

         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object ();

         virtual void remove_object_attribute (
            const Handle AttributeHandle,
            const Mask &AttrMask);

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
            const Handle AttributeObjectHandle);

         virtual void update_object_counter (
            const Handle AttributeHandle,
            const Int64 Value);

         virtual void update_object_counter_minimum (
            const Handle AttributeHandle,
            const Int64 Value);

         virtual void update_object_counter_maximum (
            const Handle AttributeHandle,
            const Int64 Value);

         virtual void update_object_alternate_type (
            const Handle AttributeHandle,
            const ObjectType &Value);

         virtual void update_object_state (
            const Handle AttributeHandle,
            const Mask &Value);

         virtual void update_object_flag (
            const Handle AttributeHandle,
            const Boolean Value);

         virtual void update_object_time_stamp (
            const Handle AttributeHandle,
            const Float64 Value);

         virtual void update_object_position (
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_object_orientation (
            const Handle AttributeHandle,
            const Matrix &Value);

         virtual void update_object_velocity (
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_object_acceleration (
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_object_scale (
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_object_vector (
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_object_scalar (
            const Handle AttributeHandle,
            const Float64 Value);

         virtual void update_object_text (
            const Handle AttributeHandle,
            const String &Value);

         virtual void update_object_data (
            const Handle AttributeHandle,
            const Data &Value);

      Q_SIGNALS:
         void finished (const Handle);
         void linkItemClicked (const Handle);

      protected Q_SLOTS:
         void on_treeWidget_itemDoubleClicked (QTreeWidgetItem *item, int column);

         // void on_treeWidget_currentItemChanged (
         //    QTreeWidgetItem *current,
         //    QTreeWidgetItem *previous);

         void on_treeWidget_itemChanged (QTreeWidgetItem *item, int column);

         void _update_object_counter (int value);
         void _update_object_counter_minimum (int value);
         void _update_object_counter_maximum (int value);
         void _update_object_scalar (double value);
         void _update_object_text (const QString &Value);
         void _update_object_position (const Vector &Value);
         void _update_object_velocity (const Vector &Value);
         void _update_object_acceleration (const Vector &Value);
         void _update_object_scale (const Vector &Value);
         void _update_object_vector (const Vector &Value);
         void _update_object_state (const QString &Value);

      protected:
         virtual void closeEvent (QCloseEvent *event);
         void _init ();

         class GroupStruct;
         struct State;
         State &_state;

      private:
         QtObjectInspector ();
         QtObjectInspector (const QtObjectInspector &);
         QtObjectInspector &operator= (const QtObjectInspector &);
   };
};

#endif // DMZ_QT_OBJECT_INSPECTOR_DOT_H
