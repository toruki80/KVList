#include "friendsmodel.h"

bool FriendsModel::deSerialize()
{
    bool res = KVListModel::deSerialize();

    // after deserialization we need to adjust the activities
    // activities have been loaded from xml.. need to merge those with the "available"
    // activities.. this allows us to later add more activities and not loose users choices then
    //
    for(KVListEntry *e : *this) {
        KVListModel *m = e->getChildModel(FriendsEntry::activitiesAll);
        Q_ASSERT(m);
        ActivityModel *m2 = dynamic_cast<ActivityModel*>(m);
        Q_ASSERT(m2);
        m2->processActivities();
    }

    return res;
}

void FriendsModel::createDefaultValues()
{
    *this << FriendsEntry::create({
                                      {FriendsEntry::firstname, "Donald"},
                                      {FriendsEntry::surname, "Trump"},
                                      {FriendsEntry::email, "donald.trump@whitehouse.gov "},
                                  });

    *this << FriendsEntry::create({
                                      {FriendsEntry::firstname, "Barack"},
                                      {FriendsEntry::surname, "Obama"},
                                      {FriendsEntry::email, "barack.obama@whitehouse.gov "},
                                  });

    *this << FriendsEntry::create({
                                      {FriendsEntry::firstname, "George W."},
                                      {FriendsEntry::surname, "Bush"},
                                      {FriendsEntry::email, "george.bush@whitehouse.gov "},
                                  });
}
