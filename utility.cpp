#include "utility.h"

#include <QDebug>
#include <QSqlDatabase>
#include <cstdlib>
#include <QApplication>

void die(int retcode)
{
    QSqlDatabase::database().close();
    qDebug()<<"DB Closed on die";
    QApplication::instance()->exit(retcode);
    exit(retcode);
}
