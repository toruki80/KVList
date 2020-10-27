#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include "friendsmodel.h"
#include "activitymodel.h"
#include "activityentry.h"
#include "kvlistserializer.h"


void registerTypes(QObject *parent) {
    qmlRegisterSingletonType<FriendsModel>("Insta", 1, 0, "FriendsModel", [=](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        QString loc = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir d(loc);
        d.mkpath(".");
        QString fname = d.filePath("friends.xml");

        FriendsModel *f = new FriendsModel(parent);
        f->setSerializationFile(fname);

        if(!f->deSerialize())
            f->createDefaultValues();

        return f;
    });

    qmlRegisterType<FriendsEntry>("Insta", 1, 0, "FriendsEntry");

    REGISTER_2_SERIALIZATION_FACTORY(FriendsModel);
    REGISTER_2_SERIALIZATION_FACTORY(FriendsEntry);
    REGISTER_2_SERIALIZATION_FACTORY(ActivityModel);
    REGISTER_2_SERIALIZATION_FACTORY(ActivityEntry);
}



int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    registerTypes(&app);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
