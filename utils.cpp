#include "utils.h"

#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>



void die(int retcode)
{
    QSqlDatabase::database().close();
    QApplication::instance()->exit(retcode);
    exit(retcode);
}


QString formatCurrency(qint64 num)
{
    QLocale locale;
    return locale.toString(((double)num/FIXEDPOINTCOEFF),'f',2);
}



QString formatDouble(double num, unsigned int decimals)
{
    QLocale locale;
    return locale.toString(num,'f',decimals);
}

void initCurrentDb(void)
{
    QSqlQuery query;

    query.exec("create table if not exists index_arbres(id integer primary key autoincrement, nom text unique not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        qFatal(query.lastError().text().toLocal8Bit());
        exit(-1);
    }

    query.exec("create table if not exists index_audits(id integer primary key autoincrement, nom text unique not null, id_arbre integer not null, annees text not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        qFatal(query.lastError().text().toLocal8Bit());
        exit(-1);
    }
}

bool loadDb(const QString &databaseName)
{
    QSqlDatabase db=QSqlDatabase::database();
    if(!databaseName.isNull())
        db.setDatabaseName(databaseName);

    if(!db.open())
    {
        qFatal(db.lastError().text().toLocal8Bit());
        exit(-1);
    }
    return true;
}


QString newDb()
{
    QString tmpDbName=QUuid::createUuid().toString();
    tmpDbName.chop(1);
    tmpDbName=tmpDbName.remove(0,1);
    tmpDbName.append(".pcxdb");

    tmpDbName.prepend(QDir::separator());
    tmpDbName.prepend(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    return tmpDbName;
}

QString generateUniqueFileName(const QString &suffix)
{
    QString imageName=QUuid::createUuid().toString();
    imageName.chop(1);
    imageName=imageName.remove(0,1).append(suffix);
    return imageName;
}




