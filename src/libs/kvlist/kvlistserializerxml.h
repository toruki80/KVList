#ifndef KVLISTSERIALIZERXML_H
#define KVLISTSERIALIZERXML_H

#include "kvlistserializer.h"
#include "kvlistentry.h"
#include <QDomElement>
#include <QDomDocument>



class KVListSerializerXml : public KVListSerializer
{
    Q_OBJECT

public:
    explicit KVListSerializerXml(QObject *parent = nullptr);
    virtual ~KVListSerializerXml() = default;

    bool serialize(KVListModel *model, const QString &filename) override;
    KVListModel* deserializeToNewModel(const QString &filename) override;
    bool deserializeToExistingModel(KVListModel* model, const QString &filename) override;

private:
    QDomElement serialize(QDomDocument &document, KVListModel *model);
    QDomElement serialize(QDomDocument &document, KVListEntry *entry);
    QDomElement serialize(QDomDocument &document, const QString key, const QVariant  &value);

    KVListModel* deserializeChildModel(const QDomElement &el, KVListModel *destination=nullptr);
    bool deserializeModel(const QDomElement &el, KVListModel* model);
    KVListEntry *deserializeEntry(const QDomElement &el, KVListModel *model, const QVersionNumber &modelVersion);
    QPair<KVListEntry::Key, QVariant> deserializeValue(const QDomElement &el, KVListModel *model, KVListEntry *entry, const QVersionNumber &modelVersion);
};

#endif // KVLISTSERIALIZERXML_H
