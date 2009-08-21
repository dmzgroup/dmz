#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzQtPluginCanvasObjectBasic.h"
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
#include <QtSvg/QtSvg>


dmz::QtCanvasObjectGroup::QtCanvasObjectGroup (QGraphicsItem *parent) :
      QGraphicsItem (parent) {

//   setHandlesChildEvents (true);
}


dmz::QtCanvasObjectGroup::~QtCanvasObjectGroup () {;}


QRectF
dmz::QtCanvasObjectGroup::boundingRect () const {

   return QRectF (); //childrenBoundingRect ();
}


void
dmz::QtCanvasObjectGroup::paint (
      QPainter *painter,
      const QStyleOptionGraphicsItem *option,
      QWidget *widget) {

   // QColor color (Qt::black);
   // color.setAlphaF (0.25);
   // painter->setBrush (color);
   // painter->drawRect (boundingRect ());
}


dmz::QtCanvasObjectText::QtCanvasObjectText (QGraphicsItem *parent) :
         QGraphicsItem (parent),
         _alignment (Qt::AlignLeft),
         _drawBackground (True),
         _text (),
         _textColor (Qt::black),
         _outlineColor (Qt::black),
         _backgroundColor (Qt::white),
         _maxLength (0) {

//   setFlag (ItemIsSelectable, true);
}


dmz::QtCanvasObjectText::~QtCanvasObjectText () {;}


void
dmz::QtCanvasObjectText::set_alignment (const Qt::Alignment Alignment) {

   prepareGeometryChange ();
   _alignment = Alignment;
   update ();
}


void
dmz::QtCanvasObjectText::set_text (const QString &Text) {

   prepareGeometryChange ();
   _text = Text;
   
   if ((_maxLength > 0) && (_text.length () > _maxLength)) {
     
      _text.resize (_maxLength);
      _text += QString (3, '.'); 
   }

   update ();
}


void
dmz::QtCanvasObjectText::set_text_color (const QColor &Color) {

   _textColor = Color;
   update ();
}


void
dmz::QtCanvasObjectText::set_outline_color (const QColor &Color) {

   _outlineColor = Color;
   update ();
}


void
dmz::QtCanvasObjectText::set_background_color (const QColor &Color) {

   _backgroundColor = Color;
   update ();
}


void
dmz::QtCanvasObjectText::set_max_length (const int MaxLength) { _maxLength = MaxLength; }


void
dmz::QtCanvasObjectText::enable_background (const Boolean Value) {

   prepareGeometryChange ();
   _drawBackground = Value;
   update ();
}


QRectF
dmz::QtCanvasObjectText::boundingRect () const {

   QRectF rect;

   if (_text.length ()) {

      const int Margin (1);
      rect = _outline_rect ().adjusted (-Margin, -Margin, Margin, Margin);
   }

   return rect;
}


QPainterPath
dmz::QtCanvasObjectText::shape () const {

   QPainterPath path;

   if (_text.length ()) {

      QRectF rect (_outline_rect ());
      path.addRoundRect (rect, _roundness (rect.width ()), _roundness (rect.height ()));
   }

   return path;
}


void
dmz::QtCanvasObjectText::paint (
      QPainter *painter,
      const QStyleOptionGraphicsItem *option,
      QWidget *widget) {

   if (_text.length ()) {

      QRectF rect (_outline_rect ());

      if (_drawBackground) {

         QPen pen (_outlineColor);
         pen.setWidth (2);

         painter->setPen (pen);
         painter->setBrush (_backgroundColor);

         painter->drawRoundRect (
            rect,
            _roundness (rect.width ()), _roundness (rect.height ()));
      }

      painter->setPen (_textColor);
      painter->drawText (rect, Qt::AlignCenter, _text);
   }
}


QRectF
dmz::QtCanvasObjectText::_outline_rect () const {

   QRectF rect;

   if (_text.length ()) {

      const int Padding (8);
      QFontMetricsF metrics = qApp->font ();

      rect.setSize (metrics.size (0, _text));

      if (_drawBackground) {

         rect.adjust (-Padding, -Padding, Padding, Padding);
      }

      if (_alignment == Qt::AlignLeft) {

         rect.translate (0, -rect.center ().y ());
      }
      else if (_alignment == Qt::AlignRight) {

         rect.translate (-rect.width (), -rect.center ().y ());
      }
      else {

         rect.translate (-rect.center ());
      }
   }

   return rect;
}


dmz::Int32
dmz::QtCanvasObjectText::_roundness (const Float32 Size) const {

   Int32 retVal (0);
   const Int32 Diameter (12);
   if (Size) { retVal = 100 * Diameter / Int32 (Size); }
   return retVal;
}


dmz::QtPluginCanvasObjectBasic::QtPluginCanvasObjectBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _defaultAttributeHandle (0),
      _itemIgnoresTransformations (False),
      _zValue (10),
      _canvasModule (0),
      _canvasModuleName (),
      _fileRequestTable (),
      _svgRendererTable (),
      _modelTable (),
      _masterModelTable (),
      _objectTable () {

   _init (local);
}


dmz::QtPluginCanvasObjectBasic::~QtPluginCanvasObjectBasic () {

   _masterModelTable.clear ();
   _modelTable.empty ();
   _objectTable.empty ();
   _svgRendererTable.empty ();
   _fileRequestTable.clear ();
}


// Plugin Interface
void
dmz::QtPluginCanvasObjectBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _objectTable.empty ();
         _canvasModule = 0;
      }
   }
}


// Object Observer Interface
void
dmz::QtPluginCanvasObjectBasic::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   ModelStruct *ms (_get_model_struct (Type));

   if (ms) {

      if (_create_object (ObjectHandle, Type, *ms)) {

         Mask objState;
         _lookup_object_state (ObjectHandle, objState);
         _update_object_state (ObjectHandle, objState, 0);
      }
   }
}


void
dmz::QtPluginCanvasObjectBasic::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _destroy_object (ObjectHandle);
}


void
dmz::QtPluginCanvasObjectBasic::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      _update_object_state (ObjectHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginCanvasObjectBasic::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));
   if (os) {

      QtCanvasObjectTextTable *textTable (os->textTable.lookup (AttributeHandle));

      if (textTable) {

         HashTableStringIterator it;
         QtCanvasObjectText *item (textTable->get_first (it));

         while (item) {

            item->set_text (Value.get_buffer ());
            item = textTable->get_next (it);
         }
      }
   }
}


void
dmz::QtPluginCanvasObjectBasic::process_file (
      const FileCacheResultEnum RequestResult,
      const String &LocalFilePath,
      const String &RequestedFileLocation,
      const String &RequestedFileName) {

   QGraphicsItem *item (
      _fileRequestTable.remove (RequestedFileLocation + RequestedFileName));

   if (item) {

      if (RequestResult == CacheFileFound) {

         QGraphicsPixmapItem *pixmapItem (
            qgraphicsitem_cast<QGraphicsPixmapItem *> (item));

         if (pixmapItem) {

            QPixmap pixmap;

            _log.info << "Loading pixmap file: " << LocalFilePath << endl;

            if (pixmap.load (LocalFilePath.get_buffer ())) {

               pixmapItem->setPixmap (pixmap);
            }
            else {

               _log.error << "Pixmap failed to load: " << LocalFilePath << endl;
            }
         }

         QGraphicsSvgItem *svgItem (qgraphicsitem_cast<QGraphicsSvgItem *> (item));

         if (svgItem) {

            QSvgRenderer *renderer (
               _svgRendererTable.lookup (RequestedFileLocation + RequestedFileName));

            if (!renderer) {

               renderer = new QSvgRenderer ();

               _log.info << "Loading SVG file: " << LocalFilePath << endl;

               if (renderer->load (QString (LocalFilePath.get_buffer ()))) {

                  _svgRendererTable.store (
                     RequestedFileLocation + RequestedFileName, renderer);
               }
               else {

                  _log.error << "SVG failed to load: " << LocalFilePath << endl;
                  delete renderer; renderer = 0;
               }
            }

            if (renderer) { svgItem->setSharedRenderer (renderer); }
         }
      }
      else if (RequestResult == UnknownFileLocation) {

         _log.error << "Unknown file location: "
            << RequestedFileLocation << " " << RequestedFileName << endl;
      }
      else if (RequestResult == FileNotFoundAtLocation) {

         _log.error << "File not found: "
            << RequestedFileLocation << " " << RequestedFileName << endl;
      }
      else {

         _log.error << "Unknown file cache error for: "
            << RequestedFileLocation << " " << RequestedFileName << endl;
      }
   }
   else {

      _log.error << "Unable to process unknown file: " << LocalFilePath << endl;
   }
}


dmz::Boolean
dmz::QtPluginCanvasObjectBasic::_create_object (
      const Handle ObjectHandle,
      const ObjectType &Type,
      ModelStruct &ms) {

   Boolean retVal (False);

   if (_canvasModule) {

      QGraphicsItem *parent (_canvasModule->lookup_item (ObjectHandle));

      if (parent) {

         ObjectStruct  *os (new ObjectStruct (ObjectHandle));

         os->objType = Type;

         os->item = _create_item (*os, parent, ms.itemData);

//         if (!parent->isVisible () && os->item) {
//
//            os->item->hide ();
//         }

         _process_item_text (*os, ms.textData);
         _process_item_switch (*os, ms.switchData);

         if (_objectTable.store (os->ObjHandle, os)) { retVal = True; }
         else { delete os; os = 0; }
      }
   }

   return retVal;
}


void
dmz::QtPluginCanvasObjectBasic::_destroy_object (const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));
   if (os) { delete os; os = 0; }
}


void
dmz::QtPluginCanvasObjectBasic::_update_object_state (
      const Handle ObjectHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

#if 0
      String sName, psName;
      _defs.lookup_state_name (Value, sName);

      _log.error << "_update_object_state: " << ObjectHandle << endl;
      _log.error << "Value: " << sName << endl;

      if (PreviousValue) { _defs.lookup_state_name (*PreviousValue, psName); }
      else { psName = "NULL"; }

      _log.error << "PreviousValue: " << psName << endl;
#endif

      StateGroupStruct *group (os->groupList);

      while (group) {

         Mask newState (Value & group->groupState);

         Mask prevState;
         if (PreviousValue) { prevState = (*PreviousValue) & group->groupState; }

         if (newState || prevState) {

            _update_group_state (*group, newState, prevState);
         }
         else if (group->defaultState) {

            group->defaultState->set_visible (True);
         }

         group = group->next;
      }

      Mask newState (Value & os->defaultGroup.groupState);

      Mask prevState;
      if (PreviousValue) { prevState = (*PreviousValue) & os->defaultGroup.groupState; }

      if (newState || prevState) {

         _update_default_group_state (os->defaultGroup, newState);
      }
      else if (os->defaultGroup.defaultState){

         os->defaultGroup.defaultState->set_visible (True);
      }
   }
}


void
dmz::QtPluginCanvasObjectBasic::_update_group_state (
      StateGroupStruct &group,
      const Mask &Value,
      const Mask &PreviousValue) {

   StateStruct *ssToHide (0);
   StateStruct *ssToShow (0);

   StateStruct *ss (group.stateList);
   Boolean done (False);

   while (ss && !done) {

      if (!ssToShow) {

         if (!Value) {

            ssToShow = group.defaultState;
         }
         else if (ss->State) {

            if (Value.contains (ss->State)) { ssToShow = ss; }
         }
      }

      if (!ssToHide) {

         if (!PreviousValue) {

            ssToHide = group.defaultState;
         }
         else if (ss->State) {

            if (PreviousValue.contains (ss->State)) { ssToHide = ss; }
         }
      }

      if (ssToShow && ssToHide) { done = True; }

      ss = ss->next;
   }

   if (ssToHide) { ssToHide->set_visible (False); }
   else if (group.defaultState) { group.defaultState->set_visible (False); }

   if (ssToShow) { ssToShow->set_visible (True); }
   else if (group.defaultState) { group.defaultState->set_visible (True); }
}


void
dmz::QtPluginCanvasObjectBasic::_update_default_group_state (
      StateGroupStruct &group,
      const Mask &Value) {

   Boolean showDefault (True);
   StateStruct *ssDefault (0);
   StateStruct *ss (group.stateList);

   while (ss) {

      if (!ssDefault && !(ss->State)) {

         ssDefault = ss;
      }

      if (ss->State) {

         if (Value.contains (ss->State)) {

            ss->set_visible (True);
            showDefault = False;
         }
         else {

            ss->set_visible (False);
         }
      }
      else if (!Value) {

         ss->set_visible (True);
         showDefault = False;
      }

      ss = ss->next;
   }

   if (showDefault && ssDefault) {

      ssDefault->set_visible (True);
   }
}


dmz::QtCanvasObjectGroup *
dmz::QtPluginCanvasObjectBasic::_create_item (
      ObjectStruct &os,
      QGraphicsItem *parent,
      const Config &ItemList) {

   QtCanvasObjectGroup *group (0);

   if (parent) {

      group = new QtCanvasObjectGroup (parent);
      
//group->setFlag (QGraphicsItem::ItemIgnoresTransformations, true);

      QGraphicsItem *item (0);

      ConfigIterator it;
      Config cd;
      Float32 z (1.0);

      while (ItemList.get_next_config (it, cd)) {

         const String DataName (cd.get_name ().to_lower ());
         const String ItemName (config_to_string ("name", cd));

         if (DataName == "image") {

            item = _create_image_item (os, group, cd);
            item->setData (QtCanvasObjectHandleIndex, (quint64)os.ObjHandle);
//item->setFlag (QGraphicsItem::ItemIgnoresTransformations, false);
         }
         else if (DataName == "text") {

            item =  _create_text_item (os, group, cd);
            item->setData (QtCanvasObjectHandleIndex, (quint64)os.ObjHandle);
//item->setFlag (QGraphicsItem::ItemIgnoresTransformations, false);
         }
         else if (DataName == "group") {

            item = _create_item (os, group, cd);
//item->setFlag (QGraphicsItem::ItemIgnoresTransformations, true);
         }

         if (item) {

            item->setZValue (z++);

            if (ItemName) {

               String name (ItemName);
               name << "." << os.ObjHandle;

               item->setData (QtCanvasObjectNameIndex, name.get_buffer ());

               os.itemTable.store (ItemName, item);
            }
         }
      }
   }

   return group;
}


QGraphicsItem *
dmz::QtPluginCanvasObjectBasic::_create_image_item (
      ObjectStruct &os,
      QGraphicsItem *parent,
      const Config &Data) {

   QGraphicsItem *item (0);

   const String File (config_to_string ("file", Data).to_lower ());
   QFileInfo fi (File.get_buffer ());

   if (fi.suffix () == QLatin1String ("svg")) {

      item = _create_svg_item (os, parent, Data);
   }
   else {

      item = _create_pixmap_item (os, parent, Data);
   }

   return item;
}


QGraphicsPixmapItem *
dmz::QtPluginCanvasObjectBasic::_create_pixmap_item (
      ObjectStruct &os,
      QGraphicsItem *parent,
      const Config &Data) {

   QGraphicsPixmapItem *item (new QGraphicsPixmapItem (parent));

   if (_file_request (item, Data)) {

      item->setTransformationMode (Qt::SmoothTransformation);

      ConfigIterator it;
      Config cd;

      while (Data.get_next_config (it, cd)) {

         const String DataName (cd.get_name ().to_lower ());

         if (DataName == "offset") {

            Vector vec (config_to_vector (cd));
            item->setOffset (vec.get_x (), vec.get_y ());
         }
         else if (DataName == "translate") {

            Vector vec (config_to_vector (cd));
            item->translate (vec.get_x (), vec.get_y ());
         }
         else if (DataName == "scale") {

            Vector vec (config_to_vector (cd));

            if (vec.get_x () && vec.get_y ()) {

               item->scale (vec.get_x (), vec.get_y ());
            }
         }
      }
   }
   else { delete item; item = 0; }

   return item;
}


QGraphicsSvgItem *
dmz::QtPluginCanvasObjectBasic::_create_svg_item (
      ObjectStruct &os,
      QGraphicsItem *parent,
      const Config &Data) {

   QGraphicsSvgItem *item (new QGraphicsSvgItem (parent));

   if (_file_request (item, Data)) {

      ConfigIterator it;
      Config cd;

      while (Data.get_next_config (it, cd)) {

         const String DataName (cd.get_name ().to_lower ());

         if (DataName == "translate") {

            Vector vec (config_to_vector (cd));
            item->translate (vec.get_x (), vec.get_y ());
         }
         else if (DataName == "scale") {

            Vector vec (config_to_vector (cd));

            if (vec.get_x () && vec.get_y ()) {

               item->scale (vec.get_x (), vec.get_y ());
            }
         }
      }
   }
   else { delete item; item = 0; }

   return item;
}


dmz::QtCanvasObjectText *
dmz::QtPluginCanvasObjectBasic::_create_text_item (
      ObjectStruct &os,
      QGraphicsItem *parent,
      const Config &Data) {

   QtCanvasObjectText *item (new QtCanvasObjectText (parent));

   ConfigIterator it;
   Config cd;

   while (Data.get_next_config (it, cd)) {

      const String DataName (cd.get_name ().to_lower ());

      if (DataName == "text") {

         String text (config_to_string (cd));
         item->set_text (text.get_buffer ());
      }
      else if (DataName == "textcolor") {

         item->set_text_color (config_to_qcolor (cd));
      }
      else if (DataName == "outlinecolor") {

         item->set_outline_color (config_to_qcolor (cd));
      }
      else if (DataName == "backgroundcolor") {

         item->set_background_color (config_to_qcolor (cd));
      }
      else if (DataName == "enablebackground") {

         item->enable_background (config_to_boolean ("value", cd, true));
      }
      else if (DataName == "alignment") {

         String alignmentName (config_to_string (cd).to_lower ());
         if (alignmentName == "center") {  item->set_alignment (Qt::AlignCenter); }
         else if (alignmentName == "right") {  item->set_alignment (Qt::AlignRight); }
         else {  item->set_alignment (Qt::AlignLeft); }
      }
      else if (DataName == "max-length") {

         item->set_max_length (config_to_int32 ("value", cd));
      }
      else if (DataName == "translate") {

         Vector vec (config_to_vector (cd));
         item->translate (vec.get_x (), vec.get_y ());
      }
      else if (DataName == "scale") {

         Vector vec (config_to_vector (cd));

         if (vec.get_x () && vec.get_y ()) {

            item->scale (vec.get_x (), vec.get_y ());
         }
      }
   }

   return item;
}


void
dmz::QtPluginCanvasObjectBasic::_process_item_text (
      ObjectStruct &os,
      const Config &TextList) {

   ConfigIterator it;
   Config cd;

   while (TextList.get_next_config (it, cd)) {

      const String Name (config_to_string ("name", cd));
      const String AttrName (config_to_string ("attribute", cd));
      const Handle AttrHandle (activate_object_attribute (AttrName, ObjectTextMask));

      QGraphicsItem *item (os.itemTable.lookup (Name));

      if (AttrHandle && item) {

         QtCanvasObjectTextTable *textTable (os.textTable.lookup (AttrHandle));

         if (!textTable) {

            textTable = new QtCanvasObjectTextTable ();
            os.textTable.store (AttrHandle, textTable);
         }

         QtCanvasObjectText *text (qgraphicsitem_cast<QtCanvasObjectText *>(item));

         if (textTable && text) {

            textTable->store (Name, text);
         }
      }
   }
}


void
dmz::QtPluginCanvasObjectBasic::_process_item_switch (
      ObjectStruct &os,
      const Config &SwitchList) {

   ConfigIterator it;
   Config cd;

   while (SwitchList.get_next_config (it, cd)) {

      const String DataName (cd.get_name ().to_lower ());

      if (DataName == "state") {

         const String StateName (config_to_string ("name", cd));

         Mask state;
         _defs.lookup_state (StateName, state);

         const String StateGroup (config_to_string ("group", cd));

         _process_item_state (os, state, StateGroup, cd);
      }
      else if (DataName == "flag") {

         const String AttrName (config_to_string ("attribute", cd));
         const Handle AttrHandle (activate_object_attribute (AttrName, ObjectFlagMask));
//         _process_item_flag (os, cd);
      }
   }
}


void
dmz::QtPluginCanvasObjectBasic::_process_item_state (
      ObjectStruct &os,
      const Mask &State,
      const String &GroupName,
      const Config &ItemList) {

   StateGroupStruct *group (os.groupTable.lookup (GroupName));

   if (!group) {

      if (!GroupName) {

         group = &(os.defaultGroup);
      }
      else {

         group = new StateGroupStruct (GroupName);

         if (os.groupTable.store (GroupName, group)) {

            group->next = os.groupList;
            os.groupList = group;
         }
         else { delete group; group = 0; }
      }
   }

   if (group) {

      StateStruct *ss (new StateStruct (State));

      ss->next = group->stateList;
      group->stateList = ss;
      group->groupState |= ss->State;

      if (!(group->defaultState) && !(ss->State)) {

         group->defaultState = ss;
      }

      ConfigIterator it;
      Config cd;

      while (ItemList.get_next_config (it, cd)) {

         const String Name (config_to_string ("name", cd));
         QGraphicsItem *item (os.itemTable.lookup (Name));

         if (item) {

            item->setVisible (False);
            ss->itemTable.store (ss->itemTable.get_count (), item);
         }
      }
   }
}


dmz::Boolean
dmz::QtPluginCanvasObjectBasic::_file_request (QGraphicsItem *item, const Config &Data) {

   Boolean retVal (False);
   const String Url (config_to_string ("url", Data));
   const String File (config_to_string ("file", Data));

   FileCache *fc = FileCache::get_interface (get_plugin_runtime_context ());

   if (File && item && fc) {

      if (_fileRequestTable.store (Url + File, item)) {

         //_log.info << "Adding file request: " << Url << File << endl;
         fc->add_file_request (Url, File, *this);
         retVal = True;
      }
   }

   return retVal;
}


void
dmz::QtPluginCanvasObjectBasic::_lookup_object_state (
      const Handle ObjectHandle,
      Mask &objState) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objectModule->lookup_state (ObjectHandle, _defaultAttributeHandle, objState);
   }
}


void
dmz::QtPluginCanvasObjectBasic::_lookup_object_type (
      const Handle ObjectHandle,
      ObjectType &objType) {

   ObjectModule *objectModule (get_object_module ());

   if (objectModule) {

      objType = objectModule->lookup_object_type (ObjectHandle);
   }
}


dmz::QtPluginCanvasObjectBasic::ModelStruct *
dmz::QtPluginCanvasObjectBasic::_get_model_struct (const ObjectType &ObjType) {

   ModelStruct *retVal (_masterModelTable.lookup (ObjType.get_handle ()));

   if (!retVal) {

      Config local;
      ObjectType currentType (ObjType);

      if (_find_config_from_type (local, currentType)) {

         ModelStruct *ms (_modelTable.lookup (currentType.get_handle ()));

         if (!ms) {

            ms = _config_to_model_struct (local, currentType);

            if (ms) {

               _modelTable.store (ms->ObjType.get_handle (), ms);
            }
         }

         retVal = ms;
      }
   }

   if (retVal) {

      _masterModelTable.store (ObjType.get_handle (), retVal);
   }

   return retVal;
}


dmz::Boolean
dmz::QtPluginCanvasObjectBasic::_find_config_from_type (
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


dmz::QtPluginCanvasObjectBasic::ModelStruct *
dmz::QtPluginCanvasObjectBasic::_config_to_model_struct (
      Config &local,
      const ObjectType &ObjType) {

   ModelStruct *ms (new ModelStruct (ObjType));

   if (local.lookup_all_config_merged ("items", ms->itemData)) {

      local.lookup_all_config_merged ("text", ms->textData);

      local.lookup_all_config_merged ("switch", ms->switchData);
   }
   else { delete ms; ms = 0; }

   return ms;
}


dmz::String
dmz::QtPluginCanvasObjectBasic::_get_object_name (ObjectStruct &os) {

   String name (os.objType.get_name ());

   String stateName;
   _defs.lookup_state_name (os.objState, stateName);
   if (!stateName) { stateName = "Default"; }

   name << "." << stateName << "." << os.ObjHandle;

   return name;
}


void
dmz::QtPluginCanvasObjectBasic::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   _defaultAttributeHandle = activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectStateMask);

   _zValue = config_to_int32 ("defaults.zValue", local, _zValue);

   _itemIgnoresTransformations = config_to_boolean (
      "defaults.itemIgnoresTransformations",
      local,
      _itemIgnoresTransformations);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasObjectBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasObjectBasic (Info, local);
}

};
