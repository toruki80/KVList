#ifndef KVLISTFILTEREDMODEL_H
#define KVLISTFILTEREDMODEL_H

#include "kvlistmodel.h"
#include "kvlistentry.h"
#include "kvlist_global.h"
#include <QSortFilterProxyModel>
#include <QVariant>

class KVLIST_EXPORT KVListFilteredModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit KVListFilteredModel(QObject* parent=nullptr);

    void setFilter(const QMap<KVListEntry::Key, QVariantList> &filter);
    Q_INVOKABLE KVListModel *getSourceModel() const { return dynamic_cast<KVListModel*>(QSortFilterProxyModel::sourceModel()); }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    Q_INVOKABLE void dbgMe() {
        qDebug() << this << sourceModel() << "info:" << this->rowCount() << "vs." << sourceModel()->rowCount();
        KVListModel *m = dynamic_cast<KVListModel*>(sourceModel());
        for(auto *e : *m)
            qDebug() << e->getValue(0);
    }

private:
    QMap<KVListEntry::Key, QVariantList> filterMap_;
};


#endif // KVLISTFILTEREDMODEL_H
