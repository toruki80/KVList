#include "kvlistmodel.h"
#include "kvlistserializerxml.h"
#include <QSet>
#include <QDebug>

#define DEBUG_DATA_ACCESS 0

#if (DEBUG_DATA_ACCESS != 0) && (!defined QT_DEBUG)
    #error "debug macro set for non debug build"
#endif

KVListModel::KVListModel(const QMetaEnum &keysEnum, QObject *parent) : QAbstractListModel(parent){
    roleNames_ = setupModelRoleNames(keysEnum);
}

KVListModel::~KVListModel() {
    for(int role : entryChangedCallbacks_.keys())
        qDeleteAll(entryChangedCallbacks_[role]);

    deleteAll();
}

KVListEntry *KVListModel::at(int i) {
    return entries_.at(i);
}

void KVListModel::insert(int i, KVListEntry *entry)
{
    if(i<0) i=0;
    if(i>=entries_.size()) i=entries_.size();
    insertInt(i, entry);
}

void KVListModel::append(KVListEntry *entry)
{
    insertInt(entries_.size(), entry);
}

void KVListModel::prepend(KVListEntry *entry)
{
    insertInt(0, entry);
}

KVListEntry *KVListModel::takeAt(int i)
{
    KVListEntry *e=nullptr;
    if(i>=0 && i < entries_.size())
        e = takeAtInt(i);
    return e;
}

KVListEntry *KVListModel::takeFirst()
{
    return takeAt(0);
}

KVListEntry *KVListModel::takeLast()
{
    return takeAt(entries_.size()-1);
}

void KVListModel::move(int from, int to)
{
    if(from>=0 && from<entries_.size() && to>=0 && to<entries_.size())
        moveInt(from, to);
    else
        qWarning() << "invalid index recieved!";
}


void KVListModel::clear()
{
    if(entries_.isEmpty())
        return;

    beginResetModel();
    for(KVListEntry *e : entries_)
        disconnectEntry(e);
    entries_.clear();
    endResetModel();
}

void KVListModel::deleteAt(int i) {
    KVListEntry *e = takeAt(i);
    if(e) delete e;
}

void KVListModel::deleteFirst() {
    KVListEntry *e = takeFirst();
    if(e) delete e;
}

void KVListModel::deleteLast() {
    KVListEntry *e = takeLast();
    if(e) delete e;
}

void KVListModel::deleteAll()
{
    if(entries_.isEmpty())
        return;

    beginResetModel();
    for(KVListEntry *e : entries_)
        disconnectEntry(e);
    qDeleteAll(entries_);
    entries_.clear();
    endResetModel();
}

KVListModel &KVListModel::operator<<(KVListEntry *entry){
    append(entry);
    return *this;
}

int KVListModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : entries_.size();
}

QHash<int, QByteArray> KVListModel::roleNames() const {
    return roleNames_;
}

QVariant KVListModel::data(const QModelIndex &index, int role) const
{    
    QVariant result;
    if(role == MODEL) // readonly!
        result = QVariant::fromValue((KVListModel*)this);
    else
    {
        KVListEntry *e = entries_.value(index.row(), nullptr);
        if(role == ENTRY)
            result =  QVariant::fromValue(e);
        else
            result = e ? e->getValue(role) : QVariant();
    }
#if (DEBUG_DATA_ACCESS != 0)
    qDebug().nospace() << "KVListModel::data(" << index << ", " << roleNames_.value(role) << ") -> " << result;
#endif
    return result;
}

bool KVListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
#if (DEBUG_DATA_ACCESS != 0)
    qDebug().nospace() << "KVListModel::setData(" << index << ", " << value << ", " << roleNames_.value(role) << ")";
#endif

    if(role == MODEL || role == ENTRY) // readonly!
        return false;

    KVListEntry *e = entries_.value(index.row(), nullptr);
    if(e) {
        e->setValue(role, value);
        return true;
    }
    return false;
}

Qt::ItemFlags KVListModel::flags(const QModelIndex &index) const {
    return index.isValid() ? Qt::ItemIsEditable : Qt::NoItemFlags;
}

QHash<int, QByteArray> KVListModel::createHashFromEnum(const QMetaEnum &keysEnum){
    QHash<int, QByteArray> hash;
    for(int i=0; i<keysEnum.keyCount(); i++)
        hash.insert(keysEnum.value(i), keysEnum.key(i));
    return hash;
}

KVListModel::CbHandle *KVListModel::onEntriesChanged(const QVector<int> &roles, KVListModel::EntryChangedCallbackFunc func)
{
    CbHandle *obj = new CbHandle(func);
    for(int role : roles)
        entryChangedCallbacks_[role] << obj;
    return obj;
}

KVListModel::CbHandle *KVListModel::onEntriesChanged(int role, KVListModel::EntryChangedCallbackFunc func)
{
    CbHandle *obj = new CbHandle(func);
    entryChangedCallbacks_[role] << obj;
    return obj;
}

void KVListModel::removeEntriesRoleChanged(KVListModel::CbHandle *obj)
{
    for(int role : entryChangedCallbacks_.keys())
        entryChangedCallbacks_[role].removeAll(obj);

    delete obj;
}

QVector<KVListEntry*>::iterator KVListModel::begin()
{
    return entries_.begin();
}

QVector<KVListEntry*>::iterator KVListModel::end()
{
    return entries_.end();
}

QVector<KVListEntry*>::const_iterator KVListModel::begin() const
{
    return entries_.begin();
}

QVector<KVListEntry*>::const_iterator KVListModel::end() const
{
    return entries_.end();
}

QStringList KVListModel::keyNamesList() const
{
    QStringList l;
    QHashIterator<int, QByteArray> i(roleNames_);
    while (i.hasNext()) {
        i.next();
        l << i.value();
    }
    return l;
}

bool KVListModel::serialize(const QString &to) {

    if(to.isEmpty()) return false;

    if(to.endsWith(".xml", Qt::CaseInsensitive))
    {
        KVListSerializerXml s;
        return s.serialize(this, to);
    }
    else
    {
        qCritical() << "invalid file type!";
        return false;
    }
}

bool KVListModel::deSerialize(const QString &from)
{
    if(from.isEmpty()) return false;

    if(from.endsWith(".xml", Qt::CaseInsensitive))
    {
        KVListSerializerXml s;
        return s.deserializeToExistingModel(this, from);
    }
    else
    {
        qCritical() << "invalid file type!";
        return false;
    }
}

KVListEntry::Key KVListModel::lookupKey(const QString &keyName, const QVersionNumber &version) {

    // default implementation... overwrite this function this for your customized version handling
    if(version.isNull()) {}
    else if((version.majorVersion() == versionMajor_) && (version.minorVersion() == versionMinor_)) {}
    else return -1;

    return roleNames_.key(keyName.toLocal8Bit(), -1);
}

int KVListModel::applyAllShadowedChanges()
{
    int cnt=0;
    for(KVListEntry *e : *this)
        cnt += e->applyShadowedChanges();
    return cnt;
}

void KVListModel::revertAllShadowedChanges()
{
    for(KVListEntry *e : *this)
        e->revertShadowedChanges();
}

void KVListModel::entryHasChanged(const KVListEntry *entry, const QVector<int> &modifiedRoles)
{
    // obtaining the index can be improved, if the index is being stored within the entry
    // whenever the model changes, of course the index must be updated
    // for now we obtain the index each time something changes... This allows us to minimize
    // model <-> entry interaction
    //
    // note: the cast is a ugly hack, but Qt does not provide a indexOf for const pointers!
    int index = entries_.indexOf((KVListEntry*)entry);
    Q_ASSERT(index >= 0);

    // filter duplicates
    QSet<CbHandle*> callbacks;
    for(int role : modifiedRoles) {
        for(CbHandle *obj : entryChangedCallbacks_[role])
            callbacks << obj;
    }

    for(CbHandle *obj : callbacks)
        obj->func(entry);

    QModelIndex ix = QAbstractListModel::index(index);
    dataChanged(ix, ix, modifiedRoles);
}

QHash<int, QByteArray> KVListModel::setupModelRoleNames(const QMetaEnum &keysEnum) const
{
    QMetaEnum internalRoles = QMetaEnum::fromType<EnInternalKeys>();

#ifdef QT_DEBUG
    QStringList internalEnumNames;
    for(int i=0; i<internalRoles.keyCount(); i++)
        internalEnumNames << internalRoles.key(i);
#endif

    // the enum gives us the first role names
    QHash<int, QByteArray> hash = createHashFromEnum(keysEnum);

    // for each role name, we create a shadowed one
    for(int key : hash.keys())
    {
#ifdef QT_DEBUG
        Q_ASSERT(key >= 0 && key < KVListEntry::ShadowedKeysStartAt);
        Q_ASSERT(!internalEnumNames.contains(hash[key], Qt::CaseInsensitive));
#endif
        hash.insert(key+KVListEntry::ShadowedKeysStartAt, hash[key] + "_shadowed");
    }

    // append the internal role names
    for(int i=0; i<internalRoles.keyCount(); i++)
        hash.insert(internalRoles.value(i), internalRoles.key(i));

    return hash;
}

void KVListModel::insertInt(int i, KVListEntry *entry)
{
    if(!entry)
        return;
    beginInsertRows(QModelIndex(), i, i);
    entries_.insert(i, entry);
    connectEntry(entry);
    endInsertRows();
}

void KVListModel::moveInt(int from, int to)
{
    // qt's API is really strange here! make it more sane
    int to2 = to;
    if(to > from)
        to2++;

    if(beginMoveRows(QModelIndex(), from, from, QModelIndex(), to2)) {
        entries_.move(from, to);
        endMoveRows();
    } else {
        qWarning() << "move condition not satisfied; from/to:" << from << to2;
    }
}

KVListEntry *KVListModel::takeAtInt(int i)
{
    KVListEntry *e;
    beginRemoveRows(QModelIndex(), i, i);
    e=entries_.takeAt(i);
    disconnectEntry(e);
    endRemoveRows();
    return e;
}

void KVListModel::disconnectEntry(KVListEntry *entry)
{
    entry->model_ = nullptr;
}

void KVListModel::connectEntry(KVListEntry *entry) {
    entry->model_ = this;
}

