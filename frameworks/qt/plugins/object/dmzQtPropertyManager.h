#ifndef DMZ_QT_PROPERTY_MANAGER_H
#define DMZ_QT_PROPERTY_MANAGER_H

#include <dmzTypesVector.h>
#include <QtCore/QMap>
#include "qtpropertybrowser.h"

class QtDoublePropertyManager;
class QtProperty;


namespace dmz {

   class VectorPropertyManagerPrivate;
   
   class VectorPropertyManager : public QtAbstractPropertyManager {
      
   Q_OBJECT
   
      public:
         VectorPropertyManager(QObject *parent = 0);
         ~VectorPropertyManager();

         QtDoublePropertyManager *subDoublePropertyManager() const;

         Vector value(const QtProperty *property) const;
         int decimals(const QtProperty *property) const;

      public Q_SLOTS:
         void setValue(QtProperty *property, const Vector &val);
         void setDecimals(QtProperty *property, int prec);
   
      Q_SIGNALS:
         void valueChanged(QtProperty *property, const Vector &val);
         void decimalsChanged(QtProperty *property, int prec);
   
      protected:
         QString valueText(const QtProperty *property) const;
         virtual void initializeProperty(QtProperty *property);
         virtual void uninitializeProperty(QtProperty *property);
   
      private:
         VectorPropertyManagerPrivate *d_ptr;
         Q_DECLARE_PRIVATE(VectorPropertyManager)
         Q_DISABLE_COPY(VectorPropertyManager)
         Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty *, double))
         Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty *))
   };
   
   class VectorPropertyManagerPrivate {

      VectorPropertyManager *q_ptr;
      Q_DECLARE_PUBLIC(VectorPropertyManager)

      public:

         struct Data {

         Data() : decimals(2) {}
         Vector val;
         int decimals;
      };

      void slotDoubleChanged(QtProperty *property, double value);
      void slotPropertyDestroyed(QtProperty *property);

      typedef QMap<const QtProperty *, Data> PropertyValueMap;
      PropertyValueMap m_values;

      QtDoublePropertyManager *m_doublePropertyManager;

      QMap<const QtProperty *, QtProperty *> m_propertyToX;
      QMap<const QtProperty *, QtProperty *> m_propertyToY;
      QMap<const QtProperty *, QtProperty *> m_propertyToZ;

      QMap<const QtProperty *, QtProperty *> m_xToProperty;
      QMap<const QtProperty *, QtProperty *> m_yToProperty;
      QMap<const QtProperty *, QtProperty *> m_zToProperty;
   };
};


#endif // DMZ_QT_PROPERTY_MANAGER_H
