#ifndef DMZ_QT_PLUGIN_CANVAS_OBJECT_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_OBJECT_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QGraphicsItem>


namespace dmz {

   class QtModuleCanvas;

   class QtCanvasObject : public QGraphicsItem {

      public:
         QtCanvasObject (QGraphicsItem *parent = 0);
         ~QtCanvasObject ();

         virtual QRectF boundingRect () const;

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);
   };

   class QtPluginCanvasObject :
      public Plugin,
      public TimeSlice,
      public ObjectObserverUtil {

      public:
         QtPluginCanvasObject (const PluginInfo &Info, Config &local, Config &global);
         ~QtPluginCanvasObject ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

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

      protected:
         struct ObjectStruct {

            const Handle ObjHandle;
            QtCanvasObject *item;
            Float32 posX;
            Float32 posY;
            Float32 scaleX;
            Float32 scaleY;
            Float32 heading;

            ObjectStruct (const Handle TheHandle) :
               ObjHandle (TheHandle),
               item (new QtCanvasObject ()),
               posX (0.0f),
               posY (0.0f),
               scaleX (1.0f),
               scaleY (1.0f),
               heading (0.0f) {;}

            ~ObjectStruct () { if (item) { delete item; item = 0; } }

            void update ();
         };

         virtual void _store_object_module (ObjectModule &module);
         virtual void _remove_object_module (ObjectModule &module);

         Boolean _find_config_from_type (Config &local, ObjectType &objType);

         void _init (Config &local, Config &global);

         Log _log;
         Definitions _defs;
         PluginContainer _extensions;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _defaultAttributeHandle;
         Handle _linkAttributeHandle;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         HashTableHandleTemplate<ObjectStruct> _updateTable;

      private:
         QtPluginCanvasObject ();
         QtPluginCanvasObject (const QtPluginCanvasObject &);
         QtPluginCanvasObject &operator= (const QtPluginCanvasObject &);
   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_OBJECT_DOT_H
