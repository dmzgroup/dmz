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


namespace dmz {

   class QtModuleCanvas;

   class QtCanvasLink : public QGraphicsLineItem {

      public:
         QtCanvasLink (
            const Handle LinkHandle,
            const Handle SuberHandle,
            const Handle SubHandle,
            QGraphicsItem *parent = 0);

         ~QtCanvasLink ();

         Handle get_link_handle () const;

         void update (const Handle ObjHandle, const Vector &Value);

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);

      protected:
         Handle _LinkHandle;
         Handle _SuperHandle;
         Handle _SubHandle;
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
               const Handle TheSubHandle) :
               ObjHandle (TheHandle),
               AttrHandle (TheAttrHandle),
               SuperHandle (TheSuperHandle),
               SubHandle (TheSubHandle),
               item (new QtCanvasLink (ObjHandle, SuperHandle, SubHandle)) {;}

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
         ApplicationStateWrapper _appState;
         Definitions _defs;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _positionAttrHandle;
         HashTableHandle _linkAttrTable;
         HashTableHandleTemplate<LinkStruct> _linkTable;
         HashTableHandleTemplate<NodeStruct> _nodeTable;
         HashTableHandleTemplate<LinkStruct> _attrObjTable;

         ColorStruct *_stateList;

      private:
         QtPluginCanvasLink ();
         QtPluginCanvasLink (const QtPluginCanvasLink &);
         QtPluginCanvasLink &operator= (const QtPluginCanvasLink &);
   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_LINK_DOT_H
