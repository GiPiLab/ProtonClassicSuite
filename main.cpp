#include "mainwindow.h"
#include "dbutils.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QLocale>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QTranslator qtTranslator;
    qtTranslator.load(
                "qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath)
                );
    a.installTranslator(&qtTranslator);

    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
    int retval;
    qsrand(time(NULL));


    QSqlDatabase db=loadDb(true);

    w.show();

    retval=a.exec();
    db.close();
    return retval;
}
