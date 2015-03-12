#include "pcx_prevision.h"
#include "pcx_previsionitem.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>


PCx_Prevision::PCx_Prevision(unsigned int previsionId):previsionId(previsionId)
{
    QSqlQuery q;
    attachedAudit=nullptr;
    if(!q.exec(QString("select * from index_previsions where id='%1'").arg(previsionId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        previsionName=q.value("nom").toString();
        attachedAuditId=q.value("id_audit").toUInt();
        attachedAudit=new PCx_Audit(attachedAuditId);
        attachedTree=attachedAudit->getAttachedTree();
        attachedTreeId=attachedAudit->getAttachedTreeId();
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

int PCx_Prevision::toPrevisionalExtendedAudit(const QString &newAuditName)
{
    if(PCx_Audit::auditNameExists(newAuditName))
    {
        qWarning()<<QObject::tr("Il existe déjà un audit portant ce nom");
        return -1;
    }
    QList<unsigned int> years=attachedAudit->getYears();
    qSort(years);
    int lastYear=years.last()+1;
    years.append(lastYear);

    int res=attachedAudit->duplicateAudit(newAuditName,years,true,true,true,true);
    if(res<0)
        return res;

    PCx_Audit dupAudit(res);

    QList<unsigned int> leaves=attachedTree->getLeavesId();
    QList<MODES::DFRFDIRI> modes={MODES::DF,MODES::RF,MODES::DI,MODES::RI};
    QHash<PCx_Audit::ORED,double> values;
    QSqlDatabase::database().transaction();
    foreach(unsigned int leaf,leaves)
    {
        foreach(MODES::DFRFDIRI unMode,modes)
        {
            PCx_PrevisionItem prevItem(this,unMode,leaf,lastYear);
            prevItem.loadFromDb();
            values.clear();
            values.insert(PCx_Audit::OUVERTS,NUMBERSFORMAT::fixedPointToDouble(prevItem.getSummedPrevisionItemValue()));
            dupAudit.setLeafValues(leaf,unMode,lastYear,values,true);
        }
    }
    QSqlDatabase::database().commit();
    return res;
}

bool PCx_Prevision::isPrevisionEmpty() const
{
    QSqlQuery q;

    q.exec(QString("select count(*) from prevision_DF_%1 where prevision_operators_to_add is not null or prevision_operators_to_substract is not null").arg(previsionId));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next() && q.value(0).toInt()>0)
        return false;

    q.exec(QString("select count(*) from prevision_RF_%1 where prevision_operators_to_add is not null or prevision_operators_to_substract is not null").arg(previsionId));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next() && q.value(0).toInt()>0)
        return false;

    q.exec(QString("select count(*) from prevision_DI_%1 where prevision_operators_to_add is not null or prevision_operators_to_substract is not null").arg(previsionId));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next() && q.value(0).toInt()>0)
        return false;

    q.exec(QString("select count(*) from prevision_RI_%1 where prevision_operators_to_add is not null or prevision_operators_to_substract is not null").arg(previsionId));
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next() && q.value(0).toInt()>0)
        return false;


    return true;
}

PCx_Prevision::~PCx_Prevision()
{
    attachedTree=nullptr;
    if(attachedAudit!=nullptr)
        delete attachedAudit;
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
    if(previsionId==0)
    {
        qFatal("Assertion failed");
    }
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
    if(auditId==0||name.isEmpty()||previsionNameExists(name)||name.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;

    PCx_Audit tmpAudit(auditId,true);

    if(tmpAudit.isFinished()==false)
    {
        qCritical()<<QObject::tr("Audit non terminé !");
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


    if(!q.exec(QString("create table prevision_DF_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, computedPrevision integer,"
                       " unique(id_node,year) on conflict replace)").arg(uLastId).arg(tmpAudit.getAttachedTreeId())))
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

    if(!q.exec(QString("create table prevision_RF_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, computedPrevision integer,"
                       " unique(id_node,year) on conflict replace)").arg(uLastId).arg(tmpAudit.getAttachedTreeId())))
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

    if(!q.exec(QString("create table prevision_DI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, computedPrevision integer,"
                       " unique(id_node,year) on conflict replace)").arg(uLastId).arg(tmpAudit.getAttachedTreeId())))
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

    if(!q.exec(QString("create table prevision_RI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, year integer not null, label text,"
                   "prevision_operators_to_add text, prevision_operators_to_substract text, computedPrevision integer,"
                       " unique(id_node,year) on conflict replace)").arg(uLastId).arg(tmpAudit.getAttachedTreeId())))
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

QString PCx_Prevision::generateHTMLHeader() const
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head><title>Prévision %1</title>\n<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>\n<style type='text/css'>\n%2\n</style>\n</head>\n<body>").arg(previsionName.toHtmlEscaped()).arg(PCx_Audit::getCSS());
}

QString PCx_Prevision::generateHTMLPrevisionTitle() const
{
    return QString("<h3>Prévision %1 portant sur l'audit %2 (%3)</h3>").arg(previsionName.toHtmlEscaped()).arg(attachedAudit->getAuditName().toHtmlEscaped()).arg(attachedAudit->getYearsString());
}

