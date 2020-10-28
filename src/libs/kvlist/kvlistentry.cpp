#include "kvlistentry.h"
#include "kvlistmodel.h"
#include <QDebug>


KVListEntry::KVListEntry(QObject *parent) :
    QObject(parent),
    model_(nullptr){}

KVListEntry::~KVListEntry()
{
    QSet<CbHandle*> collect;

    // collect them in a set first, as different keys might contain the same callback...
    for(Key key : keyModifiedCallbacks_.keys()) {
        for(CbHandle *h : keyModifiedCallbacks_[key])
            collect.insert(h);
    }
    qDeleteAll(collect);
    keyModifiedCallbacks_.clear();
}

QVariant KVListEntry::getValue(Key key) const
{
    if(key >= ShadowedKeysStartAt)
    {
        key -= ShadowedKeysStartAt;
        if(keyValueStoreShadowed_.contains(key)) // in case there is no entry for the shadowed value, we return the regular one
            return keyValueStoreShadowed_[key];
    }

    return keyValueStore_[key];
}

QVariant &KVListEntry::getValue(KVListEntry::Key key)
{
    if(key >= ShadowedKeysStartAt)
    {
        key -= ShadowedKeysStartAt;
        if(keyValueStoreShadowed_.contains(key)) // in case there is no entry for the shadowed value, we return the regular one
            return keyValueStoreShadowed_[key];
    }

    return keyValueStore_[key];
}

void KVListEntry::setValue(Key key, const QVariant &value)
{
    if(setValueInt(key, value)) {
        QVector<Key> modifies = { key };
        notifyValueChangedCallbacks(modifies);
        invalidateModel(modifies);
    }
}

void KVListEntry::setChildModel(Key key, KVListModel *model)
{
    setValue(key, QVariant::fromValue(model));
}

KVListModel *KVListEntry::getChildModel(KVListEntry::Key key) const
{
   return getValue(key).value<KVListModel*>();
}

void KVListEntry::setValues(const QMap<Key, QVariant> &values)
{
    QVector<Key> modifies;

    for(Key key : values.keys())
    {
        if(setValueInt(key, values[key]))
            modifies << key;
    }

    if(!modifies.isEmpty()) {
        notifyValueChangedCallbacks(modifies);
        invalidateModel(modifies);
    }
}

QVariant KVListEntry::getShadowedValue(KVListEntry::Key key) const
{
    return getValue(SHADOWED_KEY(key));
}

void KVListEntry::setShadowedValue(KVListEntry::Key key, const QVariant &value)
{
    setValue(SHADOWED_KEY(key), value);
}

void KVListEntry::setShadowedValues(const KeyValueMap &values) {
    QMap<Key, QVariant> values2;
    for(Key key : values.keys())
        values2[SHADOWED_KEY(key)] = values[key];
    setValues(values2);
}

KVListEntry::CbHandle* KVListEntry::onValueChanged(const QVector<Key> &keys, KVListEntry::ValueChangedCallbackFunc func)
{
    CbHandle *obj = new CbHandle;
    obj->func = func;
    for(Key key : keys)
        keyModifiedCallbacks_[key] << obj;
    return obj;
}

KVListEntry::CbHandle* KVListEntry::onValueChanged(Key key, KVListEntry::ValueChangedCallbackFunc func)
{
    CbHandle *obj = new CbHandle;
    obj->func = func;
    keyModifiedCallbacks_[key] << obj;
    return obj;
}

void KVListEntry::removeOnValueChanged(KVListEntry::CbHandle *obj)
{
    for(Key key : keyModifiedCallbacks_.keys())
        keyModifiedCallbacks_[key].removeAll(obj);

    delete obj;
}

void KVListEntry::revertShadowedChanges() {
    keyValueStoreShadowed_.clear();
}

int KVListEntry::applyShadowedChanges() {
    QVector<Key> modifies;
    for(Key key : keyValueStoreShadowed_.keys()) {
        if(setValueInt(key, keyValueStoreShadowed_[key]))
            modifies << key;
    }
    keyValueStoreShadowed_.clear();
    if(!modifies.isEmpty()){
        notifyValueChangedCallbacks(modifies);
        invalidateModel(modifies);
    }
    return modifies.size();
}


void KVListEntry::invalidateModel(const QVector<Key> &keys) const
{
    if(model_)
        model_->entryHasChanged(this, keys);
}

void KVListEntry::notifyValueChangedCallbacks(const QVector<Key> &keys) const
{
    // in case only one key has been changed, simply call the stored callbacks
    if(keys.size() == 1) {
        for(CbHandle *o : keyModifiedCallbacks_.value(keys[0]))
            o->func();
        return;
    }

    // different keys might have the same callback... remove duplicates
    QSet<CbHandle*> set;
    for(Key key : keys){
        if(keyModifiedCallbacks_.contains(key)){
            const QVector<CbHandle*> &callbacks = keyModifiedCallbacks_.value(key);
            for(CbHandle *o : callbacks)
                set << o;
        }
    }

    for(CbHandle *o : set)
        o->func();
}

bool KVListEntry::setValueInt(Key key, const QVariant &value)
{
    QVariant *v = nullptr;

    if(key >= ShadowedKeysStartAt)
        v = &keyValueStoreShadowed_[key-ShadowedKeysStartAt];
    else
        v = &keyValueStore_[key];

    if(*v != value) {
        *v = value;
        return true;
    }

    return false;
}
