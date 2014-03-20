#include "utility.h"

#include <QDebug>
#include <QSqlDatabase>
#include <cstdlib>
#include <QApplication>

void die(int retcode)
{
    QSqlDatabase::database().close();
    QApplication::instance()->exit(retcode);
    exit(retcode);
}
