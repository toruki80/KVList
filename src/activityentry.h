#ifndef ACTIVITYENTRY_H
#define ACTIVITYENTRY_H


#include "kvlistentry.h"

class ActivityEntry : public KVListEntry
{
    Q_OBJECT

public:
    enum EnKey { name, iconRes, selected };
    Q_ENUM(EnKey)

    explicit ActivityEntry(QObject *parent = nullptr) : KVListEntry(parent) {}
};

#endif // ACTIVITYENTRY_H
