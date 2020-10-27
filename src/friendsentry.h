#ifndef FRIENDSENTRY_H
#define FRIENDSENTRY_H

#include "kvlistentry.h"
#include <QDateTime>
#include <QTimer>
#include <QUrl>
#include "activitymodel.h"

class FriendsEntry : public KVListEntry
{
    Q_OBJECT

public:
    enum EnKey { firstname, surname, displayName_ns, email, phonenumber, address,
                 displayLastseen_ns, lastseen, uid, activitiesAll, activitiesSelected, logoURL, newlyCreatedEntry_ns };
    Q_ENUM(EnKey)

    // constructor
    explicit FriendsEntry(QObject * parent=nullptr) : KVListEntry(parent) { init(); }

    // convenicence function that allows us to create a entry with default values
    static FriendsEntry *create(const KeyValueMap &initialValues, QObject* parent=nullptr);

    // the user chooses a local file as logo...
    // need to copy and process the logo first.. afterwards we simply store the updated url
    virtual int applyShadowedChanges() override;

    // call this function when we had contact to our friend here...
    Q_INVOKABLE void updateLastSeenStatus(bool hasContact);

private:
    void addNewLogo();
    QImage cropImg(const QImage &src) const;
    void init();
    QTimer *contactStatusUpdateTimer_;
};

#endif // FRIENDSENTRY_H
