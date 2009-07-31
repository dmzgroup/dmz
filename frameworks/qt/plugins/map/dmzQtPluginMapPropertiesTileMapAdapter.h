#ifndef DMZ_QT_PLUGIN_MAP_PROPERTIES_TILE_MAP_ADAPTER_DOT_H
#define DMZ_QT_PLUGIN_MAP_PROPERTIES_TILE_MAP_ADAPTER_DOT_H

#include <dmzRuntimeConfig.h>
#include <QtGui/QDialog>
#include "ui_dmzQtPluginMapPropertiesTileMapAdapter.h"


namespace dmz {

   class QtPluginMapPropertiesTileMapAdapter : public QDialog {
            
   Q_OBJECT

      public:
         QtPluginMapPropertiesTileMapAdapter (QWidget *parent = 0);
         ~QtPluginMapPropertiesTileMapAdapter ();
         
         void from_config (const Config &Data);
         void to_config (Config &data);
         
      protected slots:
         void _validate_input ();
         
      protected:
         Ui::tileMapAdapterDialog _ui;
         
      private:
         QtPluginMapPropertiesTileMapAdapter ();
         QtPluginMapPropertiesTileMapAdapter (const QtPluginMapPropertiesTileMapAdapter &);
         QtPluginMapPropertiesTileMapAdapter &operator= (const QtPluginMapPropertiesTileMapAdapter &);
   };
};


#endif // DMZ_QT_PLUGIN_MAP_PROPERTIES_TILE_MAP_ADAPTER_DOT_H
