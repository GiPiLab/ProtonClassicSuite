#include "mainwindow.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    int retval;
    qsrand(time(NULL));

    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbtest.sqlite3");
    if(!db.open())
    {
        qDebug()<<db.lastError();
    }

    w.show();

    retval=a.exec();
    db.close();
    qDebug()<<"DB closed";
    return retval;



}
