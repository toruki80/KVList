#include "friendsentry.h"
#include <QObject>
#include <QUuid>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QImage>
#include <QtGlobal>

static const QUrl defaultLogo_("qrc:///qml/icons/default_user.png");
static QString logoUrlTemplate_;

FriendsEntry *FriendsEntry::create(const KVListEntry::KeyValueMap &initialValues, QObject *parent) {
    FriendsEntry *e = new FriendsEntry(parent);
    e->setValues(initialValues);
    return e;
}

int FriendsEntry::applyShadowedChanges() {
    // in case a new logo has been set, we need to copy it first and then adjust the url
    if(keyValueStoreShadowed_.contains(logoURL))
        addNewLogo();

    return KVListEntry::applyShadowedChanges();
}

void FriendsEntry::updateLastSeenStatus(bool hasContact) {
    QDateTime d;
    qint64 updateStatusInSecs;
    if(hasContact) {
        d = QDateTime::currentDateTime();
        setValue(lastseen, d);
    } else
        d = getValue(lastseen).toDateTime();

    // never, now, minutes, hours, weekdays, weeks, month, years
    QString displayStr;
    if(!d.isValid()) {
        displayStr = "never";
        updateStatusInSecs = -1;
    }
    else
    {
        qint64 seconds = d.secsTo(QDateTime::currentDateTime());

        if(seconds < 60) {
            displayStr = "now";
            updateStatusInSecs = 60-seconds;
        } else if(seconds < 60*60) {
            displayStr = QString("%1 minutes ago").arg(seconds/60);
            updateStatusInSecs = 60*60-seconds;
        } else if (seconds < 60*60*24) {
            displayStr = QString("%1 hours ago").arg(seconds/(60*60));
            updateStatusInSecs = 60*60*24-seconds;
        } else if (seconds < 60*60*24*7) {
            displayStr = d.date().toString("dddd");
            updateStatusInSecs = 60*60*24*7-seconds;
        } else if(seconds < 60*60*24*365) {
            displayStr = d.date().toString("MMMM");
            updateStatusInSecs = 60*60*24*365-seconds;
        } else {
            displayStr = QString("%1 years").arg(seconds/(60*60*24*365));
            updateStatusInSecs = 60*60*24*365-seconds;
        }
    }

    if(updateStatusInSecs < 0)
        contactStatusUpdateTimer_->stop();
    else
        contactStatusUpdateTimer_->start(updateStatusInSecs * 1000);

    setValue(displayLastseen_ns, displayStr);
}


void FriendsEntry::addNewLogo()
{
    QString localFile = keyValueStoreShadowed_[logoURL].toUrl().toLocalFile();
    QString newFile = QString(logoUrlTemplate_).arg(keyValueStore_[uid].toUuid().toString(QUuid::WithoutBraces));

    // remove the old file, if there is any
    QUrl urlOld = keyValueStore_[logoURL].toUrl();
    if(urlOld.isLocalFile())
        QFile::remove(urlOld.toLocalFile());

    QImage img = cropImg(QImage(localFile));
    img.save(newFile);

    keyValueStoreShadowed_[logoURL] = QUrl::fromLocalFile(newFile);
}

QImage FriendsEntry::cropImg(const QImage &src) const
{
    int min = qMin(src.width(), src.height());
    QRect r1(0,0, src.width(), src.height()), r2(0,0, min, min);
    r2.moveCenter(r1.center());
    QImage img = src.copy(r2);

    if(img.width() > 512)
        img = img.scaled(512, 512);

    return img;
}

void FriendsEntry::init()
{
    if(logoUrlTemplate_.isEmpty()) {
        QString loc = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir d(loc);
        d.mkpath("./logos");
        d.cd("logos");

        logoUrlTemplate_ = d.path() + QDir::separator() + "%1.png";
    }

    setValues({
                  {phonenumber, ""},
                  {address, ""},
                  {email, ""},
                  {phonenumber, ""},
                  {logoURL, defaultLogo_},
                  {newlyCreatedEntry_ns, false},
              });

    //each entry on creation should contain a uid
    setValue(uid, QUuid::createUuid());

    // prepare the name to be displayed whenever first-/surename changes
    onValueChanged(QVector<Key>() << firstname << surname, [this](){
        QString name, name1 = getValue(firstname).toString(), name2 = getValue(surname).toString();
        if(name1.isEmpty() && name2.isEmpty())
            name = "unkown";
        else if(name1.isEmpty())
            name = name2;
        else if (name2.isEmpty())
            name = name1;
        else
            name = name1 + ' ' + name2.at(0);
        setValue(displayName_ns, name);
    });

    onValueChanged(logoURL, [=](){
        QUrl url = getValue(logoURL).toUrl();
        if(url.isLocalFile() && !QFile::exists(url.toLocalFile()))
            setValue(logoURL, defaultLogo_);

    });


    // prepare the last-seen status
    contactStatusUpdateTimer_ = new QTimer(this);
    contactStatusUpdateTimer_->setSingleShot(true);
    connect(contactStatusUpdateTimer_, &QTimer::timeout, [this](){
        updateLastSeenStatus(false);
    });
    onValueChanged(lastseen, [=](){
        updateLastSeenStatus(false);
    });
    updateLastSeenStatus(false);

    // add a activity submodel
    ActivityModel * m = new ActivityModel(this);
    m->processActivities();
    setChildModel(activitiesAll, m);

    // add a model that filters above (selected only)
    KVListFilteredModel *fm = new KVListFilteredModel(m);
    fm->setFilter({{ActivityEntry::selected, QVariantList() << true}});
    setValue(activitiesSelected, QVariant::fromValue(fm));
}




