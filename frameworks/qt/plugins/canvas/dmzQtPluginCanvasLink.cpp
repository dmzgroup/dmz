#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtCanvasConsts.h>
#include "dmzQtPluginCanvasLink.h"
#include <dmzQtModuleCanvas.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToState.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesMath.h>
#include <dmzTypesUUID.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>

dmz::QtCanvasLink::QtCanvasLink (
      const Handle LinkHandle,
      const Handle SuperHandle,
      const Handle SubHandle,
      const Float32 PenWidth,
      QGraphicsItem *parent) :
      QGraphicsLineItem (parent),
      _LinkHandle (LinkHandle),
      _SuperHandle (SuperHandle),
      _SubHandle (SubHandle),
      _rotation (0.0f),
      _arrow1 (0),
      _arrow2 (0) {

   setFlag (ItemIsSelectable, true);

   setData (QtCanvasObjectHandleIndex, (qlonglong)LinkHandle);
   setData (QtCanvasObjectHandleIndex + 1, (qlonglong)SuperHandle);
   setData (QtCanvasObjectHandleIndex + 2, (qlonglong)SubHandle);

   QColor c (Qt::black);
   c.setAlphaF (0.75);
   QBrush b (c);
   setPen (QPen (b, PenWidth));
}


dmz::QtCanvasLink::~QtCanvasLink () {;}


dmz::Handle
dmz::QtCanvasLink::get_link_handle () const { return _LinkHandle; }


void
dmz::QtCanvasLink::set_arrow_state (const Boolean State) {

   if (State) {

      if (!_arrow1) { _arrow1 = new QGraphicsPolygonItem (this); }
      if (!_arrow2) { _arrow2 = new QGraphicsPolygonItem (this); }

      QPolygon poly (4);

      poly.putPoints (0, 4,
         0, -9,
         9, 5,
         0, 0,
         -9, 5);

      if (_arrow1) {

         _arrow1->setPolygon (poly);
         _arrow1->setZValue (zValue () - 1.0f);
         _arrow1->setBrush (QBrush (Qt::SolidPattern));
      }

      if (_arrow2) {

         _arrow2->setPolygon (poly);
         _arrow2->setZValue (zValue () - 1.0f);
         _arrow2->setBrush (QBrush (Qt::SolidPattern));
      }

      setColorAll (pen ());
      _set_arrow_transform ();
   }
   else {

      if (_arrow1) { delete _arrow1; _arrow1 = 0; }
      if (_arrow2) { delete _arrow2; _arrow2 = 0; }
   }
}


void
dmz::QtCanvasLink::set_rotation_offset (const Float32 RotationOffset) {

   _rotation = RotationOffset;
   _set_arrow_transform ();
}


dmz::Float32
local_angle (QPointF p) {

   dmz::Vector v1 (0.0, 1.0, 0.0);
   dmz::Vector v2 ((float)p.x (), (float)p.y (), 0.0);

   return dmz::to_degrees (-v1.get_signed_angle (v2));
}


void
dmz::QtCanvasLink::update (const Handle ObjHandle, const Vector &Value) {

   QPointF point (Value.get_x (), Value.get_z ());

   if (ObjHandle == _SuperHandle) {

      setLine (QLineF (point, line ().p2 ()));
   }
   else if (ObjHandle == _SubHandle) {

      setLine (QLineF (line ().p1 (), point));
   }

   _set_arrow_transform ();
}


void
dmz::QtCanvasLink::setColorAll (QPen pen) {

   setPen (pen);

   if (_arrow1) {

      QPen apen = _arrow1->pen ();
      apen.setColor (pen.color ());
      _arrow1->setPen (apen);
      _arrow1->setBrush (QBrush (pen.color ()));
   }

   if (_arrow2) {

      QPen apen = _arrow2->pen ();
      apen.setColor (pen.color ());
      _arrow2->setPen (apen);
      _arrow2->setBrush (QBrush (pen.color ()));
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

void
dmz::QtCanvasLink::_set_arrow_transform () {

   if (_arrow1 && _arrow2) {

      const float ZValue = zValue () - 1.0f;
      const float Rotate = local_angle (line ().p1 () - line ().p2 ()) + _rotation;
      const QPointF P1 = line ().p1 ();
      const QPointF P2 = line ().p2 ();
      const QPointF Dir = P1 - P2;

      _arrow1->resetTransform ();
      _arrow1->rotate (Rotate);
      _arrow1->setPos ((Dir * 0.3) + P2);
      _arrow1->setZValue (ZValue);

      _arrow2->resetTransform ();
      _arrow2->rotate (Rotate);
      _arrow2->setPos ((Dir * 0.7) + P2);
      _arrow2->setZValue (ZValue);
   }
}


dmz::QtPluginCanvasLink::QtPluginCanvasLink (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _appState (Info),
      _defs (Info, &_log),
      _defaultAttrHandle (0),
      _positionAttrHandle (0),
      _flowAttrHandle (0),
      _canvasModule (0),
      _canvasModuleName (),
      _linkAttrTable (),
      _linkTable (),
      _nodeTable (),
      _stateList (0),
      _penWidth (4.0f) {

   _init (local);
}


dmz::QtPluginCanvasLink::~QtPluginCanvasLink () {

   _attrObjTable.clear ();
   _nodeTable.empty ();
   _linkTable.empty ();

   if (_stateList) { delete _stateList; _stateList = 0; }
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
         _linkTable.empty ();
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

            LinkStruct *os (new LinkStruct (
               LinkHandle,
               AttributeHandle,
               SuperHandle,
               SubHandle,
               _penWidth));

            if (_linkTable.store (LinkHandle, os)) {

               Float32 minZ (qMin (superItem->zValue (), subItem->zValue ()));

               os->item->setZValue (minZ - 1.0f);

               String name ("Link");
               name << "." << AttributeHandle << "." << LinkHandle;

               os->item->setData (QtCanvasObjectNameIndex, name.get_buffer ());

               _store_edge (SuperHandle, os->item);
               _store_edge (SubHandle, os->item);

               _canvasModule->add_item (LinkHandle, os->item);

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

   LinkStruct *ls (_linkTable.remove (LinkHandle));

   if (ls && (ls->AttrHandle == AttributeHandle)) {

      _remove_edge (SuperHandle, ls->item);
      _remove_edge (SubHandle, ls->item);

      if (_canvasModule) { _canvasModule->remove_item (LinkHandle); }

      delete ls; ls = 0;
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

   if (AttributeObjectHandle) {

      LinkStruct *ls = _linkTable.lookup (LinkHandle);

      if (ls) { _attrObjTable.store (AttributeObjectHandle, ls); }

      if (_flowAttrHandle) {

         ObjectModule *objMod (get_object_module ());

         if (objMod) {

            Mask state;

            if (objMod->lookup_state (AttributeObjectHandle, _flowAttrHandle, state)) {

               update_object_state (
                  UUID (),
                  AttributeObjectHandle,
                  _flowAttrHandle,
                  state,
                  0);
            }
         }
      }
   }

   if (PrevAttributeObjectHandle) { _attrObjTable.remove (PrevAttributeObjectHandle); }
}


void
dmz::QtPluginCanvasLink::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   LinkStruct *ls = _attrObjTable.lookup (ObjectHandle);

   if (ls) {

      if (AttributeHandle == _defaultAttrHandle) {

         ColorStruct *current (_stateList);

         ColorStruct *cs (0);

         while (current && !cs) {

            if (Value.contains (current->State)) { cs = current; }
            else { current = current->next; }
         }

         QColor color (Qt::black);
         color.setAlphaF (0.75f);

         if (cs) { color = cs->Color; }

         QBrush b (color);
         ls->item->setColorAll (QPen (b, 4));
      }
      else if (AttributeHandle == _flowAttrHandle) {

         if (Value & _flowStateMask) {

            const Boolean Forward = Value & _forwardState;
            const Boolean Reverse = Value & _reverseState;

            if (Forward && !Reverse) {

               ls->item->set_arrow_state (True);
               ls->item->set_rotation_offset (0.0f);
            }
            else if (!Forward && Reverse) {

               ls->item->set_arrow_state (True);
               ls->item->set_rotation_offset (180.0f);
            }
            else { ls->item->set_arrow_state (False); }
         }
         else { ls->item->set_arrow_state (False); }
      }
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

   RuntimeContext *context = get_plugin_runtime_context ();

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   const String PosAttrName (
      config_to_string ("attribute.position.name", local, ObjectAttributeDefaultName));
   
   _defaultAttrHandle = activate_default_object_attribute (ObjectStateMask);

   _positionAttrHandle = activate_object_attribute (
      PosAttrName,
      ObjectPositionMask);

   const String FlowAttrName = config_to_string ("flow-attribute.name", local);

   if (FlowAttrName) {

      _flowAttrHandle = activate_object_attribute (FlowAttrName, ObjectStateMask);
   }

   _defs.lookup_state (
      config_to_string ("forward-flow-state.name", local),
      _forwardState);

   _defs.lookup_state (
      config_to_string ("reverse-flow-state.name", local),
      _reverseState);

   _flowStateMask = _forwardState | _reverseState;

   Handle attrHandle = activate_object_attribute (
     config_to_string ("link.name", local, ObjectAttributeNodeLinkName),
     ObjectLinkMask | ObjectUnlinkMask | ObjectLinkAttributeMask);

   _linkAttrTable.store (attrHandle, this);

   Config colorList;

   if (local.lookup_all_config ("state-color-list.state-color", colorList)) {

      QColor defaultColor (Qt::black);
      defaultColor.setAlphaF (0.75);

      ConfigIterator it;
      Config color;

      while (colorList.get_prev_config (it, color)) {

         const Mask State = config_to_state ("name", color, context);
         const QColor Value = config_to_qcolor (color, defaultColor);

         if (State) {

            ColorStruct *cs = new ColorStruct (State, Value);
            if (cs) { cs->next = _stateList; _stateList = cs; }
         }
      }
   }

   _penWidth = config_to_float32 ("pen-width.value", local, _penWidth);
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
