#include "pcx_prevision.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_Prevision::PCx_Prevision(PCx_Audit *audit):audit(audit)
{

    
}


void PCx_Prevision::createTablePrevisions(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;
    if(!q.exec(QString("create table audit_previsions_DF_%1(id integer primary key autoincrement, id_node integer not null, "
                   "prevision_operators_to_add text, prevision_operators_to_substract text)").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create table audit_previsions_RF_%1(id integer primary key autoincrement, id_node integer not null, "
                   "prevision_operators_to_add text, prevision_operators_to_substract text)").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create table audit_previsions_DI_%1(id integer primary key autoincrement, id_node integer not null, "
                   "prevision_operators_to_add text, prevision_operators_to_substract text)").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create table audit_previsions_RI_%1(id integer primary key autoincrement, id_node integer not null, "
                   "prevision_operators_to_add text, prevision_operators_to_substract text)").arg(auditId)))
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


qint64 PCx_Prevision::computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionCriteria> criteriaToAdd, QList<PCx_PrevisionCriteria> criteriaToSubstract) const
{
    Q_ASSERT(nodeId>0);

    qint64 value=0;
    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToAdd)
    {
        value+=oneCriteria.compute(audit,mode,nodeId);
    }
    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToSubstract)
    {
        value-=oneCriteria.compute(audit,mode,nodeId);
    }
    return value;

}

qint64 PCx_Prevision::computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    Q_ASSERT(nodeId>0);

}





PCx_Prevision::~PCx_Prevision()
{

}


