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
        attachedAuditId=q.value("id_audit").toUInt();
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


qint64 PCx_Prevision::computePrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionItemCriteria> criteriaToAdd, QList<PCx_PrevisionItemCriteria> criteriaToSubstract) const
{
    Q_ASSERT(nodeId>0);

    qint64 value=0;

    foreach(PCx_PrevisionItemCriteria oneCriteria,criteriaToAdd)
    {
        value+=oneCriteria.compute(attachedAuditId,mode,nodeId);
    }
    foreach(PCx_PrevisionItemCriteria oneCriteria,criteriaToSubstract)
    {
        value-=oneCriteria.compute(attachedAuditId,mode,nodeId);
    }
    return value;
}


QHash<QString,QString> PCx_Prevision::getPrevisionItemDescription(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year) const
{
    QSqlQuery q;
    QHash<QString,QString> description;

    q.prepare(QString("select * from prevision_%1_%2 where id_node=:id_node and year=:year").arg(MODES::modeToTableString(mode))
              .arg(previsionId));
    q.bindValue(":id_node",nodeId);
    q.bindValue(":year",year);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        description.insert("label",q.value("label").toString());
        PCx_PrevisionItemCriteria criteria;

        QStringList items=q.value("prevision_operators_to_add").toString().split(';');
        QStringList itemDescriptions;
        foreach(const QString &item,items)
        {
            if(criteria.unserialize(item))
            {
                itemDescriptions.append(criteria.getCriteriaLongDescription());
            }
            else
            {
                qDebug()<<"Unable to decode";
                return description;
            }

        }
        description.insert("add",itemDescriptions.join('+'));

        itemDescriptions.clear();
        items.clear();
        items=q.value("prevision_operators_to_substract").toString().split(';');

        foreach(const QString &item,items)
        {
            if(criteria.unserialize(item))
            {
                itemDescriptions.append(criteria.getCriteriaLongDescription());
            }
            else
            {
                qDebug()<<"Unable to decode";
                return description;
            }

        }
        description.insert("remove",itemDescriptions.join('-'));
    }

    return description;
}


int PCx_Prevision::setPrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year, const QString &label,
                                    QList<PCx_PrevisionItemCriteria> criteriaToAdd, QList<PCx_PrevisionItemCriteria> criteriaToSubstract) const
{
    Q_ASSERT(nodeId>0);

    QStringList operatorsToAddSerialized,operatorsToSubstractSerialized;


    foreach(PCx_PrevisionItemCriteria oneCriteria,criteriaToAdd)
    {
        operatorsToAddSerialized.append(oneCriteria.serialize());
    }
    foreach(PCx_PrevisionItemCriteria oneCriteria,criteriaToSubstract)
    {
        operatorsToSubstractSerialized.append(oneCriteria.serialize());
    }

    QSqlQuery q;
    q.prepare(QString("insert into prevision_%1_%2 (id_node,year,label,prevision_operators_to_add,"
                      "prevision_operators_to_substract) values (:id_node,:year,:label,:prevadd,:prevsub)")
              .arg(MODES::modeToTableString(mode))
              .arg(previsionId));

    q.bindValue(":id_node",nodeId);
    q.bindValue(":year",year);
    q.bindValue(":label",label);
    q.bindValue(":prevadd",operatorsToAddSerialized.join(';'));
    q.bindValue(":prevsub",operatorsToSubstractSerialized.join(';'));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return -1;
    }
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        return -1;
    }
    return q.lastInsertId().toInt();
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


