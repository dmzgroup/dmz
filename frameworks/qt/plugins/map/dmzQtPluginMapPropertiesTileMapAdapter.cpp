#include "dmzQtPluginMapPropertiesTileMapAdapter.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzTypesString.h>
#include <QtGui/QtGui>


dmz::QtPluginMapPropertiesTileMapAdapter::QtPluginMapPropertiesTileMapAdapter (QWidget *parent) :
      QDialog (parent) {

   _ui.setupUi (this);

   connect (
      _ui.nameLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));

   connect (
      _ui.serverLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));

   connect (
      _ui.pathLineEdit, SIGNAL (textChanged (const QString &)),
      this, SLOT (_validate_input ()));
      
   _validate_input ();
}


dmz::QtPluginMapPropertiesTileMapAdapter::~QtPluginMapPropertiesTileMapAdapter () {

}


void
dmz::QtPluginMapPropertiesTileMapAdapter::from_config (const Config &Data) {

   _ui.nameLineEdit->setText (config_to_string ("name", Data).get_buffer ());
   _ui.serverLineEdit->setText (config_to_string ("server", Data).get_buffer ());
   _ui.pathLineEdit->setText (config_to_string ("path", Data).get_buffer ());

   _ui.tileSizeSpinBox->setValue (config_to_int32 ("tileSize", Data));
   _ui.zoomMinSpinBox->setValue (config_to_int32 ("minZoom", Data));
   _ui.zoomMaxSpinBox->setValue (config_to_int32 ("maxZoom", Data));
   
   _validate_input ();
}


void
dmz::QtPluginMapPropertiesTileMapAdapter::to_config (Config &data) {

   String value;
   
   data.store_attribute ("name", qPrintable (_ui.nameLineEdit->text ()));
   data.store_attribute ("server", qPrintable (_ui.serverLineEdit->text ()));
   data.store_attribute ("path", qPrintable (_ui.pathLineEdit->text ()));
   
   value.flush () << _ui.tileSizeSpinBox->value ();
   data.store_attribute ("tileSize", value);

   value.flush () << _ui.zoomMinSpinBox->value ();
   data.store_attribute ("minZoom", value);

   value.flush () << _ui.zoomMaxSpinBox->value ();
   data.store_attribute ("maxZoom", value);
}


void
dmz::QtPluginMapPropertiesTileMapAdapter::_validate_input () {
   
   if (_ui.nameLineEdit->text ().isEmpty () ||
       _ui.serverLineEdit->text ().isEmpty () ||
       _ui.pathLineEdit->text ().isEmpty ()) {

      _ui.buttonBox->button (QDialogButtonBox::Ok)->setEnabled (false);
   }
   else {
    
      _ui.buttonBox->button (QDialogButtonBox::Ok)->setEnabled (true);
   }
}