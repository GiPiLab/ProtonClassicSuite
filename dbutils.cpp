#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QFile>
#include "dbutils.h"



void emptyDb(void)
{
    QSqlDatabase db=QSqlDatabase::database();
    db.close();
        //db.removeDatabase(db.connectionName());
        if(!QFile::remove(DBNAME))
        {
            qCritical()<<"Error cleaning default database";
            exit(-1);
        }
   loadDb(false);
   initDb();
}

void initDb(void)
{
    QSqlQuery query;
    QSqlDatabase db=QSqlDatabase::database();


    query.exec("create table if not exists index_arbres(id integer primary key autoincrement, nom text unique not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        db.close();
        qCritical()<<query.lastError().text();
        exit(-1);
    }

    query.exec("create table if not exists index_audits(id integer primary key autoincrement, nom text unique not null, id_arbre integer not null, annees text not null, termine integer not null default 0, le_timestamp text default current_timestamp)");
    if(query.numRowsAffected()==-1)
    {
        db.close();
        qCritical()<<query.lastError().text();
        exit(-1);
    }
}


QSqlDatabase loadDb(bool addDriver)
{
    QSqlDatabase db;
    if(addDriver)
    {
        db=QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(DBNAME);
    }

    else
    {
        db=QSqlDatabase::database();
    }

    if(!db.open())
    {
        qCritical()<<db.lastError();
        exit(-1);
    }
    initDb();
    return db;
}

