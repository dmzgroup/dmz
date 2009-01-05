#include <dmzObjectAttributeMasks.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginGraph.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::QtPluginGraph::QtPluginGraph (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      QtWidget (Info),
      _log (Info),
      _scene (0),
      _view (0),
      _xAxis (0),
      _yAxis (0),
      _yLabels (0),
      _graphDirty (False),
      _maxCount (0),
      _totalCount (0),
      _maxBarCount (0),
      _ascendingOrder (True),
      _barWidth (20.0f),
      _barHeight (130.0f),
      _spaceWidth (5.0f),
      _yDivisions (4),
      _steps (1) {

   _init (local);
}


dmz::QtPluginGraph::~QtPluginGraph () {

   delete []_yLabels; _yLabels = 0;
   _objTable.empty ();
   _barTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginGraph::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      QColor white (1.0, 1.0, 1.0, 1.0);
      QPen pen (white);
      QGraphicsLineItem *spacer = new QGraphicsLineItem (0.0, 0.0, -40.0f, 0.0);
      spacer->setPen (pen);
      _scene->addItem (spacer);

      _xAxis = new QGraphicsLineItem (0.0f, 0.0f, _barWidth + _spaceWidth, 0.0);
      _scene->addItem (_xAxis);

      _yAxis = new QGraphicsLineItem (0.0f, 0.0f, 0.0f, -_barHeight);
      _scene->addItem (_yAxis);
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginGraph::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::QtPluginGraph::update_time_slice (const Float64 TimeDelta) {

   if (_graphDirty) { _update_graph (); _graphDirty = False; }
}


// Object Observer Interface
void
dmz::QtPluginGraph::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_typeSet.contains_type (Type)) {

      ObjectStruct *os = new ObjectStruct;

      if (_objTable.store (ObjectHandle, os)) {

         _totalCount++;

         os->bar = _lookup_bar (os->count);

         if (os->bar) {

            os->bar->count++;
            _graphDirty = True;
         }
      }
      else { delete os; os = 0; }
   }
}


void
dmz::QtPluginGraph::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objTable.remove (ObjectHandle));

   if (os) {

      _totalCount--;

      if (os->bar) {

         os->bar->count--;
         _graphDirty = True;
      }

      delete os; os = 0;
   }
}


void
dmz::QtPluginGraph::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   ObjectStruct *super (_objTable.lookup (SuperHandle));
   ObjectStruct *sub (_objTable.lookup (SubHandle));

   if (super) { _update_object_count (1, *super); }
   if (sub) { _update_object_count (1, *sub); }

   _graphDirty = True;
}


void
dmz::QtPluginGraph::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   ObjectStruct *super (_objTable.lookup (SuperHandle));
   ObjectStruct *sub (_objTable.lookup (SubHandle));

   if (super) { _update_object_count (-1, *super); }
   if (sub) { _update_object_count (-1, *sub); }

   _graphDirty = True;
}


void
dmz::QtPluginGraph::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   ObjectStruct *os (_objTable.lookup (ObjectHandle));

   if (os) {

      const Int32 Diff (Value - (PreviousValue ? *PreviousValue : 0));

      _update_object_count (Diff, *os);

      _graphDirty = True;
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginGraph::get_qt_widget () { return _view; }


void
dmz::QtPluginGraph::_update_object_count (const Int32 Value, ObjectStruct &obj) {

   obj.count += Value;

   if (obj.bar) { obj.bar->count--; }

   obj.bar = _lookup_bar (obj.count - (obj.count % _steps));

   if (obj.bar) { obj.bar->count++; }
}


dmz::QtPluginGraph::BarStruct *
dmz::QtPluginGraph::_lookup_bar (const Int32 Count) {

   BarStruct *result (0);

   while (Count >= _maxCount) {

      BarStruct *bar = new BarStruct (_maxCount);

      if (!_barTable.store ((UInt32)_maxCount, bar)) { delete bar; bar = 0; }

      _maxCount++;
   }

   result = _barTable.lookup ((UInt32)Count);

   return result;
}


void
dmz::QtPluginGraph::_remove_bar (BarStruct &bar) {

   if (bar.bar) {

      if (_scene) { _scene->removeItem (bar.bar); }
      delete (bar.bar); bar.bar = 0;
   }

   if (bar.text) {

      if (_scene) { _scene->removeItem (bar.text); }
      delete (bar.text); bar.text = 0;
   }

   if (bar.countText) {

      if (_scene) { _scene->removeItem (bar.countText); }
      delete (bar.countText); bar.countText = 0;
   }

}


void
dmz::QtPluginGraph::_update_bar (BarStruct &bar) {


   const Float32 Height = ((_maxBarCount > 0) ?
      -(_barHeight * ((Float32)bar.count / (Float32)_maxBarCount)) : 0.0);

   if (bar.bar) {

      bar.bar->setRect (bar.offset, 0.0, _barWidth, Height);
   }

   if (bar.text) {

      QRectF rect = bar.text->boundingRect ();
      bar.text->setPos (
         bar.offset + ((_barWidth * 0.5) - (rect.width () * 0.5)),
         5.0);
   }

   if (bar.countText) {

      QRectF rect = bar.countText->boundingRect ();
      bar.countText->setPlainText (QString::number (bar.count));
      bar.countText->setPos (
         bar.offset + ((_barWidth * 0.5) - (rect.width () * 0.5)),
         Height - rect.height ());
   }
}


void
dmz::QtPluginGraph::_update_graph () {

   HashTableUInt32Iterator it;

   BarStruct *lastBar (0);
   BarStruct *bar (!_ascendingOrder ? _barTable.get_first (it) : _barTable.get_last (it));

   _maxBarCount = 0;

   while (bar) {

      if (bar->count > _maxBarCount) { _maxBarCount = bar->count; }
      if (!lastBar) {

         if (bar->count) { lastBar = bar; bar = 0; }
         else { _remove_bar (*bar); }
      }

      bar = (!_ascendingOrder ? _barTable.get_next (it) : _barTable.get_prev (it));
   }

   bar = (_ascendingOrder ? _barTable.get_first (it) : _barTable.get_last (it));

   Boolean foundFirstBar (False);
   Float32 offset = _spaceWidth;

   while (bar) {

      if (!foundFirstBar && bar->count) { foundFirstBar = True; }

      if (foundFirstBar) {

         if ((bar->Id % _steps) == 0) {

            if (!bar->bar) {

               bar->bar = new QGraphicsRectItem;
               bar->bar->setPen (_barStroke);
               bar->bar->setBrush (_barFill);
               if (_scene) { _scene->addItem (bar->bar); }
            }

            if (!bar->text) {

               bar->text = new QGraphicsTextItem (QString::number (bar->Id));
               if (_scene) { _scene->addItem (bar->text); }
            }

            if (!bar->countText) {

               bar->countText = new QGraphicsTextItem (QString::number (bar->count));
               if (_scene) { _scene->addItem (bar->countText); }
            }

            bar->offset = offset;

            _update_bar (*bar);

            offset += (_barWidth + _spaceWidth);
         }
      }
      else { _remove_bar (*bar); }

      if (bar == lastBar) { bar = 0; }
      else {

         bar = (_ascendingOrder ? _barTable.get_next (it) : _barTable.get_prev (it));
      }
   }

   if (_xAxis) { _xAxis->setLine (0.0f, 0.0f, offset, 0.0f); }


   for (Int32 ix = 0; ix < _yDivisions; ix++) {

      const QString Value (
         // QString::number ((_maxBarCount * (ix + 1)) / _yDivisions) +
         // QString (" " ) +
         ((_totalCount > 0) ?
            QString::number (
               Int32 ((Float32)(_maxBarCount * (ix + 1)) * 100.0f /
               (Float32)(_yDivisions * _totalCount))) : QString ("0")) +
         QString ("%"));

      _yLabels[ix]->setPlainText (Value);

      QRectF rect = _yLabels[ix]->boundingRect ();

      _yLabels[ix]->setPos (
         -5.0  - rect.width (),
         (-_barHeight * ((Float32)(ix + 1)) / (Float32)_yDivisions) -
            (rect.height () * 0.5f));
   }

   if (_scene) { _scene->setSceneRect (_scene->itemsBoundingRect ()); }
}


void
dmz::QtPluginGraph::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);

   _scene = new QGraphicsScene;
   _view = new QGraphicsView (_scene);
   _view->setAlignment (Qt::AlignLeft); // | Qt::AlignBottom);

   _typeSet = config_to_object_type_set ("set", local, context);

   if (_typeSet.get_count () == 0) {

      _log.info << "No object types specified. Using root type." << endl;
      _typeSet.add_object_type (defs.get_root_object_type ());
   }

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);

   Config attrList;

   if (local.lookup_all_config ("attribute", attrList)) {

      ConfigIterator it;

      Config attr;

      while (attrList.get_next_config (it, attr)) {

         const String Type = config_to_string ("type", attr).get_lower ();
         const String AttrName = config_to_string ("name", attr);

         if (AttrName) {

            if (Type == "link") {

               activate_object_attribute (AttrName, ObjectLinkMask | ObjectUnlinkMask);
            }
            else if (Type == "counter") {

               activate_object_attribute (AttrName, ObjectCounterMask);
            }
            else {

               _log.error << "Unknown attribute type: " << Type << endl;
            }
         }
         else {

            _log.error << "Attribute missing name." << endl;
         }
      }
   }

   _ascendingOrder = config_to_boolean ("ascending.value", local, _ascendingOrder);

   _maxCount = config_to_int32 ("start.value", local, _maxCount);

   _barStroke = config_to_qpen ("bar.stroke", local, _barStroke);
   _barFill = config_to_qbrush ("bar.fill", local, _barFill);
   _barWidth = config_to_int32 ("bar.width", local, _barWidth);
   _barHeight = config_to_int32 ("bar.height", local, _barHeight);
   _spaceWidth = config_to_int32 ("bar.space", local, _spaceWidth);
   _steps = config_to_int32 ("bar.steps", local, _steps);

   _yDivisions = config_to_int32 ("bar.divisions", local, _yDivisions);

   _yLabels = new QGraphicsTextItem*[_yDivisions];

   for (Int32 ix = 0; ix < _yDivisions; ix++) {

      const Float32 Offset (-_barHeight * ((Float32)(ix + 1)) / (Float32)_yDivisions);

      _yLabels[ix] = new QGraphicsTextItem;

      _yLabels[ix]->setPlainText (
         QString::number (100 * (ix +1) / _yDivisions) + QString ("%"));

      QRectF rect = _yLabels[ix]->boundingRect ();

      _yLabels[ix]->setPos (-5.0 - rect.width (), Offset - (rect.height () * 0.5f));

      _scene->addItem (_yLabels[ix]);

      QGraphicsLineItem *line = new QGraphicsLineItem (-4.0, Offset, 0.0f, Offset);

      _scene->addItem (line);
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginGraph (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginGraph (Info, local);
}

};