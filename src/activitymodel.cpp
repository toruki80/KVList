#include "activitymodel.h"

void ActivityModel::processActivities()
{
    static const QStringList allIcons = {
        "aqua-gym.png",
        "archery.png",
        "ball-games.png",
        "ball.png",
        "baseball.png",
        "basketball-1.png",
        "basketball.png",
        "bicycle.png",
        "boxing-1.png",
        "boxing.png",
        "canoe-1.png",
        "canoe.png",
        "chalice-1.png",
        "chalice.png",
        "chronometer.png",
        "equestrian.png",
        "equiptment.png",
        "fencing-1.png",
        "fencing.png",
        "field.png",
        "finish.png",
        "flag.png",
        "football-1.png",
        "football-2.png",
        "football.png",
        "golf.png",
        "gymnast-1.png",
        "gymnast-2.png",
        "gymnast-3.png",
        "gymnast-4.png",
        "gymnast-5.png",
        "gymnast.png",
        "gymnastic-1.png",
        "gymnastic-2.png",
        "gymnastic-3.png",
        "gymnastic-4.png",
        "gymnastic.png",
        "hammer-throw.png",
        "high-jump.png",
        "hockey.png",
        "javelin-throw.png",
        "karate.png",
        "kayak-1.png",
        "kayak-2.png",
        "kayak-3.png",
        "kayak.png",
        "lanes.png",
        "long-jump.png",
        "medal-1.png",
        "medal-2.png",
        "medal.png",
        "olympia.png",
        "olympic-games-1.png",
        "olympic-games.png",
        "olympic-torch-1.png",
        "olympic-torch.png",
        "paralympic.png",
        "pin-pong-1.png",
        "pin-pong.png",
        "podium.png",
        "pole-vault.png",
        "shirt.png",
        "shoot.png",
        "skates.png",
        "ski-1.png",
        "ski-2.png",
        "ski-3.png",
        "ski-jump.png",
        "ski.png",
        "snowboard.png",
        "sprint.png",
        "stadium.png",
        "start-gun.png",
        "swimmer.png",
        "swimmers.png",
        "target-shooter.png",
        "target.png",
        "tennis.png",
        "track-field.png",
        "velleyball.png",
        "volleyball.png",
        "water-polo.png",
        "water-sport-1.png",
        "water-sport-2.png",
        "water-sport.png",
        "water-sports.png",
        "weightlifter-1.png",
        "weightlifter.png",
        "weightlifting.png",
        "whistle.png",
        "wind-surf.png",
        "winner-1.png",
        "winner-2.png",
        "winner.png",
        "winners.png",
        "winter-games-1.png",
        "winter-games.png",
        "wrestling-1.png",
        "wrestling-2.png",
        "wrestling.png",

    };

    QMap<QString, ActivityEntry*> tmpMap;
    for(const QString &iconFilename : allIcons)
    {
        QString basename = iconFilename;
        basename.chop(4); // remove .png
        if(basename.back().isDigit())
            basename.chop(2);

        basename[0] = basename.front().toUpper();

        ActivityEntry *e = new ActivityEntry(this);
        e->setValues({
                         { ActivityEntry::selected, false },
                         { ActivityEntry::name, basename },
                         { ActivityEntry::iconRes, QString("icons/activity-icons/%1").arg(iconFilename) },
                     });

        tmpMap[basename] = e;
    }

    // xml has been loaded... now we need to add all available activities that are not contained in the xml
    // first of all lets just throw away the ones that are not active.. this simplifies processing a lot
    for(const KVListEntry *e : *this) {
        if (e->getValue(ActivityEntry::selected).toBool())
        {
            const QString name = e->getValue(ActivityEntry::name).toString();
            if(tmpMap.contains(name))
                tmpMap[name]->setValue(ActivityEntry::selected, true);
        }
    }

    beginResetModel();
    // clear old ones
    for(KVListEntry *e : entries_)
        disconnectEntry(e);
    qDeleteAll(entries_);
    entries_.clear();

    QMapIterator<QString, ActivityEntry*> i(tmpMap);
    while (i.hasNext()) {
        i.next();
        ActivityEntry *e = i.value();
        this->connectEntry(e);
        entries_ << e;
    }

    endResetModel();
}

