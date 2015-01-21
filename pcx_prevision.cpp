#include "pcx_prevision.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

PCx_Prevision::PCx_Prevision(unsigned int previsionId):previsionId(previsionId)
{
    QSqlQuery q;
    if(!q.exec(QString("select * from index_previsions where id='%1'").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        previsionName=q.value("nom").toString();
        unsigned int auditId=q.value("id_audit").toUInt();
        attachedAudit=new PCx_AuditWithTreeModel(auditId);

        creationTimeUTC=QDateTime::fromString(q.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
    }
    else
    {
        qCritical()<<"Invalid audit ID !";
        die();
    }

}


qint64 PCx_Prevision::computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionCriteria> criteriaToAdd, QList<PCx_PrevisionCriteria> criteriaToSubstract) const
{
    Q_ASSERT(nodeId>0);

    qint64 value=0;
    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToAdd)
    {
        value+=oneCriteria.compute(attachedAudit,mode,nodeId);
    }
    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToSubstract)
    {
        value-=oneCriteria.compute(attachedAudit,mode,nodeId);
    }
    return value;

}


bool PCx_Prevision::setPrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year, const QString &label, QList<PCx_PrevisionCriteria> criteriaToAdd, QList<PCx_PrevisionCriteria> criteriaToSubstract) const
{
    Q_ASSERT(nodeId>0);
    //FIXME : years
    //if(year<=attachedAudit->getYears().last())

    QStringList operatorsToAddSerialized,operatorsToSubstractSerialized;


    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToAdd)
    {
        operatorsToAddSerialized.append(oneCriteria.serialize());
    }
    foreach(PCx_PrevisionCriteria oneCriteria,criteriaToSubstract)
    {
        operatorsToSubstractSerialized.append(oneCriteria.serialize());
    }

    QSqlQuery q;
    q.prepare(QString("insert into prevision_%1_%2 (id_node,year,label,prevision_operators_to_add,"
                      "prevision_operators_to_substract) values (:id_node,:year,:label,:prevadd,:prevsub)")
              .arg(MODES::modeToTableString(mode))
              .arg(attachedAudit->getAuditId()));

    q.bindValue(":id_node",nodeId);
    q.bindValue(":year",year);
    q.bindValue(":label",label);
    q.bindValue(":prevadd",operatorsToAddSerialized.join(';'));
    q.bindValue(":prevsub",operatorsToSubstractSerialized.join(';'));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return false;
    }
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        return false;
    }
    return true;
}






qint64 PCx_Prevision::computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    Q_ASSERT(nodeId>0);

}





PCx_Prevision::~PCx_Prevision()
{

}


bool PCx_Prevision::previsionNameExists(const QString &previsionName)
{
    QSqlQuery q;
    q.prepare("select count(*) from index_previsions where nom=:name");
    q.bindValue(":name",previsionName);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.value(0).toLongLong()>0)
        return true;
    return false;
}


bool PCx_Prevision::deletePrevision(unsigned int previsionId)
{
    Q_ASSERT(previsionId>0);
    QSqlQuery q(QString("select count(*) from index_previsions where id='%1'").arg(previsionId));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qWarning()<<"Attempting to delete an inexistant prevision !";
            return false;
        }
    }

    QSqlDatabase::database().transaction();
    if(!q.exec(QString("delete from index_previsions where id='%1'").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table prevision_DF_%1").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table prevision_RF_%1").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table prevision_DI_%1").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table prevision_RI_%1").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().commit();
    return true;
}





QList<QPair<unsigned int, QString> > PCx_Prevision::getListOfPrevisions()
{
    QList<QPair<unsigned int,QString> > listOfPrevisions;
    QDateTime dt;

    QSqlQuery query;
    if(!query.exec("select * from index_previsions order by datetime(le_timestamp)"))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QString item;
        dt=QDateTime::fromString(query.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();
        QPair<unsigned int, QString> p;
        item=QString("%1 - %2").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
        p.first=query.value("id").toUInt();
        p.second=item;
        listOfPrevisions.append(p);
    }
    return listOfPrevisions;
}

bool PCx_Prevision::addNewPrevision(unsigned int auditId, const QString &name)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::FinishedAuditsOnly);
    QPair<unsigned int, QString> anAudit;

    bool found=false;
    foreach(anAudit,listOfAudits)
    {
        if(anAudit.first==auditId)
        {
            found=true;
            break;
        }
    }
    if(found==false)
    {
        qCritical()<<QObject::tr("Audit inexistant ou non terminé !");
        return false;
    }

    if(previsionNameExists(name))
    {
        QMessageBox::warning(0,QObject::tr("Attention"),QObject::tr("Il existe déjà une prévision portant ce nom !"));
        return false;
    }






    QSqlDatabase::database().transaction();
    q.prepare("insert into index_previsions (nom,id_audit) values (:nom,:id_audit)");
    q.bindValue(":nom",name);
    q.bindValue(":id_audit",auditId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    QVariant lastId=q.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<q.lastError();
        die();
    }
    unsigned int uLastId=lastId.toUInt();


    if(!q.exec(QString("create table prevision_DF_%1(id integer primary key autoincrement, id_node integer not null, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, unique(id_node,year) on conflict replace)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_prevision_DF_%1 on prevision_DF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table prevision_RF_%1(id integer primary key autoincrement, id_node integer not null, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, unique(id_node,year) on conflict replace)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_prevision_RF_%1 on prevision_RF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table prevision_DI_%1(id integer primary key autoincrement, id_node integer not null, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, unique(id_node,year) on conflict replace)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_prevision_DI_%1 on prevision_DI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table prevision_RI_%1(id integer primary key autoincrement, id_node integer not null, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, unique(id_node,year) on conflict replace)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_prevision_RI_%1 on prevision_RI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return true;
}


