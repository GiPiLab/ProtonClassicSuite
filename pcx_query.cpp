#include "utils.h"
#include "pcx_query.h"
#include "pcx_queryvariation.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>

PCx_Query::PCx_Query(PCx_Audit *model):model(model),queryId(0)
{
}

PCx_Query::PCx_Query(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri,
                         unsigned int year1, unsigned int year2, const QString &name):model(model),typeId(typeId),ored(ored),dfrfdiri(dfrfdiri),
                        name(name),queryId(0)
{
    setYears(year1,year2);
}

void PCx_Query::setYears(unsigned int year1, unsigned int year2)
{
    if(year1>year2)
    {
        unsigned int yeartmp;
        yeartmp=year1;
        year1=year2;
        year2=yeartmp;
    }
    unsigned int firstYear=model->getYears().first();
    unsigned int lastYear=model->getYears().last();
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

bool PCx_Query::remove(unsigned int queryId)
{
    return deleteQuery(model->getAuditId(),queryId);
}

void PCx_Query::createTableQueries(unsigned int auditId)
{
    QSqlQuery q;
    if(!q.exec(QString("create table audit_queries_%1(id integer primary key autoincrement, name text not null, "
                   "query_mode integer not null, target_type integer check (target_type>=0) not null, "
                   "ored integer not null, dfrfdiri integer not null, oper integer, "
                   "percent_or_point integer, increase_decrease integer,val1 integer not null default '0', val2 integer,"
                   "year1 integer not null, year2 integer not null)").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
}

QString PCx_Query::getCSS()
{
    QString css=PCx_QueryVariation::getCSS()+PCx_QueryRank::getCSS()+PCx_QueryMinMax::getCSS();
    return css;
}

bool PCx_Query::deleteQuery(unsigned int auditId,unsigned int queryId)
{
    Q_ASSERT(queryId>0 && auditId>0);
    QSqlQuery q;
    q.prepare(QString("delete from audit_queries_%1 where id=:id").arg(auditId));
    q.bindValue(":id",queryId);
    q.exec();

    if(q.numRowsAffected()==0)
    {
        qCritical()<<q.lastError();
        return false;
    }
    return true;
}
