#include "dmzQtPluginMapPropertiesMapAdapter.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzTypesString.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>

dmz::QtPluginMapPropertiesMapAdapter::QtPluginMapPropertiesMapAdapter (QWidget *parent) :
      QDialog (parent) {

   _ui.setupUi (this);

   connect (
      _ui.serverLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));

   connect (
      _ui.typeComboBox, SIGNAL (currentIndexChanged (int)),
      this, SLOT (_validate_input ()));

   connect (
      _ui.pathLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));
   
   connect (
      _ui.mainPathLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));
      
   _validate_input ();
}


dmz::QtPluginMapPropertiesMapAdapter::~QtPluginMapPropertiesMapAdapter () {

}


void
dmz::QtPluginMapPropertiesMapAdapter::from_config (const Config &Data) {

   Int32 index = 0;
   String type = config_to_string ("type", Data).get_lower ();
   if (type == "tile") {
      
      index = 0;
      
      qlineedit_config_read ("path", Data, _ui.pathLineEdit);

      _ui.tileSizeSpinBox->setValue (config_to_int32 ("tileSize", Data));
      _ui.zoomMinSpinBox->setValue (config_to_int32 ("minZoom", Data));
      _ui.zoomMaxSpinBox->setValue (config_to_int32 ("maxZoom", Data));
   }
   else if (type == "wms") {
      
      index = 1;

      qlineedit_config_read ("path", Data, _ui.mainPathLineEdit);
      qlineedit_config_read ("layers", Data, _ui.layersLineEdit);
      qlineedit_config_read ("projection", Data, _ui.projectionLineEdit);
      qlineedit_config_read ("imageFormat", Data, _ui.imageFormatLineEdit);
      qlineedit_config_read ("service", Data, _ui.serviceLineEdit);
      qlineedit_config_read ("styles", Data, _ui.stylesLineEdit);
      qlineedit_config_read ("wms", Data, _ui.wmsLineEdit);
   }
   
   qlineedit_config_read ("name", Data, _ui.nameLineEdit);
   qlineedit_config_read ("server", Data, _ui.serverLineEdit);

   _ui.typeComboBox->setCurrentIndex (index);
   _ui.stackedWidget->setCurrentIndex (index);
   
   _validate_input ();
}


void
dmz::QtPluginMapPropertiesMapAdapter::to_config (Config &data) {

   String value;
   
   QString name = _ui.nameLineEdit->text ();
   if (name.isEmpty ()) { name = _ui.serverLineEdit->text (); }
   
   _store_attribute (data, "name", name);
   _store_attribute (data, "server", _ui.serverLineEdit->text ());
   
   Int32 index = _ui.typeComboBox->currentIndex ();
   if ( index == 0) {
      
      _store_attribute (data, "type", "tile");
      _store_attribute (data, "path", _ui.pathLineEdit->text ());
      _store_attribute (data, "tileSize", QString::number (_ui.tileSizeSpinBox->value ()));
      _store_attribute (data, "minZoom", QString::number (_ui.zoomMinSpinBox->value ()));
      _store_attribute (data, "maxZoom", QString::number (_ui.zoomMaxSpinBox->value ()));
   }
   else if (index == 1) {

      _store_attribute (data, "type", "wms");
      _store_attribute (data, "path", _ui.mainPathLineEdit->text ());
      _store_attribute (data, "layers", _ui.layersLineEdit->text ());
      _store_attribute (data, "projection", _ui.projectionLineEdit->text ());
      _store_attribute (data, "imageFormat", _ui.imageFormatLineEdit->text ());
      _store_attribute (data, "service", _ui.serviceLineEdit->text ());
      _store_attribute (data, "styles", _ui.stylesLineEdit->text ());
      _store_attribute (data, "wms", _ui.wmsLineEdit->text ());
   }
}


void
dmz::QtPluginMapPropertiesMapAdapter::_store_attribute (Config &data, const String &Key, const QString &Value) {

   if (!Value.isEmpty ()) { data.store_attribute (Key, qPrintable (Value)); }
}


void
dmz::QtPluginMapPropertiesMapAdapter::_validate_input () {

   Boolean valid (True);

   if (_ui.serverLineEdit->text ().isEmpty ()) { valid = False; }

   if (_ui.typeComboBox->currentIndex () == 0) {

      if (_ui.pathLineEdit->text ().isEmpty ()) { valid = False; }
   }
   else {

      if (_ui.mainPathLineEdit->text ().isEmpty ()) { valid = False; }
   }
   
   _ui.buttonBox->button (QDialogButtonBox::Ok)->setEnabled (valid);
}

