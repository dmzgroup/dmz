#ifndef DMZ_QT_PLUGIN_MAP_PROPERTIES_MAP_ADAPTER_DOT_H
#define DMZ_QT_PLUGIN_MAP_PROPERTIES_MAP_ADAPTER_DOT_H

#include <dmzRuntimeConfig.h>
#include <QtGui/QDialog>
#include "ui_dmzQtPluginMapPropertiesMapAdapter.h"


namespace dmz {

   class QtPluginMapPropertiesMapAdapter : public QDialog {
            
   Q_OBJECT

      public:
         QtPluginMapPropertiesMapAdapter (QWidget *parent = 0);
         ~QtPluginMapPropertiesMapAdapter ();
         
         void from_config (const Config &Data);
         void to_config (Config &data);
         
      protected slots:
         void _store_attribute (Config &data, const String &Key, const QString &Value);
         void _validate_input ();
         
      protected:
         Ui::mapAdapterDialog _ui;
         
      private:
         QtPluginMapPropertiesMapAdapter (const QtPluginMapPropertiesMapAdapter &);
         QtPluginMapPropertiesMapAdapter &operator= (const QtPluginMapPropertiesMapAdapter &);
   };
};


#endif // DMZ_QT_PLUGIN_MAP_PROPERTIES_MAP_ADAPTER_DOT_H
