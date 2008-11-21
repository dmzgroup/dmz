#include <dmzObjectAttributeMasks.h>
#include "dmzQtPluginHistogram.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::QtPluginHistogram::QtPluginHistogram (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _scene (0),
      _view (0),
      _maxCount (0),
      _ascendingOrder (True),
      _barWidth (20.0f),
      _barHeightScale (-10.0),
      _spaceWidth (5.0f) {

   _init (local);
}


dmz::QtPluginHistogram::~QtPluginHistogram () {

   _objTable.empty ();
   _barTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginHistogram::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      QGraphicsRectItem *bar = new QGraphicsRectItem;
      bar->setRect (0.0f, 0.0f, 0.0, -100.0);
      _scene->addItem (bar);

      bar = new QGraphicsRectItem;
      bar->setRect (0.0f, 0.0f, 200.0, 0.0);
      _scene->addItem (bar);
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginHistogram::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void
dmz::QtPluginHistogram::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_typeSet.contains_type (Type)) {

      ObjectStruct *os = new ObjectStruct;

      if (_objTable.store (ObjectHandle, os)) {

         os->bar = _lookup_bar (os->count);

         if (os->bar) {

            os->bar->count++;
            _update_bar (*(os->bar));
         }
      }
      else { delete os; os = 0; }
   }
}


void
dmz::QtPluginHistogram::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objTable.remove (ObjectHandle));

   if (os) {

      if (os->bar) {

         os->bar->count--;
         _update_bar (*(os->bar));
      }

      delete os; os = 0;
   }
}


void
dmz::QtPluginHistogram::link_objects (
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
}


void
dmz::QtPluginHistogram::unlink_objects (
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
}


void
dmz::QtPluginHistogram::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

}


void
dmz::QtPluginHistogram::_update_object_count (const Int32 Value, ObjectStruct &obj) {

   obj.count += Value;

   if (obj.bar) {

      obj.bar->count--;
      _update_bar (*(obj.bar));
   }

   obj.bar = _lookup_bar (obj.count);

   if (obj.bar) {

      obj.bar->count++;
      _update_bar (*(obj.bar));
   }
}


dmz::QtPluginHistogram::BarStruct *
dmz::QtPluginHistogram::_lookup_bar (const Int32 Count) {

   BarStruct *result (0);

   while (Count >= _maxCount) {

      BarStruct *bar = new BarStruct (_maxCount);

      if (!_barTable.store ((UInt32)_maxCount, bar)) { delete bar; bar = 0; }

      _update_graph ();

      _maxCount++;
   }

   result = _barTable.lookup ((UInt32)Count);

   return result;
}


void
dmz::QtPluginHistogram::_update_bar (BarStruct &bar) {

   if (bar.bar) {

      bar.bar->setRect (bar.offset, 0.0, _barWidth, _barHeightScale * (Float32)bar.count);
   }

   if (bar.text) {

      QRectF rect = bar.text->boundingRect ();
      bar.text->setPos (
         bar.offset + ((_barWidth * 0.5) - (rect.width () * 0.5)),
         5.0);
   }
}


void
dmz::QtPluginHistogram::_update_graph () {

   HashTableUInt32Iterator it;

   BarStruct *bar (_ascendingOrder ? _barTable.get_first (it) : _barTable.get_last (it));

   Float32 offset = _spaceWidth;

   while (bar) {

      if (!bar->bar) {

         bar->bar = new QGraphicsRectItem;
         if (_scene) { _scene->addItem (bar->bar); }
      }

      if (!bar->text) {

         bar->text = new QGraphicsTextItem (QString::number (bar->Id));
         if (_scene) { _scene->addItem (bar->text); }
      }

      bar->offset = offset;

      _update_bar (*bar);

      offset += (_barWidth + _spaceWidth);

      bar = (_ascendingOrder ? _barTable.get_next (it) : _barTable.get_prev (it));
   }
}


void
dmz::QtPluginHistogram::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);

   _scene = new QGraphicsScene;
   _view = new QGraphicsView (_scene);
   //_view->setAlignment (Qt::AlignLeft | Qt::AlignBottom);
   _view->resize (400, 300);
   _view->show ();

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
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginHistogram (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginHistogram (Info, local);
}

};
