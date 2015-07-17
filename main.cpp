#include <QCoreApplication>
#include <QSettings>
#include <memory>
#include "common_util.h"
#include "QsLog.h"
#include <iostream>
#include "QsLogDest.h"
#include <QFile>
#include "config.h"
#include "brain_stroke_server.h"
#include "brain_stroke_web.h"
#include "httplistener.h"
#include <iostream>
#include <QSettings>
#include <QProcess>
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //init config
    Config * config = Config::single();

    //init setting
    QSettings* listenerSettings =
            new QSettings(Config::single()->mp["server_setting"],QSettings::IniFormat,&app);
    listenerSettings->beginGroup("listener");

    //init file settings
    QSettings* fileSettings =
            new QSettings(Config::single()->mp["server_setting"],QSettings::IniFormat,&app);
    fileSettings->beginGroup("files");

    brain_stroke_web::StaticFileController_ins = new StaticFileController(fileSettings,&app);
    //init log
    QsLogging::Logger & log = QsLogging::Logger::instance();
    log.setLoggingLevel(QsLogging::DebugLevel);
    log.addDestination(QsLogging::DestinationFactory::MakeFileDestination(config -> mp["log_file"]));

    //init tcp server
    new brain_stroke_server(&app);

    //init web server
    new HttpListener(listenerSettings,new brain_stroke_web(&app),&app);
    return app.exec();
}
