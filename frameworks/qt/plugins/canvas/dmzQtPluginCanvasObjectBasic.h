#ifndef DMZ_QT_PLUGIN_CANVAS_OBJECT_BASIC_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_OBJECT_BASIC_DOT_H

#include <dmzFileCache.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeObjectType.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <QtGui/QGraphicsItem>

class QGraphicsPixmapItem;
class QGraphicsSvgItem;
class QSvgRenderer;


namespace dmz {

   class QtModuleCanvas;

   class QtCanvasObjectGroup : public QGraphicsItem {

      public:
         QtCanvasObjectGroup (QGraphicsItem *parent = 0);
         ~QtCanvasObjectGroup ();

         virtual QRectF boundingRect () const;

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);
   };

   class QtCanvasObjectText : public QGraphicsItem {

      public:
         QtCanvasObjectText (QGraphicsItem *parent = 0);
         ~QtCanvasObjectText ();

         void set_alignment (const Qt::Alignment Alignment);
         void set_text (const QString &Text);
         void set_text_color (const QColor &Color);
         void set_outline_color (const QColor &Color);
         void set_background_color (const QColor &Color);
         void enable_background (const Boolean Value);

         // QGraphicsItem Interface
         virtual QRectF boundingRect () const;
         virtual QPainterPath shape () const;

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);

         protected:
            QRectF _outline_rect () const;
            Int32 _roundness (const Float32 Size) const;

            Qt::Alignment _alignment;
            Boolean _drawBackground;
            QString _text;
            QColor _textColor;
            QColor _outlineColor;
            QColor _backgroundColor;
   };


   class QtPluginCanvasObjectBasic :
      public Plugin,
      public ObjectObserverUtil,
      public FileCacheAction {

      public:
         QtPluginCanvasObjectBasic (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasObjectBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

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

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         // FileCacheAction Interface
         virtual void process_file (
            const FileCacheResultEnum RequestResult,
            const String &LocalFilePath,
            const String &RequestedFileLocation,
            const String &RequestedFileName);

      protected:
         struct ModelStruct {

            const ObjectType ObjType;
            Config itemData;
            Config textData;
            Config switchData;

            ModelStruct (const ObjectType &TheType) :
               ObjType (TheType),
               itemData (),
               textData (),
               switchData () {;}

            ~ModelStruct () {;}
         };

         typedef HashTableStringTemplate<QtCanvasObjectText> QtCanvasObjectTextTable;

         struct StateStruct {

            const Mask State;
            HashTableUInt32Template<QGraphicsItem> itemTable;
            StateStruct *next;

            StateStruct (const Mask &TheState) :
               State (TheState),
               itemTable (),
               next (0) {;}

            ~StateStruct () {

               itemTable.clear ();
               if (next) { delete next; next = 0; }
            }

            void set_visible (const Boolean Value) {

               HashTableUInt32Iterator it;
               QGraphicsItem *item (itemTable.get_first (it));
               while (item) {

                  item->setVisible (Value);
                  item = itemTable.get_next (it);
               }
            }
         };

         struct StateGroupStruct {

            const String Name;
            Mask groupState;
            StateStruct *stateList;
            StateStruct *defaultState;
            StateGroupStruct *next;

            StateGroupStruct (const String &TheName) :
               Name (TheName),
               groupState (),
               stateList (0),
               defaultState (0),
               next (0) {;}

            ~StateGroupStruct () {

               defaultState = 0;
               if (stateList) { delete stateList; }
               if (next) { delete next; next = 0; }
            }
         };

         struct ObjectStruct {

            const Handle ObjHandle;
            ObjectType objType;
            Mask objState;
            QtCanvasObjectGroup *item;
            HashTableStringTemplate<QGraphicsItem> itemTable;
            HashTableHandleTemplate<QtCanvasObjectTextTable> textTable;
            StateGroupStruct defaultGroup;
            StateGroupStruct *groupList;
            HashTableStringTemplate<StateGroupStruct> groupTable;

            ObjectStruct (const Handle TheHandle) :
               ObjHandle (TheHandle),
               objType (),
               objState (),
               item (0),
               itemTable (),
               textTable (),
               defaultGroup (""),
               groupList (0),
               groupTable () {;}

            ~ObjectStruct () {

               groupTable.clear ();
               itemTable.clear ();
               textTable.empty ();

               if (groupList) { delete groupList; groupList = 0; }
               //if (item) { delete item; item = 0; }
               item = 0;
            }
         };

         Boolean _create_object (
            const Handle ObjectHandle,
            const ObjectType &Type,
            ModelStruct &ms);

         void _destroy_object (const Handle ObjectHandle);

         void _update_object_state (
            const Handle ObjectHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         void _update_group_state (
            StateGroupStruct &group,
            const Mask &Value,
            const Mask &PreviousValue);

         void _update_default_group_state (StateGroupStruct &group, const Mask &Value);

         QtCanvasObjectGroup *_create_item (
            ObjectStruct &os,
            QGraphicsItem *parent,
            const Config &itemList);

         QGraphicsItem *_create_image_item (
               ObjectStruct &os,
               QGraphicsItem *parent,
               const Config &Data);

         QGraphicsPixmapItem *_create_pixmap_item (
            ObjectStruct &os,
            QGraphicsItem *parent,
            const Config &Data);

         QGraphicsSvgItem *_create_svg_item (
            ObjectStruct &os,
            QGraphicsItem *parent,
            const Config &Data);

         QtCanvasObjectText *_create_text_item (
            ObjectStruct &os,
            QGraphicsItem *parent,
            const Config &Data);

         void _process_item_text (ObjectStruct &os, const Config &TextList);

         void _process_item_switch (
            ObjectStruct &os,
            const Config &SwitchList);

         void _process_item_state (
            ObjectStruct &os,
            const Mask &State,
            const String &GroupName,
            const Config &ItemList);

         Boolean _file_request (QGraphicsItem *item, const Config &Data);

         void _lookup_object_state (const Handle ObjectHandle, Mask &objState);
         void _lookup_object_type (const Handle ObjectHandle, ObjectType &objType);

         ModelStruct *_get_model_struct (const ObjectType &ObjType);

         Boolean _find_config_from_type (Config &local, ObjectType &objType);

         ModelStruct *_config_to_model_struct (
            Config &local,
            const ObjectType &ObjType);

         String _get_object_name (ObjectStruct &os);

         void _init (Config &local);

         Log _log;
         Definitions _defs;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _defaultAttributeHandle;
         Boolean _itemIgnoresTransformations;
         Int32 _zValue;
         HashTableStringTemplate<QGraphicsItem> _fileRequestTable;
         HashTableStringTemplate<QSvgRenderer> _svgRendererTable;
         HashTableHandleTemplate<ModelStruct> _modelTable;
         HashTableHandleTemplate<ModelStruct> _masterModelTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;

      private:
         QtPluginCanvasObjectBasic ();
         QtPluginCanvasObjectBasic (const QtPluginCanvasObjectBasic &);
         QtPluginCanvasObjectBasic &operator= (const QtPluginCanvasObjectBasic &);
   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_OBJECT_BASIC_DOT_H
