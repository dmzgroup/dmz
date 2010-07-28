#include <dmzObjectAttributeMasks.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginGraph.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>
#include <math.h>


dmz::QtPluginGraph::QtPluginGraph (const PluginInfo &Info, Config &local) :
      QFrame (0),
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      QtWidget (Info),
      _log (Info),
      _convert (Info),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _scene (0),
      _xAxis (0),
      _yAxis (0),
      _yLabels (0),
      _powerLawPath (0),
      _powerLabel (0),
      _graphDirty (False),
      _showPowerLaw (False),
      _showPercents (False),
      _rotateLabel (False),
      _countHandle (0),
      _labelHandle (0),
      _maxCount (0),
      _totalCount (0),
      _maxBarCount (0),
      _ascendingOrder (True),
      _barWidth (20.0f),
      _barHeight (130.0f),
      _spaceWidth (5.0f),
      _yDivisions (4),
      _steps (1) {

   _ui.setupUi (this);

   _init (local);
   
   adjustSize ();
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

      QColor white (1.0f, 1.0f, 1.0f, 1.0f);
      QPen pen (white);
      QGraphicsLineItem *spacer = new QGraphicsLineItem (0.0f, 0.0f, -40.0f, 0.0f);
      spacer->setPen (pen);
      spacer->setZValue (-1.0f);
      _scene->addItem (spacer);

      spacer = new QGraphicsLineItem (0.0f, -_barHeight, 0.0f, -(_barHeight + 20.0f));
      spacer->setPen (pen);
      spacer->setZValue (-1.0f);
      _scene->addItem (spacer);

      _xAxis = new QGraphicsLineItem (0.0f, 0.0f, _barWidth + _spaceWidth, 0.0f);
      _xAxis->setZValue (1.0);
      _scene->addItem (_xAxis);

      _yAxis = new QGraphicsLineItem (0.0f, 0.0f, 0.0f, -_barHeight);
      _yAxis->setZValue (1.0);
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

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mainWindowModule &&
            (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         _mainWindowModule = 0;
      }
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

      ObjectStruct *os = new ObjectStruct (ObjectHandle);

      if (_objTable.store (ObjectHandle, os)) {

         _totalCount++;

         os->bar = _lookup_bar (os->count);

         if (os->bar) {

            if (!_countHandle) { os->bar->count++; }
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

         if (_countHandle) {

            os->bar->count -= os->count;

            _graphDirty = True;
         }

         String *ptr = os->bar->remove_label (ObjectHandle);
         if (ptr) { delete ptr; ptr = 0; }
         if (!_countHandle) { os->bar->count--; }
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

      if (AttributeHandle == _countHandle) {

         os->count = (Int32)Value;

         if (os->bar) {

            os->bar->count += (Value - (PreviousValue ? *PreviousValue : 0 ));

            _graphDirty = True;
         }
      }
      else {

         const Int32 Diff (Value - (PreviousValue ? *PreviousValue : 0));

         _update_object_count (Diff, *os);

         _graphDirty = True;
      }
   }
}


void
dmz::QtPluginGraph::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   ObjectStruct *os (_objTable.lookup (ObjectHandle));

   if (os && os->bar) {

      String *ptr = os->bar->labels.lookup (ObjectHandle);

      if (ptr) { *ptr = Value; }
      else {

         ptr = new String (Value);

         if (!os->bar->labels.store (ObjectHandle, ptr)) {

            delete ptr; ptr = 0;
         }
      }

      os->bar->update_label ();
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginGraph::get_qt_widget () { return this; }


void
dmz::QtPluginGraph::on_exportButton_clicked () {

   if (_mainWindowModule) {

      QMainWindow *mainWindow (_mainWindowModule->get_qt_main_window ());

      const char *format = "png";
      const QString Extension = QString::fromAscii (format);
      const QString Filter = tr ("Image files (*.%1)").arg (Extension);

      QImage image;

      do {

         const QString FileName (get_save_file_name_with_extension (
            mainWindow, tr ("Save Image"), QString::null, Filter, Extension));

         if (FileName.isEmpty ()) { break; }

         if (image.isNull ()) {

            QPixmap pixmap = _screen_grab ();

            if (!pixmap.isNull ()) { image = pixmap.toImage (); }
         }

         if (image.save (FileName, format)) {

            if (mainWindow) {

               QString msg = tr ("Saved image %1.").arg (QFileInfo (FileName).fileName ()); 
               mainWindow->statusBar ()->showMessage (msg, 2000);
            }

            break;
          }

          QMessageBox box (
             QMessageBox::Warning,
             tr("Save Image"),
             tr("The file %1 could not be written.").arg (FileName),
             QMessageBox::Retry | QMessageBox::Cancel,
             mainWindow);

           if (box.exec () == QMessageBox::Cancel) { break; }
      } while (True);
   }
}


void
dmz::QtPluginGraph::showEvent (QShowEvent *event) {

   if (_visibleMsg) {

      Data out = _convert.to_data (True);
      _visibleMsg.send (&out);
   }
}


void
dmz::QtPluginGraph::hideEvent (QHideEvent *event) {

   if (_visibleMsg) {

      Data out = _convert.to_data (False);
      _visibleMsg.send (&out);
   }
}


QPixmap
dmz::QtPluginGraph::_screen_grab () {
   
   qApp->setOverrideCursor (QCursor (Qt::WaitCursor));
   
   QPixmap screenGrab (_ui.graphicsView->viewport ()->rect ().size ());
   screenGrab.fill (Qt::white);
   QPainter painter (&screenGrab);
   _ui.graphicsView->render (&painter);
   painter.end ();
   
   qApp->restoreOverrideCursor ();

   return screenGrab;
}


void
dmz::QtPluginGraph::_update_object_count (const Int32 Value, ObjectStruct &obj) {

   obj.count += Value;

   String *label (0);

   if (obj.bar) {

      if (!_countHandle) { obj.bar->count--; }

      label = obj.bar->remove_label (obj.Object);
   }

   obj.bar = _lookup_bar (obj.count - (obj.count % _steps));

   if (obj.bar) {

      if (!_countHandle) { obj.bar->count++; }

      if (label) {

         if (!obj.bar->labels.store (obj.Object, label)) { delete label; label = 0; }
         else { obj.bar->update_label (); }
      }
   }
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

   // Note: _totalCount was _maxBarCount
   const Float32 Percent = _countHandle ?
      ((_maxBarCount > 0) ? (Float32)bar.count / (Float32)_maxBarCount : 0.0f) :
      ((_totalCount > 0) ? (Float32)bar.count / (Float32)_totalCount : 0.0f);

   bar.height = -(_barHeight * Percent);

   if (bar.bar) {

      bar.bar->setRect (bar.offset, 0.0f, _barWidth, bar.height);
   }

   if (bar.text) {

      QRectF rect = bar.text->boundingRect ();

      bar.text->setPos (
         bar.offset + (
            (_barWidth * 0.5) - ((_rotateLabel ? rect.height () : rect.width ()) * 0.5)),
         5.0);
   }

   if (bar.countText) {

      if (_showPercents) {

         bar.countText->setPlainText (QString::number (Percent * 100.0, 'f', 0));
      }
      else {

         bar.countText->setPlainText (QString::number (bar.count));
      }

      QRectF rect = bar.countText->boundingRect ();

      bar.countText->setPos (
         bar.offset + ((_barWidth * 0.5) - (rect.width () * 0.5)),
         bar.height - rect.height ());
   }
}

static inline dmz::Float64
local_power (const dmz::Float64 P, const dmz::Float64 Q, const dmz::Float64 X) {

   return (X <= 0.0 ? P : P * pow (X, Q));
}


void
dmz::QtPluginGraph::_update_power_law (
      const BarStruct *LastBar,
      const Float64 EndOfXAxis) {

   Boolean foundFirstBar (False);
   Float32 offset = _spaceWidth;

   Float64 p (0.0);
   Float64 q (0.0);
   Float64 nonZeroCount (0.0);
   Float64 x (0.0);
   Float64 y (0.0);
   Float64 sumX (0.0);
   Float64 sumY (0.0);
   Float64 sumXY (0.0);
   Float64 sumX2 (0.0);

   HashTableUInt32Iterator it;
   BarStruct *bar (_ascendingOrder ? _barTable.get_first (it) : _barTable.get_last (it));

   while (bar) {

      if (!foundFirstBar && bar->count) { foundFirstBar = True; }

      if (foundFirstBar) {

         if (bar->Id == 0) { x = 1.0; }
         else { x = log (Float64 (bar->Id)); }

         if (bar->height < 0.0f) {

            y = log (-bar->height);
            sumY += y;
            sumX += x;
            sumXY += x * y;
            sumX2 += x * x;
            nonZeroCount++;
         }
      }

      if (LastBar == bar) { bar = 0; }
      else {

         bar = (_ascendingOrder ? _barTable.get_next (it) : _barTable.get_prev (it));
      }
   }

   if (nonZeroCount > 0.0) {

      q = ((nonZeroCount * sumXY) - (sumX * sumY)) /
            ((nonZeroCount * sumX2) - (sumX * sumX));

      p = exp ((sumY - (q * sumX)) / nonZeroCount);
   }

   foundFirstBar = false;
   QPainterPath path;

   bar = (_ascendingOrder ? _barTable.get_first (it) : _barTable.get_last (it));

   const Float32 Offset (_barWidth * 0.5f);

   while (bar) {

      if (!foundFirstBar && bar->count) {

         path.moveTo (bar->offset + Offset, -local_power (p, q, bar->Id));
         foundFirstBar = True;
      }
      else if (foundFirstBar) {

         path.lineTo (bar->offset + Offset, -local_power (p, q, bar->Id));
      }
      
      if (LastBar == bar) { bar = 0; }
      else {

         bar = (_ascendingOrder ? _barTable.get_next (it) : _barTable.get_prev (it));
      }
   }

   if (path.elementCount () > 1) {

      if (!_powerLawPath) {

         _powerLawPath = new QGraphicsPathItem;
         _powerLawPath->setPen (_powerStroke);
         _powerLawPath->setZValue (2.0);
         if (_scene) { _scene->addItem (_powerLawPath); }
      }

      if (_powerLawPath) {

         _powerLawPath->setPath (path);
      }

      if (!_powerLabel) {

         _powerLabel = new QGraphicsTextItem;
         _powerLabel->setPos (260.0, -_barHeight);
         if (_scene) { _scene->addItem (_powerLabel); }
      }

      if (_powerLabel) {

         _powerLabel->setPlainText (
            QString::fromAscii ("Exponent = ") + QString::number (q));
      }
   }
   else if (_scene) {

      if (_powerLawPath) {

         _scene->removeItem (_powerLawPath);
         delete _powerLawPath; _powerLawPath = 0;
      }

      if (_powerLabel) {

         _scene->removeItem (_powerLabel);
         delete _powerLabel; _powerLabel = 0;
      }
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
               bar->bar->setZValue (0.0f);
               if (_scene) { _scene->addItem (bar->bar); }
            }

            if (!bar->text) {

               bar->text = new QGraphicsTextItem (QString::number (bar->Id));
               bar->update_label ();
               bar->text->setZValue (0.0f);

               if (_rotateLabel) {

                  QRectF rect = bar->text->boundingRect ();

                  bar->text->setTransform (
                     QTransform ().rotate (90.0).translate (0, -rect.height ()));
               }

               if (_scene) { _scene->addItem (bar->text); }
            }

            if (!bar->countText) {

               bar->countText = new QGraphicsTextItem (QString::number (bar->count));
               bar->countText->setZValue (0.0f);
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

#if 0
   for (Int32 ix = 0; ix < _yDivisions; ix++) {

      const QString Value (
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
#endif

   if (_showPowerLaw) { _update_power_law (lastBar, offset); }

   if (_scene) {

      _scene->setSceneRect (_scene->itemsBoundingRect ());
      _scene->update (_scene->itemsBoundingRect ());
   }
}



void
dmz::QtPluginGraph::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);
   
   qframe_config_read ("frame", local, this);

   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   _scene = new QGraphicsScene (this);

   _ui.graphicsView->setScene (_scene);
   
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

   _countHandle =
      defs.create_named_handle (config_to_string ("counter-handle.name", local));

   if (_countHandle) {

      activate_object_attribute (_countHandle, ObjectCounterMask);
   }

   _labelHandle =
      defs.create_named_handle (config_to_string ("label-handle.name", local));

   if (_labelHandle) {

      activate_object_attribute (_labelHandle, ObjectTextMask);
   }

   _ascendingOrder = config_to_boolean ("ascending.value", local, _ascendingOrder);

   _maxCount = config_to_int32 ("start.value", local, _maxCount);

   _showPowerLaw = config_to_boolean ("power-law.show", local, _showPowerLaw);
   _showPercents = config_to_boolean ("percents.show", local, _showPercents);
   _rotateLabel = config_to_boolean ("rotate-label.value", local, _rotateLabel);
   _powerStroke = config_to_qpen ("power-law.stroke", local, _powerStroke);
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
      _yLabels[ix]->setZValue (1.0f);

      QRectF rect = _yLabels[ix]->boundingRect ();

      _yLabels[ix]->setPos (-5.0 - rect.width (), Offset - (rect.height () * 0.5f));

      _scene->addItem (_yLabels[ix]);

      QGraphicsLineItem *line = new QGraphicsLineItem (-4.0, Offset, 0.0f, Offset);
      line->setZValue (1.0f);

      _scene->addItem (line);
   }

   _visibleMsg = config_create_message (
      "visible-message.name",
      local,
      "",
      context,
      &_log);
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
