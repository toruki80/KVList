#ifndef KVLISTSERIALIZER_H
#define KVLISTSERIALIZER_H

#include <QObject>
#include <QMap>
#include <QDebug>
#include <functional>
#include "kvlist_global.h"


class KVListModel;
class KVListEntry;
class KVListBase;

/**
 * @brief The KVListSerializer class
 *
 * Provides a generic serializer / deserializer for KVListModels.
 * Currently there is an implementation for xml.. e.g. JSON should be implemented by inheriting from this class
 * No need to use this class directly: Just usw KVListModel serialize() / deserialize() functions
 */


class KVLIST_EXPORT KVListSerializer : public QObject
{
    Q_OBJECT

public:
    explicit KVListSerializer(QObject *parent = nullptr) : QObject(parent){}
    virtual ~KVListSerializer() = default;

    // serialize model to file
    virtual bool serialize(KVListModel *model, const QString &filename) = 0;

    // de-serialize from file: create a new model (or nullptr in case of errors)
    virtual KVListModel* deserializeToNewModel(const QString &filename) = 0;

    // de-serialize from file: exsting model will be filled
    virtual bool deserializeToExistingModel(KVListModel* model, const QString &filename) = 0;

    // register you class here, so that deserializer can create an object of your class when necessary
    static void registerFactoryItem(const QString &name, std::function<KVListBase* (void)> createCallback);

    // alloc handling e.g. handline an old xml with a newer KVListSerializer
    int versionMinor() const {return versionMinor_;}
    int versionMajor() const {return versionMajor_;}

protected:
    KVListBase *createItem(const QString &name) const;
    int versionMajor_ = 1, versionMinor_  = 0;

private:
    static QMap<QString, std::function<KVListBase* (void)>> serializationFactory_;
};


// this is bullshit..  __COUNTER__ macro is not being incremented on clang...
#define GLUE2(x,y,z) x##y##z
#define GLUE(x,y,z) GLUE2(x,y,z)
#define REGISTER_2_SERIALIZATION_FACTORY(_classname_) \
    KVListSerializerRegistrar<_classname_> GLUE(tmp_, __COUNTER__, __LINE__)(_classname_().metaObject()->className())

template<class T>
class KVListSerializerRegistrar {
public:
    KVListSerializerRegistrar(const QString &name) {
        KVListSerializer::registerFactoryItem(name, []() -> KVListBase* {
            return new T();
        });
    }
};


#endif // KVLISTSERIALIZER_H
