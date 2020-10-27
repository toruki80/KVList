#ifndef ACTIVITYMODEL_H
#define ACTIVITYMODEL_H

#include "kvlistmodel.h"
#include "kvlistfilteredmodel.h"
#include "activityentry.h"

class ActivityModel : public KVListModel
{
    Q_OBJECT

public:
    ActivityModel(QObject *parent=nullptr)  : KVListModel(QMetaEnum::fromType<ActivityEntry::EnKey>(), parent) {}
    void processActivities();
private:
};



#endif // ACTIVITYMODEL_H
