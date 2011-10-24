#ifndef DMZ_QT_PLUGIN_CANVAS_LINK_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_LINK_DOT_H

#include <dmzApplicationState.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsPolygonItem>


namespace dmz {

   class QtModuleCanvas;

   class QtCanvasLink : public QGraphicsLineItem {

      public:
         QtCanvasLink (
            const Handle LinkHandle,
            const Handle SuberHandle,
            const Handle SubHandle,
            const Float32 PenWidth,
            const Int32 ArrowSize = 1,
            QGraphicsItem *parent = 0);

         ~QtCanvasLink ();

         Handle get_link_handle () const;

         void set_arrow_multiplier (const Int32 Multiplier);
         void set_arrow_state (const Boolean State);
         void set_rotation_offset (const Float32 RotationOffset);

         void update (const Handle ObjHandle, const Vector &Value);

         void setColorAll (QPen pen);

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);

      protected:
         void _set_arrow_transform ();
         Handle _LinkHandle;
         Handle _SuperHandle;
         Handle _SubHandle;
         Float32 _rotation;
         QGraphicsPolygonItem *_arrow1;
         QGraphicsPolygonItem *_arrow2;
         Int32 _arrowSizeMultiplier;
   };

   class QtPluginCanvasLink :
      public Plugin,
      public ObjectObserverUtil {

      public:
         QtPluginCanvasLink (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasLink ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
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

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
         struct LinkStruct {

            const Handle ObjHandle;
            const Handle AttrHandle;
            const Handle SuperHandle;
            const Handle SubHandle;
            QtCanvasLink *item;

            LinkStruct (
               const Handle TheHandle,
               const Handle TheAttrHandle,
               const Handle TheSuperHandle,
               const Handle TheSubHandle,
               const Float32 PenWidth,
               const Int32 ArrowMultiplier) :
               ObjHandle (TheHandle),
               AttrHandle (TheAttrHandle),
               SuperHandle (TheSuperHandle),
               SubHandle (TheSubHandle),
               item (new QtCanvasLink (ObjHandle, SuperHandle, SubHandle, PenWidth, ArrowMultiplier)) {;}

            ~LinkStruct () { if (item) { delete item; item = 0; } }
         };

         struct NodeStruct {

            HashTableHandleTemplate<QtCanvasLink> edgeTable;
            NodeStruct () : edgeTable () {;}
            ~NodeStruct () { edgeTable.clear (); }
         };

         struct ColorStruct {

            const Mask State;
            const QColor Color;
            ColorStruct *next;

            ColorStruct (const Mask &TheState, const QColor &TheColor) :
                  State (TheState),
                  Color (TheColor),
                  next (0) {;}

            ~ColorStruct () {

               while (next) {

                  ColorStruct *tmp = next;
                  next = next->next;
                  tmp->next = 0;
                  delete tmp; tmp = 0;
               }
            }
         };

         void _lookup_object_position (const Handle ObjectHandle, Vector &pos);

         void _create_link_attribute_object (const Handle LinkHandle);

         void _store_edge (const Handle ObjectHandle, QtCanvasLink *item);
         void _remove_edge (const Handle ObjectHandle, QtCanvasLink *item);

         void _init (Config &local);

         Log _log;
         ApplicationState _appState;
         Definitions _defs;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _defaultAttrHandle;
         Handle _positionAttrHandle;
         Handle _flowAttrHandle;
         Mask _forwardState;
         Mask _reverseState;
         Mask _flowStateMask;
         HashTableHandle _linkAttrTable;
         HashTableHandleTemplate<LinkStruct> _linkTable;
         HashTableHandleTemplate<NodeStruct> _nodeTable;
         HashTableHandleTemplate<LinkStruct> _attrObjTable;

         ColorStruct *_stateList;

         Float32 _penWidth;
         Int32 _arrowMultiplier;

      private:
         QtPluginCanvasLink ();
         QtPluginCanvasLink (const QtPluginCanvasLink &);
         QtPluginCanvasLink &operator= (const QtPluginCanvasLink &);
   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_LINK_DOT_H
