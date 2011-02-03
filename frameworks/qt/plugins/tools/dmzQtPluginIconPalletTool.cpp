#include "dmzQtPluginIconPalletTool.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>

#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtSvg/QSvgRenderer>

dmz::QtPluginIconPalletTool::QtPluginIconPalletTool (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      QtWidget (Info),
      _log (Info),
      _rc (Info),
      _useSession (True),
      _iconExtent (72.0f) {

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginIconPalletTool::~QtPluginIconPalletTool () {

}


// Plugin Interface
void
dmz::QtPluginIconPalletTool::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context && _useSession) {

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         session.add_config (
            boolean_to_config ("window", "visible", isVisible () ? True : False));

         set_session_config (context, session);
      }
   }
}


void
dmz::QtPluginIconPalletTool::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginIconPalletTool::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   show ();
   raise ();
}


QWidget *
dmz::QtPluginIconPalletTool::get_qt_widget () { return this; }


// QtPluginIconPalletTool Interface
void
dmz::QtPluginIconPalletTool::_add_type (const ObjectType &Type) {

   const String IconResource = config_to_string (
      get_plugin_name () + ".resource",
      Type.get_config());

   const String IconName = _rc.find_file (IconResource);

   if (IconName) {
      
      QFileInfo fi (IconName.get_buffer ());

      const String Name = Type.get_name ();

      if (Name) {

         QImage back (
            (int)_iconExtent,
            (int)_iconExtent,
            QImage::Format_ARGB32_Premultiplied);
            
         QPainter painter (&back);
         painter.setCompositionMode (QPainter::CompositionMode_Source);
         painter.fillRect (back.rect (), Qt::transparent);
         painter.setCompositionMode (QPainter::CompositionMode_SourceOver);

         if (fi.suffix ().toLower () == QLatin1String ("svg")) {
            
            QSvgRenderer qsr (fi.filePath ());
            QRectF size = qsr.viewBoxF ();
            qreal width = size.width ();
            qreal height = size.height ();
            qreal scale = (width > height) ? width : height;
            if (scale <= 0.0f) { scale = 1.0f; }
            scale = _iconExtent / scale;
            width *= scale;
            height *= scale;
            size.setWidth (width);
            size.setHeight (height);
            if (height < _iconExtent) { size.moveTop ((_iconExtent - height) * 0.5f); }
            if (width < _iconExtent) { size.moveLeft ((_iconExtent - width) * 0.5f); }
            qsr.render (&painter, size);
         }
         else {
         
            QRectF target(0.0, 0.0, _iconExtent, _iconExtent);
            QPixmap pixmap(fi.filePath ());
            
            QRectF size = pixmap.rect ();
            qreal width = size.width ();
            qreal height = size.height ();
            qreal scale = (width > height) ? width : height;
            if (scale <= 0.0f) { scale = 1.0f; }
            scale = _iconExtent / scale;
            width *= scale;
            height *= scale;
            size.setWidth (width);
            size.setHeight (height);
            if (height < _iconExtent) { size.moveTop ((_iconExtent - height) * 0.5f); }
            if (width < _iconExtent) { size.moveLeft ((_iconExtent - width) * 0.5f); }
            
            painter.drawPixmap (size, pixmap, pixmap.rect ());
         }
         
         painter.end ();
         QIcon icon;
         icon.addPixmap (QPixmap::fromImage (back));
         QStandardItem *item = new QStandardItem (icon, Name.get_buffer ());
         item->setEditable (false);
         _model.appendRow (item);
      }
   }
   else if (IconResource) {

      _log.error << "Unable to find icon resource: " << IconResource
          << " for object type: " << Type.get_name () << endl;
   }

   RuntimeIterator it;
   ObjectType next;

   while (Type.get_next_child (it, next)) { _add_type (next); }
}


void
dmz::QtPluginIconPalletTool::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Icon_Pallet_Tool",
      context);

   subscribe_to_message (_showMsg);

   _useSession = config_to_boolean ("use-session.value", local, _useSession);

   if (_useSession && context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }

   _ui.iconView->setModel (&_model);

   _add_type (Definitions (context).get_root_object_type ());
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginIconPalletTool (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginIconPalletTool (Info, local);
}

};
