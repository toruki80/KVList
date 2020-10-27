#include "kvlistserializer.h"
#include "kvlistmodel.h"
#include <QDebug>
#include <QMetaObject>
#include <QMetaType>

QMap<QString, std::function<KVListBase* (void)>> KVListSerializer::serializationFactory_;

void KVListSerializer::registerFactoryItem(const QString &name, std::function<KVListBase *()> createCallback)
{
    Q_ASSERT(!serializationFactory_.contains(name)); // cannot allow registering twice!
    serializationFactory_[name] = createCallback;
}

KVListBase *KVListSerializer::createItem(const QString &name) const
{
    if(!serializationFactory_.contains(name)) {
        qCritical() << "Serialization:" << name << "not registered to Serializer's factory!";
        return nullptr;
    }

    return serializationFactory_[name]();
}



