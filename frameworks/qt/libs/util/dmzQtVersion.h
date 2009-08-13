#ifndef DMZ_QT_VERSION_DOT_H
#define DMZ_QT_VERSION_DOT_H

#include <dmzQtUtilExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <QtGui/QDialog>

namespace dmz {

   class Config;

   class DMZ_QT_UTIL_LINK_SYMBOL QtVersion : public QDialog {

      Q_OBJECT

      public:
         QtVersion (Config &local, const String &Prefix = "dmz.version");
         virtual ~QtVersion ();

      protected:
         struct State;
         State &_state;
   };
};

#endif // DMZ_QT_VERSION_DOT_H
