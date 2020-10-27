#ifndef FRIENDSMODEL_H
#define FRIENDSMODEL_H

#include "kvlistmodel.h"
#include "friendsentry.h"
#include <QTimer>

class FriendsModel : public KVListModel
{
    Q_OBJECT

public:
    // constructor
    FriendsModel(QObject *parent=nullptr) : KVListModel(QMetaEnum::fromType<FriendsEntry::EnKey>(), parent) {}

    // override the deSerialize function as wee need to make some adjustments afterwards
    virtual bool deSerialize() override;

    Q_INVOKABLE int addNewEntry() {
        // some default values when the user clicks 'add' button
        // important is that we set flag 'newlyCreatedEntry_ns'
        // this will tell the ui to go directly to the edit page
        *this << FriendsEntry::create({
                                          {FriendsEntry::firstname, "John"},
                                          {FriendsEntry::surname, "Doe"},
                                          {FriendsEntry::newlyCreatedEntry_ns, true},
                                      }, this);
        return size() -1;
    }

    void createDefaultValues();
};

#endif // FRIENDSMODEL_H
