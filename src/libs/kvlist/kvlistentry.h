#ifndef KVLISTENTRY_H
#define KVLISTENTRY_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QMetaEnum>
#include <QPair>
#include <functional>
#include "kvlistbase.h"
#include "kvlist_global.h"

/**
 * @brief The KVListEntry class
 *
 * KVListEntry provides a KEY-VALUE based data store. In combination with KVListModel the
 * keyed values can be exposed to QML Model/View classes;
 *
 * Here is a simple example:
 * <code>
 * // file: person.h
 * #include "kvlistentry.h"
 *
 * class Person : public KVListEntry
 * {
 *     Q_OBJECT
 *
 * public:
 *     enum EnKey { name, age, address, nickname_noserialize };
 *     Q_ENUM(EnKey)
 *
 *     Person(const QString &name, int age, const QString &address)
 *     {
 *         setValue(name, name);
 *         setValue(age, age);
 *         setValue(address, address);
 *
 *         // wish happy birthay when age changes
 *         onValueChanged(age, [=](){
 *             qDebug() << "Happy birthday" << getValue(Name);
 *         });
 *
 *         // create a nickname from name and age
 *         onValuesChanged(QVector<Key>() << name << age, [=](){
 *             setValue(nickname_noserialize, QString("%1-%2")
 *                      .arg(getValue(name).toString())
 *                      .arg(getValue(age).toString()));
 *         });
 *     }
 * };
 * </code>
 *
 * IMPORTANT: In case you want to (de)serialize your type, make sure to register the type like this:
 * <code>
 * // file: person.cpp
 * #include "person.h"
 * #include "kvlistserializer.h"
 *
 * REGISTER_2_SERIALIZATION_FACTORY(Person);
 * </code>
 *
 * When modifying values of the entry via the API (set[Shadowed]Value[s]()) the model
 * (where this entry belongs) to will be notified about the changes. This allows an
 * easy way to implement your application logic without the need to care about how c++ and qml
 * interact with each other.
 *
 * Each value is automatically being 'shadowed'. The changed value is being stored in a seperate
 * entry. In case there is no entry for getShadowedValue() yet, it will return the normal entry.
 * In general this allows very easy implementation of e.g. an Edit-Value-Dialog, where the use can make
 * temperory changes and later "accept" or "cancel" the changes. (applyShadowedChanges() /
 * revertShadowedChanges())
 *
 * You can register notifications for value changes as shown in the example.
 * In order to register notifications for changes in shadowed values, use 'SHADOWED_KEY(key)'.
 *
 * It is also possible to store another KVListModel for a key. That proved to be useful for
 * displaying "childmodels" in QML easily. See KVListModel documentation for details on how
 * models can be used.
 *
 * In case you want to exclude keys from being serialized, prepend '_noserialize' to the enum name.
 * E.g. in the above example the nickname above should not be set by the user but is instead created
 * programmatically... Any deserialization might overwrite values...
 * Instead of '_noserialize' '_ns' is also accepted
 *
 * <code>
 *
 * // in qml
 * ListView {
 *     id: parentList
 *     anchors.fill: parent
 *     model: Addressbook
 *     delegate: Item {
 *         Column {
 *             Text { text: '<b>Name:</b> ' + model.name }  // 'model.name' -> access the value for Person::EnKey::name
 *             Text { text: '<b>Age:</b> ' + model.age }
 *             Text { text: '<b>Address:</b> ' + model.address }
 *         }
 *     }
 * }
 *
 * </code>
 *
 */

class KVListModel;

class KVLIST_EXPORT KVListEntry : public QObject, public KVListBase
{
    Q_OBJECT

public:
    typedef std::function<void ()> ValueChangedCallbackFunc;
    struct CbHandle{ ValueChangedCallbackFunc func; };
    typedef int Key;
    typedef QMap<Key, QVariant> KeyValueMap;

    static constexpr Key ShadowedKeysStartAt = 1024;
    static constexpr Key InternalKeysStartAt = 2048;


    explicit KVListEntry(QObject *parent = nullptr);
    virtual ~KVListEntry();

    // get/set QVariant for given key
    QVariant getValue(Key key) const;
    QVariant& getValue(Key key);
    void setValue(Key key, const QVariant &value);
    void setValues(const QMap<Key, QVariant> &values);

    // get/set shadowed QVariant for given key
    QVariant getShadowedValue(Key key) const;
    void setShadowedValue(Key key, const QVariant &value);
    void setShadowedValues(const KeyValueMap &values);

    // set another model for given key
    void setChildModel(Key key, KVListModel *model_);
    KVListModel *getChildModel(Key key) const;
    KVListModel *getParentModel() const { return model_; }

    // return all keys that are currently set
    QList<Key> keys() const { return keyValueStore_.keys(); }

    // add your callback/lambda here... will be notified when value for given key(s) have been modified
    // the returned value can be used as a handle to remove the callback again!
    CbHandle* onValueChanged(const QVector<Key> &keys, ValueChangedCallbackFunc func);
    CbHandle* onValueChanged(Key key, ValueChangedCallbackFunc func);
    // remove a callback
    void removeOnValueChanged(CbHandle *obj);

    static inline Key SHADOWED_KEY(Key index) { return index+ShadowedKeysStartAt; }

    // apply the changes stored in the shadowed entries; return number of changes
    Q_INVOKABLE virtual int applyShadowedChanges();

    // revert the changes stored in the shadowed entries
    Q_INVOKABLE virtual void revertShadowedChanges();

protected:
    void invalidateModel(const QVector<Key> &keys) const;
    void notifyValueChangedCallbacks(const QVector<Key> &keys) const;
    bool setValueInt(Key key, const QVariant &value);

    QMap<Key, QVariant> keyValueStore_, keyValueStoreShadowed_;
    QMap<Key, QVector<CbHandle*>> keyModifiedCallbacks_;
    friend class KVListModel;
    KVListModel *model_;
};


#endif // KVLISTENTRY_H
