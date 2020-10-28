#include "kvlistfilteredmodel.h"


KVListFilteredModel::KVListFilteredModel(QObject *parent) : QSortFilterProxyModel(parent) {
    KVListModel *m = dynamic_cast<KVListModel*>(parent);
    if(m)
        setSourceModel(m);
    else
        Q_ASSERT(false);
}

void KVListFilteredModel::setFilter(const QMap<KVListEntry::Key, QVariantList> &filter)
{
    filterMap_ = filter;
    invalidateFilter();
}

bool KVListFilteredModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_ASSERT(QSortFilterProxyModel::sourceModel());


    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    auto i = filterMap_.constBegin();
    bool res = true;
    while (i != filterMap_.constEnd()) {
        const QVariant &value = sourceModel()->data(index, i.key());
        if (!i.value().contains(value)) {
            res = false;
            break;
        }
        else
            ++i;
    }

    return res;
}
