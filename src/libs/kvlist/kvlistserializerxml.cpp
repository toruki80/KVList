#include "kvlistserializerxml.h"
#include <QDomDocument>
#include <QDebug>
#include <QMetaObject>
#include <QFile>
#include <QVersionNumber>
#include <QDateTime>

#include "kvlistmodel.h"
#include "kvlistentry.h"
#include "kvlist_global.h"

static const char* NAME_KEY = "Key";
static const char* NAME_TYPE = "Type";
static const char* NAME_ENTRY = "Entry";
static const char* NAME_MODEL = "Model";
static const char* NAME_VALUE = "Value";
static const char* NAME_CONTENT = "KVListSerializerXml";
static const char* NAME_VERSION = "Version";
static const char* NAME_DATE = "Date";
static const char* PREFIX_SERIALIZE_IGNORE = "_noserialize";
static const char* PREFIX_SERIALIZE_IGNORE2 = "_ns";


KVListSerializerXml::KVListSerializerXml(QObject *parent) : KVListSerializer(parent)
{
}

bool KVListSerializerXml::serialize(KVListModel *model, const QString &filename)
{
    QDomDocument document;
    QDomElement el = document.createElement(NAME_CONTENT);
    document.appendChild(el);

    // in case xml layout changes later, we want to have a version stored...
    el.setAttribute(NAME_VERSION, QVersionNumber(versionMajor(), versionMinor()).toString());
    el.setAttribute(NAME_DATE, QDateTime::currentDateTime().toString());
    el.appendChild(serialize(document, model));

    // Writing to a file
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning(kvlist) << "Open the file for writing failed";
        return false;
    } else {
        QTextStream stream(&file);
        stream << document.toString();
        file.close();
        return true;
    }
}



QDomElement KVListSerializerXml::serialize(QDomDocument &document, KVListModel *model)
{
    Q_ASSERT(model);

    QDomElement el = document.createElement(NAME_MODEL);
    el.setAttribute(NAME_TYPE, model->metaObject()->className());
    el.setAttribute(NAME_VERSION, QVersionNumber(model->versionMajor(), model->versionMinor()).toString());

    for(KVListEntry *entry : *model)
        el.appendChild(serialize(document, entry));

    return el;
}

QDomElement KVListSerializerXml::serialize(QDomDocument &document, KVListEntry *entry)
{
    QDomElement el = document.createElement(NAME_ENTRY);
    el.setAttribute(NAME_TYPE, entry->metaObject()->className());

    QHash<int, QByteArray> roleNames = entry->getParentModel()->roleNames();

    for(KVListEntry::Key key : entry->keys()) {
        QString keyStr = roleNames.value(key);
        if(!keyStr.endsWith(PREFIX_SERIALIZE_IGNORE) || keyStr.endsWith(PREFIX_SERIALIZE_IGNORE2)) {
        QDomElement el2 = serialize(document, keyStr, entry->getValue(key));
        if(!el2.isNull())
            el.appendChild(el2);
        }
    }

    return el;
}

QDomElement KVListSerializerXml::serialize(QDomDocument &document, const QString key, const QVariant &value)
{
    QDomElement el = document.createElement(NAME_VALUE);
    el.setAttribute(NAME_KEY, key);

    if(value.canConvert<KVListModel*>())
    {
        el.setAttribute(NAME_TYPE, NAME_MODEL);
        el.appendChild(serialize(document, value.value<KVListModel*>()));
    }
    else if(value.canConvert<QObject*>() || value.canConvert<void*>())
    {
        // never serialize pointers... this will only cause errer!
        return QDomElement();
    }
    else
    {
        el.setAttribute(NAME_TYPE, value.typeName());
        el.setAttribute(NAME_VALUE, value.toString());
    }
    return el;
}



KVListModel *KVListSerializerXml::deserializeToNewModel(const QString &filename)
{
    QDomDocument document;

    // Open a file for reading
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open the file for reading.";
        return nullptr;
    }
    else
    {
        // loading
        if(!document.setContent(&file))
        {
            qWarning() << "Failed to load the file for reading.";
            return nullptr;
        }
        file.close();
    }

    QDomElement root = document.firstChildElement();

    if(root.tagName() != NAME_CONTENT) {
        qWarning() << "xml supposed to start with another tag";
        return nullptr;
    }

    QVersionNumber version = QVersionNumber::fromString(root.attribute(NAME_VERSION));
    if((version.majorVersion() != versionMajor()) ||
            (version.minorVersion() != versionMinor())) {
        qWarning() << "xml must have version" << QVersionNumber(versionMajor(), versionMinor()).toString();
        return nullptr;
    }

    return deserializeChildModel(root.firstChildElement());
}

bool KVListSerializerXml::deserializeToExistingModel(KVListModel *model, const QString &filename)
{
    QDomDocument document;

    // Open a file for reading
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open the file for reading.";
        return false;
    }
    else
    {
        // loading
        if(!document.setContent(&file))
        {
            qWarning() << "Failed to load the file for reading.";
            return false;
        }
        file.close();
    }

    QDomElement root = document.firstChildElement();

    if(root.tagName() != NAME_CONTENT) {
        qWarning() << "xml supposed to start with another tag";
        return false;
    }

    QVersionNumber version = QVersionNumber::fromString(root.attribute(NAME_VERSION));
    if((version.majorVersion() != versionMajor()) ||
            (version.minorVersion() != versionMinor())) {
        qWarning() << "xml must have version" << QVersionNumber(versionMajor(), versionMinor()).toString();
        return false;
    }

    return deserializeModel(root.firstChildElement(), model);
}


KVListModel *KVListSerializerXml::deserializeChildModel(const QDomElement &el, KVListModel *destination)
{
    if(el.tagName() != NAME_MODEL)
        return nullptr;

    KVListModel *m = nullptr;
    if(destination) {
        if(destination->metaObject()->className() != el.attribute(NAME_TYPE))
            return nullptr;
        else
            m = destination;
    }
    else {

        KVListBase *b = createItem(el.attribute(NAME_TYPE));
        if(!b)
            return nullptr;
        m = dynamic_cast<KVListModel*>(b);
        if(!m) {
            delete b;
            return nullptr;
        }
    }
    if(!deserializeModel(el, m)) {
        delete m;
        return nullptr;
    }

    return m;
}

bool KVListSerializerXml::deserializeModel(const QDomElement &el, KVListModel *model)
{
    if(el.tagName() != NAME_MODEL)
        return false;

    if(el.attribute(NAME_TYPE) != model->metaObject()->className())
        return false;

    QVersionNumber version = QVersionNumber::fromString(el.attribute(NAME_VERSION));
    if((version.majorVersion() != model->versionMajor()) ||
            (version.minorVersion() != model->versionMinor())) {
        qWarning() << "xml must have version" << QVersionNumber(model->versionMajor(), model->versionMinor()).toString();
        return false;
    }

    QDomNodeList nodes = el.childNodes();
    for(int i = 0; i < nodes.count(); i++)
    {
        QDomNode elm = nodes.at(i);
        if(elm.isElement())
        {
            QDomElement e = elm.toElement();
            KVListEntry * entry = deserializeEntry(e, model, version);

            if(entry)
                model->append(entry);
        }
    }

    return true;
}

KVListEntry *KVListSerializerXml::deserializeEntry(const QDomElement &el, KVListModel *model, const QVersionNumber &modelVersion)
{
    if(el.tagName() != NAME_ENTRY)
        return nullptr;


    KVListBase *b = createItem(el.attribute(NAME_TYPE));
    if(!b)
        return nullptr;


    KVListEntry *e = dynamic_cast<KVListEntry*>(b);
    if(!e) {
        delete b;
        return nullptr;
    }

    QDomNodeList nodes = el.childNodes();
    QMap<KVListEntry::Key, QVariant> values;

    for(int i = 0; i < nodes.count(); i++)
    {
        QDomNode elm = nodes.at(i);
        if(elm.isElement())
        {
            QDomElement el = elm.toElement();
            QPair<KVListEntry::Key, QVariant> value = deserializeValue(el, model, e, modelVersion);
            if(value.first >= 0)
                values.insert(value.first, value.second);
        }
    }
    e->setValues(values);

    return e;
}

QPair<KVListEntry::Key, QVariant> KVListSerializerXml::deserializeValue(const QDomElement &el, KVListModel *model, KVListEntry *entry, const QVersionNumber &modelVersion)
{
    if(el.tagName() != NAME_VALUE)
        return qMakePair(-1, QVariant());

    QString typeStr = el.attribute(NAME_TYPE);
    QString keyStr = el.attribute(NAME_KEY);

    if(keyStr.endsWith(PREFIX_SERIALIZE_IGNORE) || keyStr.endsWith(PREFIX_SERIALIZE_IGNORE2)) {
        return qMakePair(-1, QVariant());
    }

    KVListEntry::Key key = model->lookupKey(keyStr, modelVersion);
    if(key < 0) {
        qWarning() << "unknown key in xml found:" << el.attribute(NAME_KEY);
        return qMakePair(-1, QVariant());
    }

    if(typeStr == NAME_MODEL) // another model!
    {
        // in case the childmodel has already been added to the entry, we just use the existing one!
        KVListModel* childmodel = deserializeChildModel(el.firstChildElement(), entry->getChildModel(key));
        if(!childmodel)
            return qMakePair(-1, QVariant());

        QVariant v = QVariant::fromValue(childmodel);
        if(!v.isValid())
            return qMakePair(-1, QVariant());

        return qMakePair(key, v);
    }
    else
    {
        QVariant::Type typeId = QVariant::nameToType(typeStr.toLocal8Bit().data());
        if(typeId == QVariant::Invalid)
            return qMakePair(-1, QVariant());

        QVariant v(el.attribute(NAME_VALUE));
        if(!v.convert(typeId))
            return qMakePair(-1, QVariant());
        return qMakePair(key, v);
    }
}
