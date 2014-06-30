#include "pcx_queries.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>


PCx_Queries::PCx_Queries():model(NULL)
{
}

PCx_Queries::PCx_Queries(PCx_AuditModel *model):model(model)
{
}

PCx_Queries::PCx_Queries(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri,
                         unsigned int year1, unsigned int year2, const QString &name):model(model),typeId(typeId),ored(ored),dfrfdiri(dfrfdiri),
                        name(name)
{
    setYears(year1,year2);
}

void PCx_Queries::setYears(unsigned int year1, unsigned int year2)
{
    unsigned int yeartmp;
    Q_ASSERT(year1!=year2);

    if(year1>year2)
    {
        yeartmp=year1;
        year1=year2;
        year2=yeartmp;
    }
    PCx_AuditInfos infos=model->getAuditInfos();
    unsigned int firstYear=infos.years.first();
    unsigned int lastYear=infos.years.last();
    if(year1<firstYear)
    {
        year1=firstYear;
    }

    if(year2>lastYear)
    {
        year2=lastYear;
    }
    this->year1=year1;
    this->year2=year2;
}

bool PCx_Queries::remove(unsigned int queryId)
{
    QSqlQuery q;
    if(model==NULL)return false;
    q.prepare(QString("delete from audit_requetes_%1 where id=:id").arg(model->getAuditId()));
    q.bindValue(":id",queryId);
    q.exec();
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError().text();
        return false;
    }
    return true;
}

bool PCx_Queries::createTableQueries(unsigned int auditId)
{
    QSqlQuery q;
    q.exec(QString("create table audit_queries_%1(id integer primary key autoincrement, name text unique not null, "
                   "query_mode integer not null, target_type integer check (target_type>=0) not null, "
                   "ored integer not null, dfrfdiri integer not null, oper integer, "
                   "percent_or_point integer, increase_decrease integer,val1 real not null default '0.0', val2 real,"
                   "year1 integer not null, year2 integer not null)").arg(auditId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        return false;
    }
    return true;
}
