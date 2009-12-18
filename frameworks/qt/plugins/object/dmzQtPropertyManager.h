#ifndef DMZ_QT_PROPERTY_MANAGER_H
#define DMZ_QT_PROPERTY_MANAGER_H

#include <dmzTypesMask.h>
#include <dmzTypesVector.h>
#include <dmzRuntimeDefinitions.h>
#include <QtCore/QMap>
#include "qtpropertybrowser.h"

class QtBoolPropertyManager;
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

   class MaskPropertyManagerPrivate;

   class MaskPropertyManager : public QtAbstractPropertyManager
   {
       Q_OBJECT
   public:
       MaskPropertyManager(Definitions &defs, QObject *parent = 0);
       ~MaskPropertyManager();

       QtBoolPropertyManager *subBoolPropertyManager() const;

       void setValueNames (const QStringList &NameList);
       
       Mask value(const QtProperty *property) const;
       
   public Q_SLOTS:
       void setValue(QtProperty *property, const Mask &val);

   Q_SIGNALS:
       void valueChanged(QtProperty *property, const Mask &val);

   protected:
       QString valueText(const QtProperty *property) const;
       virtual void initializeProperty(QtProperty *property);
       virtual void uninitializeProperty(QtProperty *property);

   private:
       MaskPropertyManagerPrivate *d_ptr;
       Q_DECLARE_PRIVATE(MaskPropertyManager)
       Q_DISABLE_COPY(MaskPropertyManager)
       Q_PRIVATE_SLOT(d_func(), void slotBoolChanged(QtProperty *, bool))
       Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty *))
   };

   // MaskPropertyManagerPrivate

   class MaskPropertyManagerPrivate
   {
       MaskPropertyManager *q_ptr;
       Q_DECLARE_PUBLIC(MaskPropertyManager)
   public:
       MaskPropertyManagerPrivate(Definitions &defs);

       void slotBoolChanged(QtProperty *property, bool value);
       void slotPropertyDestroyed(QtProperty *property);
       
       typedef QMap<const QtProperty *, Mask> PropertyValueMap;
       PropertyValueMap m_values;

       bool m_settingValue;
       Definitions &m_defs;
       QStringList m_nameList;
       QtBoolPropertyManager *m_boolPropertyManager;
       QMap<const QtProperty *, QList<QtProperty *> > m_propertyToMasks;
       QMap<const QtProperty *, QtProperty *> m_maskToProperty;
   };
};


#endif // DMZ_QT_PROPERTY_MANAGER_H
