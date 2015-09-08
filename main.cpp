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
    QString message=QString("%1 (%2:%3)\n").arg(msg).arg(context.file).arg(context.line);

    switch (type) {

    case QtInfoMsg:
        message.prepend("[I]");
        std::cerr<<qPrintable(message);
        break;

    case QtDebugMsg:
        message.prepend("[D]");
        std::cerr<<qPrintable(message);
        break;
    case QtWarningMsg:
        message.prepend("[W]");
        QMessageBox::warning(0,"Attention",message);
        std::cerr<<qPrintable(message);
        break;
    case QtCriticalMsg:
        message.prepend("[C]");
        QMessageBox::critical(0,"Erreur critique",message);
        std::cerr<<qPrintable(message);
        break;
    case QtFatalMsg:
        message.prepend("[F]");
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
    sharedMemory.attach();

    if (!sharedMemory.create(1)) {
        QMessageBox::information(nullptr,"Attention","Une seule instance de ProtonClassicSuite est autorisée à la fois");
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

    QSqlDatabase::database().close();
    sharedMemory.detach();
    return retval;
}
