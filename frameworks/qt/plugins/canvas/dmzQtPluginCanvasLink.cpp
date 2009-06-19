#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtCanvasConsts.h>
#include "dmzQtPluginCanvasLink.h"
#include <dmzQtModuleCanvas.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesUUID.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>


dmz::QtCanvasLink::QtCanvasLink (
      const Handle LinkHandle,
      const Handle SuperHandle,
      const Handle SubHandle,
      QGraphicsItem *parent) :
      QGraphicsLineItem (parent),
      _LinkHandle (LinkHandle),
      _SuperHandle (SuperHandle),
      _SubHandle (SubHandle) {

   setFlag (ItemIsSelectable, true);

   setData (QtCanvasObjectHandleIndex, (qlonglong)LinkHandle);
   setData (QtCanvasObjectHandleIndex + 1, (qlonglong)SuperHandle);
   setData (QtCanvasObjectHandleIndex + 2, (qlonglong)SubHandle);

   QColor c (Qt::black);
   c.setAlphaF (0.75);
   QBrush b (c);
   setPen (QPen (b, 4));
}


dmz::QtCanvasLink::~QtCanvasLink () {;}


dmz::Handle
dmz::QtCanvasLink::get_link_handle () const { return _LinkHandle; }


void
dmz::QtCanvasLink::update (const Handle ObjHandle, const Vector &Value) {

   QPointF point (Value.get_x (), Value.get_z ());

   if (ObjHandle == _SuperHandle) {

      setLine (QLineF (point, line ().p2 ()));
   }
   else if (ObjHandle == _SubHandle) {

      setLine (QLineF (line ().p1 (), point));
   }
}


void
dmz::QtCanvasLink::paint (
      QPainter *painter,
      const QStyleOptionGraphicsItem *option,
      QWidget *widget) {

   if (option->state & QStyle::State_Selected) {

      QPen p (pen ());
      p.setWidthF (p.width () * 1.25 * (1.0 / option->levelOfDetail));
      p.setStyle (Qt::DashLine);

      painter->setPen (p);
   }
   else {

      QPen p (pen ());
      p.setWidth (p.width () * (1.0 / option->levelOfDetail));
      painter->setPen (p);
   }

   painter->drawLine (line ());
}


dmz::QtPluginCanvasLink::QtPluginCanvasLink (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _appState (Info),
      _defs (Info, &_log),
      _positionAttrHandle (0),
      _linkAttrObjectType (),
      _canvasModule (0),
      _canvasModuleName (),
      _linkAttrTable (),
      _objectTable (),
      _nodeTable () {

   _init (local);
}


dmz::QtPluginCanvasLink::~QtPluginCanvasLink () {

   _nodeTable.empty ();
   _objectTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginCanvasLink::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _nodeTable.empty ();
         _objectTable.empty ();
         _canvasModule = 0;
      }
   }
}


// Object Observer Interface
void
dmz::QtPluginCanvasLink::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_canvasModule) {

      if (_linkAttrTable.lookup (AttributeHandle)) {

         QGraphicsItem *superItem (_canvasModule->lookup_item (SuperHandle));
         QGraphicsItem *subItem (_canvasModule->lookup_item (SubHandle));

         if (superItem && subItem) {

            ObjectStruct *os (
               new ObjectStruct (LinkHandle, AttributeHandle, SuperHandle, SubHandle));

            if (_objectTable.store (LinkHandle, os)) {

               Float32 minZ (qMin (superItem->zValue (), subItem->zValue ()));

               os->item->setZValue (minZ - 1.0f);

               String name ("Link");
               name << "." << AttributeHandle << "." << LinkHandle;

               os->item->setData (QtCanvasObjectNameIndex, name.get_buffer ());

               _store_edge (SuperHandle, os->item);
               _store_edge (SubHandle, os->item);

               _canvasModule->add_item (LinkHandle, os->item);

               if (_appState.is_mode_normal ()) {

                  _create_link_attribute_object (LinkHandle);
               }

               QGraphicsItem *superParent = superItem->parentItem ();

               if (superParent) { os->item->setParentItem (superParent); }
            }
            else { delete os; os = 0; }
         }
      }
   }
}


void
dmz::QtPluginCanvasLink::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   ObjectStruct *os (_objectTable.remove (LinkHandle));

   if (os && (os->AttrHandle == AttributeHandle)) {

      _remove_edge (SuperHandle, os->item);
      _remove_edge (SubHandle, os->item);

      if (_canvasModule) { _canvasModule->remove_item (LinkHandle); }

      ObjectModule *objMod (get_object_module ());

      if (objMod && os->linkAttrObject) {

         objMod->destroy_object (os->linkAttrObject);
         os->linkAttrObject = 0;
      }

      delete os; os = 0;
   }
}


void
dmz::QtPluginCanvasLink::update_link_attribute_object (
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

   ObjectStruct *os (_objectTable.lookup (LinkHandle));

   if (os && (os->AttrHandle == AttributeHandle)) {

      ObjectModule *objMod (get_object_module ());

      if (objMod && os->linkAttrObject) {

         if (os->linkAttrObject == PrevAttributeObjectHandle) {

            objMod->destroy_object (os->linkAttrObject);
            os->linkAttrObject = AttributeObjectHandle;
         }
      }
      else { os->linkAttrObject = AttributeObjectHandle; }
   }
}


void
dmz::QtPluginCanvasLink::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _positionAttrHandle) {
      
      NodeStruct *node (_nodeTable.lookup (ObjectHandle));

      if (node) {

         HashTableHandleIterator it;
         QtCanvasLink *item (node->edgeTable.get_first (it));

         while (item) {

            item->update (ObjectHandle, Value);
            item = node->edgeTable.get_next (it);
         }
      }
   }
}


void
dmz::QtPluginCanvasLink::_create_link_attribute_object (const Handle LinkHandle) {

   ObjectModule *objMod (get_object_module ());
   ObjectStruct *os (_objectTable.lookup (LinkHandle));

   if (objMod && os) {

      Handle attrObj (objMod->lookup_link_attribute_object (LinkHandle));

      if (!attrObj) {

         attrObj = objMod->create_object (_linkAttrObjectType, ObjectLocal);

         if (attrObj) {

            objMod->activate_object (attrObj);
            objMod->store_link_attribute_object (LinkHandle, attrObj);

            os->linkAttrObject = attrObj;
         }
      }
   }
}


void
dmz::QtPluginCanvasLink::_store_edge (
      const Handle ObjectHandle,
      QtCanvasLink *item) {

   if (item) {

      NodeStruct *node (_nodeTable.lookup (ObjectHandle));

      if (!node) {

         node = new NodeStruct ();
         if (!_nodeTable.store (ObjectHandle, node)) { delete node; node = 0; }
      }

      if (node) {

         node->edgeTable.store (item->get_link_handle (), item);
      }

      ObjectModule *objectModule (get_object_module ());

      if (objectModule) {

         Vector pos;
         objectModule->lookup_position (ObjectHandle, _positionAttrHandle, pos);

         item->update (ObjectHandle, pos);
      }
   }
}


void
dmz::QtPluginCanvasLink::_remove_edge (
      const Handle ObjectHandle,
      QtCanvasLink *item) {

   NodeStruct *node (_nodeTable.lookup (ObjectHandle));

   if (node && item) {

      node->edgeTable.remove (item->get_link_handle ());

      if (!node->edgeTable.get_count ()) {

         _nodeTable.remove (ObjectHandle);
         delete node; node = 0;
      }
   }
}


void
dmz::QtPluginCanvasLink::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   const String PosAttrName (
      config_to_string ("attribute.position.name", local, ObjectAttributeDefaultName));
   
   _positionAttrHandle = activate_object_attribute (
      PosAttrName,
      ObjectPositionMask);

   Handle attrHandle = activate_object_attribute (
     ObjectAttributeNodeLinkName,
     ObjectLinkMask |
     ObjectUnlinkMask |
     ObjectLinkAttributeMask);

  _linkAttrTable.store (attrHandle, this);

  const String TypeName (
     config_to_string ("linkAttributeObjectType.name", local, "na_link_attribute"));

  _defs.lookup_object_type (TypeName, _linkAttrObjectType);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasLink (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasLink (Info, local);
}

};
