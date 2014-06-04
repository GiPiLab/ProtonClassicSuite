#include "mainwindow.h"
#include "utils.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QLocale>
#include <time.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load(
                "qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath)
                );
    a.installTranslator(&qtTranslator);

    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
    int retval;
    qsrand(time(NULL));

    QSqlDatabase::addDatabase("QSQLITE");

    MainWindow w;
    w.show();

    retval=a.exec();
    QSqlDatabase::database().close();
    return retval;
}
