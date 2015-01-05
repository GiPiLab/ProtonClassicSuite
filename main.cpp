#include "mainwindow.h"
#include "utils.h"
#include <QApplication>
#include <QSharedMemory>
#include <QSqlDatabase>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QLocale>
#include <time.h>
#include <iostream>
#include <QSqlQuery>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message=QString("(%5) %1 (%2:%3 [%4])\n").arg(msg).arg(context.file).arg(context.line)
            .arg(context.function).arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));

    switch (type) {
    case QtDebugMsg:
        message.prepend("[Debug]");
        std::cerr<<qPrintable(message);
        break;
    case QtWarningMsg:
        message.prepend("[Warning]");
        QMessageBox::warning(0,"Attention",message);
        std::cerr<<qPrintable(message);
        break;
    case QtCriticalMsg:
        message.prepend("[Critical]");
        QMessageBox::critical(0,"Erreur critique",message);
        std::cerr<<qPrintable(message);
        break;
    case QtFatalMsg:
        message.prepend("[Fatal]");
        QMessageBox::critical(0,"Erreur fatale",message);
        std::cerr<<qPrintable(message);
        die();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Avoid multiple application instances
    QSharedMemory sharedMemory;
    sharedMemory.setKey("GIPILABPROTONCLASSICSUITE");
    sharedMemory.attach(QSharedMemory::ReadWrite);

    if (!sharedMemory.create(1)) {
        return EXIT_FAILURE;
    }

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);


    //qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");

    qInstallMessageHandler(myMessageOutput);

    //qDebug()<<"***ProtonClassicSuite starting***";

/*
    QPixmap pixmap(":/icons/icons/logo.png");
    QSplashScreen splash(pixmap);
    splash.show();


    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
*/


    int retval;

    QSqlDatabase::addDatabase("QSQLITE");

    MainWindow w;
    w.show();
  //  splash.finish(&w);

    retval=a.exec();
    QSqlQuery q;
    q.exec("vacuum");

    QSqlDatabase::database().close();
    sharedMemory.detach();
    return retval;
}
