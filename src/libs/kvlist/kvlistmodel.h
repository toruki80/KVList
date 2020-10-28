#ifndef KVLISTMODEL_H
#define KVLISTMODEL_H

#include <QVector>
#include <QSet>
#include <QAbstractListModel>
#include <QMetaEnum>
#include <QPair>
#include <QVersionNumber>

#include "kvlist_global.h"
#include "kvlistentry.h"
#include "kvlistserializer.h"

/**
 * @brief The KVListModel class
 *
 * KVListModel provides a generic list containing entries which can be used as-is in a
 * typical Qt Model/View usecase.
 *
 * Here is a short example of a Addressbook which uses as entry the class Person
 * described in the KVListEntry documentation
 *
 * <code>
 * // file: addressbook.h
 * #include "kvlistmodel.h"
 *
 * class Addressbook : public KVListModel
 * {
 *     Q_OBJECT
 *
 * public:
 *     Addressbook() : KVListModel(QMetaEnum::fromType<Person::EnKey>())
 *     {
 *         *this << new Person("Adrian", 30, "Munich");
 *         *this << new Person("Barbara", 40, "Boston");
 *         *this << new Person("Caesar", 50, "Rome");
 *     }
 * };
 *
 * </code>
 *
 * Register 'Addressbook' to qml:
 * <code>
 * // file: addressbook.cpp
 *
 * qmlRegisterSingletonType<Addressbook>("KVListExample", 1, 0, "Addressbook", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
 *      Q_UNUSED(engine);
 *      Q_UNUSED(scriptEngine);
 *      return new Addressbook();
 * });
 * </code>
 *
 * or:
 *
 * <code>
 * // file: addressbook.cpp
 * Addressbook ab;
 * // prepare your addressbook here
 * qmlRegisterUncreatableType<Addressbook>("MyModule",1,0,"addressbook", "");
 * engine.rootContext()->setContextProperty("addressbook", ab);
 * </code>
 *
 * Now you can access the addressbook from qml:
 *
 * <code>
 * // in qml
 * GridView {
 *     model: addressbook
 *     delegate: Text {
 *         id: addressbookEntry
 *         text: model.Name + ", Age: " + model.age
 *     }
 * }
 * </code>
 *
 *
 * IMPORTANT: In case you want to (de)serialize your type, make sure to register the type like this:
 * <code>
 * // file: addressbook.cpp
 * #include "addressbook.h"
 * #include "kvlistserializer.h"
 *
 * REGISTER_2_SERIALIZATION_FACTORY(Addressbook);
 * </code>
 *
 * KVListModel requires a QMetaEnum in the constructor in order to create the rolenames for QML.
 * Therefore you can simply use the enum value name to access the key value from the entry
 * Note: make sure the enum values start with lowercase letters
 * Note: the enum values must be within [0 .. (ShadowedKeysStartAt-1)]
 *
 * Shadowed values can be accessed like this:
 *
 * <code>
 *
 * MouseArea {
 *     id: button1
 *     onClicked: model.age_shadowed = 33
 * }
 *
 * MouseArea {
 *     id: acceptButton
 *     onClicked: model.ENTRY.applyShadowedChanges()
 * }
 *
 * </code>
 *
 * There are some extra rolenames that allow you to access the C++ class implementation from QML
 * - 'model.ENTRY' will return the delegates Entry implementation (for the example above: 'Person')
 * - 'model.MODEL' will return the model implementation (for the example above: 'Addressbook')
 *
 */
class KVLIST_EXPORT KVListModel : public QAbstractListModel, public KVListBase
{
    Q_OBJECT

public:
    typedef std::function<void (const KVListEntry *entry)> EntryChangedCallbackFunc;
    struct CbHandle{
        CbHandle(EntryChangedCallbackFunc f) : func(f){}
        EntryChangedCallbackFunc func;
    };


    enum EnInternalKeys {
        MODEL = KVListEntry::InternalKeysStartAt,
        ENTRY
    };
    Q_ENUM(EnInternalKeys);

    explicit KVListModel(const QMetaEnum &keysEnum, QObject *parent = nullptr);
    virtual ~KVListModel();

    // get entry at index
    Q_INVOKABLE virtual KVListEntry* at(int i);

    // add values
    Q_INVOKABLE virtual void insert(int i, KVListEntry *entry);
    Q_INVOKABLE virtual void append(KVListEntry *entry);
    Q_INVOKABLE virtual void prepend(KVListEntry *entry);

    // remove value (items are not deleted)
    Q_INVOKABLE virtual KVListEntry *takeAt(int i);
    Q_INVOKABLE virtual KVListEntry* takeFirst();
    Q_INVOKABLE virtual KVListEntry* takeLast();
    // clear all items... (items are not deleted)
    Q_INVOKABLE virtual void clear();

    // delete value
    Q_INVOKABLE virtual void deleteAt(int i);
    Q_INVOKABLE virtual void deleteFirst();
    Q_INVOKABLE virtual void deleteLast();
    // delete all items
    Q_INVOKABLE virtual void deleteAll();

    // move value from one position to another
    Q_INVOKABLE virtual void move(int from, int to);

    // get the index of element... or -1 in case it does not exist
    Q_INVOKABLE int indexOf(KVListEntry *entry) const { return entries_.indexOf(entry); }
    Q_INVOKABLE int size() const { return entries_.size(); }

    // appends value
    KVListModel &operator<<(KVListEntry *entry);

    // add your callback/lambda here... will be notified when any entries role changes
    // returns a handle that can be used to remove the entry again
    CbHandle* onEntriesChanged(const QVector<int> &roles, EntryChangedCallbackFunc func);
    CbHandle* onEntriesChanged(int role, EntryChangedCallbackFunc func);
    // remove a callback
    void removeEntriesRoleChanged(CbHandle *obj);

    // provide begin() end() to allow iterating via range-based-loops
    QVector<KVListEntry*>::iterator begin();
    QVector<KVListEntry*>::iterator end();
    QVector<KVListEntry*>::const_iterator begin() const;
    QVector<KVListEntry*>::const_iterator end() const;

    // this allows a simple printing of available role names from c++ and qml
    Q_INVOKABLE QStringList keyNamesList() const;

    // set a filename here... each 'serialize()' / 'deSerialize()' will use this file later as destination / source
    QString getSerializationFile() const { return serializationFile_; }
    void setSerializationFile(const QString &file) {serializationFile_ = file; }

    // serialize-to / de-serialize-from stored file location (see 'setSerializationFileLocation()'); returns false on error
    Q_INVOKABLE virtual bool serialize() { return serialize(getSerializationFile()); }
    Q_INVOKABLE virtual bool deSerialize() { return deSerialize(getSerializationFile()); }

    // serialize-to / de-serialize-from file given by argument; returns false on error
    // depending on the filetype, the serialization type is choosen... (for now only *.xml is supported!)
    Q_INVOKABLE virtual bool serialize(const QString &to);
    Q_INVOKABLE virtual bool deSerialize(const QString &from);


    // here you can implement special behaviour in case you need to e.g. deserialize data from version 1.0,
    // but your implementation has been bumped to 2.0:
    // most likely a key has changed, or has been removed... -> map/filter the 1.0 key to the 2.0 value here
    // by re-implementing this function in your inherited class
    // in case the key does not exist, please return -1
    virtual KVListEntry::Key lookupKey(const QString &keyString, const QVersionNumber &version);

    // version major & minor
    Q_INVOKABLE int versionMinor() const {return versionMinor_;}
    Q_INVOKABLE int versionMajor() const {return versionMajor_;}

    // apply the changes stored in the shadowed entries; return number of changes
    Q_INVOKABLE int applyAllShadowedChanges();

    // revert the changes stored in the shadowed entries
    Q_INVOKABLE void revertAllShadowedChanges();

    // qml can access the roles by the names created here... by default we simply use the enum names
    // please override this function in case you want to change anything
    virtual QHash<int, QByteArray> setupModelRoleNames(const QMetaEnum &keysEnum) const;

    // provide a version to serializer... this allows to react to changes in case a model/entry have changed
    // increase version in your class and implement 'lookupKey()' function
    int versionMajor_ = 1, versionMinor_  = 0;

    // QAbstractListModel impl
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    // static helper
    static QHash<int, QByteArray> createHashFromEnum(const QMetaEnum &keysEnum);

protected:
    friend class KVListEntry;
    // entry informs that keyed values have been changed
    virtual void entryHasChanged(const KVListEntry *entry, const QVector<int> &modifiedRoles);
    void insertInt(int i, KVListEntry *entry);
    void moveInt(int from, int to);
    KVListEntry* takeAtInt(int i);
    void disconnectEntry(KVListEntry *entry);
    void connectEntry(KVListEntry *entry);

    QVector<KVListEntry*> entries_;
    QHash<int, QByteArray> roleNames_;
    QMap<int, QVector<CbHandle*>> entryChangedCallbacks_;
    QString serializationFile_;
};

#endif // KVLISTMODEL_H
