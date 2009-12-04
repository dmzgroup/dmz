#include "dmzQtPropertyManager.h"
#include <qtpropertymanager.h>


namespace {

template <class Value, class PrivateData>
static Value getData(const QMap<const QtProperty *, PrivateData> &propertyMap,
            Value PrivateData::*data,
            const QtProperty *property, const Value &defaultValue = Value()) {

    typedef QMap<const QtProperty *, PrivateData> PropertyToData;
    typedef Q_TYPENAME PropertyToData::const_iterator PropertyToDataConstIterator;
    const PropertyToDataConstIterator it = propertyMap.constFind(property);
    if (it == propertyMap.constEnd())
        return defaultValue;
    return it.value().*data;
}

template <class Value, class PrivateData>
static Value getValue(const QMap<const QtProperty *, PrivateData> &propertyMap,
            const QtProperty *property, const Value &defaultValue = Value()) {

    return getData<Value>(propertyMap, &PrivateData::val, property, defaultValue);
}

};


void
dmz::VectorPropertyManagerPrivate::slotDoubleChanged(QtProperty *property, double value) {

    if (QtProperty *prop = m_xToProperty.value(property, 0)) {
        Vector p = m_values[prop].val;
        p.set_x(value);
        q_ptr->setValue(prop, p);
    } else if (QtProperty *prop = m_yToProperty.value(property, 0)) {
        Vector p = m_values[prop].val;
        p.set_y(value);
        q_ptr->setValue(prop, p);
    } else if (QtProperty *prop = m_zToProperty.value(property, 0)) {
        Vector p = m_values[prop].val;
        p.set_z(value);
        q_ptr->setValue(prop, p);
    }
}


void
dmz:: VectorPropertyManagerPrivate::slotPropertyDestroyed(QtProperty *property) {

    if (QtProperty *pointProp  = m_xToProperty.value(property, 0)) {
        m_propertyToX[pointProp] = 0;
        m_xToProperty.remove(property);
    } else if (QtProperty *pointProp = m_yToProperty.value(property, 0)) {
        m_propertyToY[pointProp] = 0;
        m_yToProperty.remove(property);
    } else if (QtProperty *pointProp = m_zToProperty.value(property, 0)) {
        m_propertyToZ[pointProp] = 0;
        m_zToProperty.remove(property);
    }
}


dmz::VectorPropertyManager::VectorPropertyManager(QObject *parent)
    : QtAbstractPropertyManager(parent) {

    d_ptr = new VectorPropertyManagerPrivate;
    d_ptr->q_ptr = this;

    d_ptr->m_doublePropertyManager = new QtDoublePropertyManager(this);
    connect(d_ptr->m_doublePropertyManager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(slotDoubleChanged(QtProperty *, double)));
    connect(d_ptr->m_doublePropertyManager, SIGNAL(propertyDestroyed(QtProperty *)),
                this, SLOT(slotPropertyDestroyed(QtProperty *)));
}


dmz::VectorPropertyManager::~VectorPropertyManager () {

    clear();
    delete d_ptr;
}


QtDoublePropertyManager *
dmz::VectorPropertyManager::subDoublePropertyManager() const {

    return d_ptr->m_doublePropertyManager;
}


dmz::Vector
dmz::VectorPropertyManager::value(const QtProperty *property) const {

    return getValue<Vector>(d_ptr->m_values, property);
}


int
dmz::VectorPropertyManager::decimals(const QtProperty *property) const {

    return getData<int>(d_ptr->m_values, &VectorPropertyManagerPrivate::Data::decimals, property, 0);
}


QString
dmz::VectorPropertyManager::valueText(const QtProperty *property) const {

    const VectorPropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
        return QString();
    const Vector v = it.value().val;
    const int dec =  it.value().decimals;
    return QString(tr("[%1, %2, %3]").arg(QString::number(v.get_x(), 'f', dec))
                                 .arg(QString::number(v.get_y(), 'f', dec))
                                 .arg(QString::number(v.get_z(), 'f', dec)));
}


void
dmz::VectorPropertyManager::setValue(QtProperty *property, const Vector &val) {

    const VectorPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    if (it.value().val == val)
        return;

    it.value().val = val;
    d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToX[property], val.get_x());
    d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToY[property], val.get_y());
    d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToZ[property], val.get_z());

    emit propertyChanged(property);
    emit valueChanged(property, val);
}


void
dmz::VectorPropertyManager::setDecimals(QtProperty *property, int prec) {

    const VectorPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    VectorPropertyManagerPrivate::Data data = it.value();

    if (prec > 13)
        prec = 13;
    else if (prec < 0)
        prec = 0;

    if (data.decimals == prec)
        return;

    data.decimals = prec;
    d_ptr->m_doublePropertyManager->setDecimals(d_ptr->m_propertyToX[property], prec);
    d_ptr->m_doublePropertyManager->setDecimals(d_ptr->m_propertyToY[property], prec);
    d_ptr->m_doublePropertyManager->setDecimals(d_ptr->m_propertyToZ[property], prec);

    it.value() = data;

    emit decimalsChanged(property, data.decimals);
}


void
dmz::VectorPropertyManager::initializeProperty(QtProperty *property) {

    d_ptr->m_values[property] = VectorPropertyManagerPrivate::Data();

    QtProperty *xProp = d_ptr->m_doublePropertyManager->addProperty();
    xProp->setPropertyName(tr("X"));
    d_ptr->m_doublePropertyManager->setDecimals(xProp, decimals(property));
    d_ptr->m_doublePropertyManager->setValue(xProp, 0);
    d_ptr->m_propertyToX[property] = xProp;
    d_ptr->m_xToProperty[xProp] = property;
    property->addSubProperty(xProp);

    QtProperty *yProp = d_ptr->m_doublePropertyManager->addProperty();
    yProp->setPropertyName(tr("Y"));
    d_ptr->m_doublePropertyManager->setDecimals(yProp, decimals(property));
    d_ptr->m_doublePropertyManager->setValue(yProp, 0);
    d_ptr->m_propertyToY[property] = yProp;
    d_ptr->m_yToProperty[yProp] = property;
    property->addSubProperty(yProp);

    QtProperty *zProp = d_ptr->m_doublePropertyManager->addProperty();
    zProp->setPropertyName(tr("Z"));
    d_ptr->m_doublePropertyManager->setDecimals(zProp, decimals(property));
    d_ptr->m_doublePropertyManager->setValue(zProp, 0);
    d_ptr->m_propertyToZ[property] = zProp;
    d_ptr->m_zToProperty[zProp] = property;
    property->addSubProperty(zProp);
}


void
dmz::VectorPropertyManager::uninitializeProperty(QtProperty *property) {

    QtProperty *xProp = d_ptr->m_propertyToX[property];
    if (xProp) {
        d_ptr->m_xToProperty.remove(xProp);
        delete xProp;
    }
    d_ptr->m_propertyToX.remove(property);

    QtProperty *yProp = d_ptr->m_propertyToY[property];
    if (yProp) {
        d_ptr->m_yToProperty.remove(yProp);
        delete yProp;
    }
    d_ptr->m_propertyToY.remove(property);

    QtProperty *zProp = d_ptr->m_propertyToZ[property];
    if (zProp) {
        d_ptr->m_zToProperty.remove(zProp);
        delete zProp;
    }
    d_ptr->m_propertyToZ.remove(property);

    d_ptr->m_values.remove(property);
}

MaskPropertyManagerPrivate::MaskPropertyManagerPrivate(RuntimeContext *context) :
      m_defs (context) {;}

void MaskPropertyManagerPrivate::slotBoolChanged(QtProperty *property, bool value)
{
    QtProperty *prop = m_maskToProperty.value(property, 0);
    if (prop == 0)
        return;

    QListIterator<QtProperty *> itProp(m_propertyToMasks[prop]);
    int level = 0;
    while (itProp.hasNext()) {

        QtProperty *p = itProp.next();
        if (p == property) {

            Mask v = m_values[prop].val;

            if (value) {
                v |= (1 << level);
            } else {
                v &= ~(1 << level);
            }

            q_ptr->setValue(prop, v);

            return;
        }

        level++;
    }
}

void MaskPropertyManagerPrivate::slotPropertyDestroyed(QtProperty *property)
{
    QtProperty *maskProperty = m_maskToProperty.value(property, 0);
    if (flagProperty == 0)
        return;

    m_propertyToMasks[maskProperty].replace(m_propertyToMasks[maskProperty].indexOf(property), 0);
    m_maskToProperty.remove(property);
}


MaskPropertyManager::MaskPropertyManager(RuntimeContext *context, QObject *parent)
    : QtAbstractPropertyManager(parent)
{
    d_ptr = new MaskPropertyManagerPrivate (context);
    d_ptr->q_ptr = this;

    d_ptr->m_boolPropertyManager = new QtBoolPropertyManager(this);
    connect(d_ptr->m_boolPropertyManager, SIGNAL(valueChanged(QtProperty *, bool)),
                this, SLOT(slotBoolChanged(QtProperty *, bool)));
    connect(d_ptr->m_boolPropertyManager, SIGNAL(propertyDestroyed(QtProperty *)),
                this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

MaskPropertyManager::~MaskPropertyManager()
{
    clear();
    delete d_ptr;
}

QtBoolPropertyManager *MaskPropertyManager::subBoolPropertyManager() const
{
    return d_ptr->m_boolPropertyManager;
}

Mask MaskPropertyManager::value(const QtProperty *property) const
{
    return getValue<Mask>(d_ptr->m_values, property, 0);
}

QStringList MaskPropertyManager::flagNames(const QtProperty *property) const
{
    return getData<QStringList>(d_ptr->m_values, &MaskPropertyManagerPrivate::Data::maskNames, property, QStringList());
}

QString MaskPropertyManager::valueText(const QtProperty *property) const
{
    const MaskPropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
        return QString();

    const MaskPropertyManagerPrivate::Data &data = it.value();

    String name;
    d_ptr->defs.lookup_state_name (data.val, name);

    QString str (name.get_buffer ());

    return str;
}

void MaskPropertyManager::setValue(QtProperty *property, const Mask val)
{
    const MaskPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

   MaskPropertyManagerPrivate::Data data = it.value();

    if (data.val == val)
        return;

    if (!data.is_valid ())
       return;

    data.val = val;

    it.value() = data;

    QListIterator<QtProperty *> itProp(d_ptr->m_propertyToMasks[property]);
    int level = 0;
    while (itProp.hasNext()) {
        QtProperty *prop = itProp.next();
        if (prop)
            d_ptr->m_boolPropertyManager->setValue(prop,  val & (1 << level));
        level++;
    }

    emit propertyChanged(property);
    emit valueChanged(property, data.val);
}


void MaskPropertyManager::setFlagNames(QtProperty *property, const QStringList &flagNames)
{
    const QtFlagPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    QtFlagPropertyManagerPrivate::Data data = it.value();

    if (data.flagNames == flagNames)
        return;

    data.flagNames = flagNames;
    data.val = 0;

    it.value() = data;

    QListIterator<QtProperty *> itProp(d_ptr->m_propertyToFlags[property]);
    while (itProp.hasNext()) {
        QtProperty *prop = itProp.next();
        if (prop) {
            delete prop;
            d_ptr->m_flagToProperty.remove(prop);
        }
    }
    d_ptr->m_propertyToFlags[property].clear();

    QStringListIterator itFlag(flagNames);
    while (itFlag.hasNext()) {
        const QString flagName = itFlag.next();
        QtProperty *prop = d_ptr->m_boolPropertyManager->addProperty();
        prop->setPropertyName(flagName);
        property->addSubProperty(prop);
        d_ptr->m_propertyToFlags[property].append(prop);
        d_ptr->m_flagToProperty[prop] = property;
    }

    emit flagNamesChanged(property, data.flagNames);

    emit propertyChanged(property);
    emit valueChanged(property, data.val);
}

/*!
    \reimp
*/
void MaskPropertyManager::initializeProperty(QtProperty *property)
{
    d_ptr->m_values[property] = QtFlagPropertyManagerPrivate::Data();

    d_ptr->m_propertyToFlags[property] = QList<QtProperty *>();
}

/*!
    \reimp
*/
void MaskPropertyManager::uninitializeProperty(QtProperty *property)
{
    QListIterator<QtProperty *> itProp(d_ptr->m_propertyToFlags[property]);
    while (itProp.hasNext()) {
        QtProperty *prop = itProp.next();
        if (prop) {
            delete prop;
            d_ptr->m_flagToProperty.remove(prop);
        }
    }
    d_ptr->m_propertyToFlags.remove(property);

    d_ptr->m_values.remove(property);
}
