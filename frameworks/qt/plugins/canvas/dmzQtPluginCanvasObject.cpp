#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtCanvasConsts.h>
#include "dmzQtPluginCanvasObject.h"
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMainWindow.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimeSession.h>
#include <dmzTypesMask.h>
#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>
#include <QtGui/QtGui>
#include <QtSvg/QtSvg>


dmz::QtCanvasObject::QtCanvasObject (QGraphicsItem *parent) :
      QGraphicsItem (parent) {

   setFlag (ItemIsSelectable, true);
   setHandlesChildEvents (true);
}


dmz::QtCanvasObject::~QtCanvasObject () {;}


QRectF
dmz::QtCanvasObject::boundingRect () const {

   return childrenBoundingRect ();
}


void
dmz::QtCanvasObject::paint (
      QPainter *painter,
      const QStyleOptionGraphicsItem *option,
      QWidget *widget) {

   if (option->state & QStyle::State_Selected) {

      const qreal pad = 0.5;
      const qreal penWidth = 0;

      const QColor fgcolor = option->palette.windowText().color();

      const QColor bgcolor ( // ensure good contrast against fgcolor
          fgcolor.red()   > 127 ? 0 : 255,
          fgcolor.green() > 127 ? 0 : 255,
          fgcolor.blue()  > 127 ? 0 : 255);

      painter->setPen (QPen (bgcolor, penWidth, Qt::SolidLine));
      painter->setBrush (Qt::NoBrush);
      painter->drawRect (boundingRect ().adjusted (-pad, -pad, pad, pad));

      painter->setPen (QPen (option->palette.windowText(), 0, Qt::DashLine));
      painter->setBrush (Qt::NoBrush);
      painter->drawRect (boundingRect ().adjusted (-pad, -pad, pad, pad));
   }

   // QColor color (Qt::black);
   // color.setAlphaF (0.25);
   // painter->setBrush (color);
   // painter->drawRect (boundingRect ());
}


void
dmz::QtPluginCanvasObject::ObjectStruct::update () {

   if (item) {

      QTransform trans;

      trans.translate (posX, posY);
      trans.rotateRadians (heading);
      if (scaleX && scaleY) { trans.scale (scaleX, scaleY); }

      item->setTransform (trans);
   }
}


dmz::QtPluginCanvasObject::QtPluginCanvasObject (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _extensions (),
      _defaultAttributeHandle (0),
      _canvasModule (0),
      _canvasModuleName (),
      _objectTable (),
      _updateTable () {

   _init (local, global);
}


dmz::QtPluginCanvasObject::~QtPluginCanvasObject () {

   _updateTable.clear ();
   _extensions.remove_plugins ();
   _objectTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginCanvasObject::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _extensions.start_plugins ();
   }
   else if (State == PluginStateStop) {

      _extensions.stop_plugins ();
   }
   else if (State == PluginStateShutdown) {

      _extensions.shutdown_plugins ();
   }
}


void
dmz::QtPluginCanvasObject::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   const String PluginName (PluginPtr ? PluginPtr->get_plugin_name () : "");

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }

      _extensions.discover_external_plugin (PluginPtr);
   }
   else if (Mode == PluginDiscoverRemove) {

      _extensions.remove_external_plugin (PluginPtr);

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _objectTable.empty ();
         _canvasModule = 0;
      }
   }
}


void
dmz::QtPluginCanvasObject::update_time_slice (const Float64 TimeDelta) {

   if (_updateTable.get_count ()) {

      HashTableHandleIterator it;
      ObjectStruct *os (_updateTable.get_first (it));
      while (os) {

         os->update ();
         os = _updateTable.get_next (it);
      }

      _updateTable.clear ();
   }
}


// Object Observer Interface
void
dmz::QtPluginCanvasObject::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   Config data;
   ObjectType currentType (Type);

   if (_find_config_from_type (data, currentType)) {

      String name (currentType.get_name ());
      name << "." << ObjectHandle;

      ObjectStruct *os (new ObjectStruct (ObjectHandle));

      os->item->setData (QtCanvasObjectNameIndex, name.get_buffer ());

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         Vector pos;
         Matrix ori;

         objMod->lookup_position (ObjectHandle, _defaultAttributeHandle, pos);
         objMod->lookup_orientation (ObjectHandle, _defaultAttributeHandle, ori);

         os->posX = pos.get_x ();
         os->posY = pos.get_z ();
         os->heading = get_heading (ori);
         os->update ();
      }

      _objectTable.store (os->ObjHandle, os);

      if (_canvasModule) { _canvasModule->add_item (os->ObjHandle, os->item); }
   }
}


void
dmz::QtPluginCanvasObject::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) {

      _updateTable.remove (ObjectHandle);
      if (_canvasModule) { _canvasModule->remove_item (ObjectHandle); }
      delete os; os = 0;
   }
}


void
dmz::QtPluginCanvasObject::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_canvasModule) {

      if (AttributeHandle == _linkAttributeHandle) {

         QGraphicsItem *superItem (_canvasModule->lookup_item (SuperHandle));
         QGraphicsItem *subItem (_canvasModule->lookup_item (SubHandle));

         if (superItem && subItem) {

            QGraphicsItemGroup *group (
               qgraphicsitem_cast <QGraphicsItemGroup *>(subItem));

            if (group) { superItem->setGroup (group); }
            else { superItem->setParentItem (subItem); }
         }
      }
   }
}


void
dmz::QtPluginCanvasObject::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_canvasModule) {

      if (AttributeHandle == _linkAttributeHandle) {

         QGraphicsItem *superItem (_canvasModule->lookup_item (SuperHandle));
         QGraphicsItem *subItem (_canvasModule->lookup_item (SubHandle));

         if (superItem) {

            QGraphicsItemGroup *group (
               qgraphicsitem_cast <QGraphicsItemGroup *>(subItem));

            if (group) { superItem->setGroup (0); }
            else { superItem->setParentItem (0); }
         }
      }
   }
}


void
dmz::QtPluginCanvasObject::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      ObjectStruct *os (_objectTable.lookup (ObjectHandle));

      if (os) {

         os->posX = Value.get_x ();
         os->posY = Value.get_z ();

         if (!_updateTable.lookup (ObjectHandle)) {

            _updateTable.store (ObjectHandle, os);
         }
      }
   }
}


void
dmz::QtPluginCanvasObject::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      ObjectStruct *os (_objectTable.lookup (ObjectHandle));

      if (os) {

         os->heading = get_heading (Value);

         if (!_updateTable.lookup (ObjectHandle)) {

            _updateTable.store (ObjectHandle, os);
         }
      }
   }
}


void
dmz::QtPluginCanvasObject::_store_object_module (ObjectModule &module) {

   PluginIterator it;

   Plugin *ptr (_extensions.get_first (it));

   while (ptr) {

      ObjectObserver *obs (ObjectObserver::cast (ptr));

      if (obs) {

         obs->store_object_module (module.get_object_module_name (), module);
      }

      ptr = _extensions.get_next (it);
   }
}


void
dmz::QtPluginCanvasObject::_remove_object_module (ObjectModule &module) {

   PluginIterator it;

   Plugin *ptr (_extensions.get_first (it));

   while (ptr) {

      ObjectObserver *obs (ObjectObserver::cast (ptr));

      if (obs) {

         obs->remove_object_module (module.get_object_module_name (), module);
      }

      ptr = _extensions.get_next (it);
   }
}


dmz::Boolean
dmz::QtPluginCanvasObject::_find_config_from_type (
      Config &local,
      ObjectType &objType) {

   const String Name (get_plugin_name ());

   Boolean found (objType.get_config ().lookup_all_config_merged (Name, local));

   if (!found) {

      ObjectType currentType (objType);
      currentType.become_parent ();

      while (currentType && !found) {

         if (currentType.get_config ().lookup_all_config_merged (Name, local)) {

            found = True;
            objType = currentType;
         }

         currentType.become_parent ();
      }
   }

   return found;
}


void
dmz::QtPluginCanvasObject::_init (Config &local, Config &global) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   Config pluginList;

   if (local.lookup_all_config ("plugins.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (dmz::load_plugins (context, pluginList, local, global, _extensions, &_log)) {

         _extensions.discover_plugins ();
      }
   }

   _defaultAttributeHandle = activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectPositionMask |
      ObjectOrientationMask);

   _linkAttributeHandle = activate_object_attribute (
      ObjectAttributeLayerLinkName,
      ObjectLinkMask | ObjectUnlinkMask);

#if 0
   Config preLoadList;

   if (local.lookup_all_config ("preload", preLoadList)) {

      Config data;
      ConfigIterator it;

      Boolean done (!preLoadList.get_first_config (it, data));

      while (!done) {

         ObjectType objType;
         Mask objState;

         if (_defs.lookup_object_type (config_to_string ("type", data), objType)) {

            _defs.lookup_state (config_to_string ("state", data), objState);

            _log.info << "Pre-Loading object of type: " << objType.get_name () << endl;
            _get_model_struct (objType, objState);
         }

         done = !preLoadList.get_next_config (it, data);
      }
   }
#endif
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasObject (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasObject (Info, local, global);
}

};
