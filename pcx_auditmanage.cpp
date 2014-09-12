#include "pcx_auditmanage.h"
#include <QSet>
#include <QMessageBox>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include "utils.h"
#include "pcx_audit.h"
#include "pcx_query.h"

namespace PCx_AuditManage{


unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId)
{
    Q_ASSERT(!years.isEmpty() && !name.isEmpty() && attachedTreeId>0);

    //Removes duplicates and sort years
    QSet<unsigned int> yearsSet=years.toSet();
    QList<unsigned int> yearsList=yearsSet.toList();
    qSort(yearsList);

    QString yearsString;
    foreach(unsigned int annee,yearsList)
    {
        yearsString.append(QString::number(annee));
        yearsString.append(',');
    }
    yearsString.chop(1);

    //qDebug()<<"years string = "<<yearsString;

    if(auditNameExists(name))
    {
        QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un audit portant ce nom !"));
        return 0;
    }

    QSqlQuery q;

    q.exec(QString("select count(*) from index_arbres where id='%1'").arg(attachedTreeId));
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Inexistant tree !";
            die();
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().transaction();
    q.prepare("insert into index_audits (nom,id_arbre,annees) values (:nom,:id_arbre,:annees)");
    q.bindValue(":nom",name);
    q.bindValue(":id_arbre",attachedTreeId);
    q.bindValue(":annees",yearsString);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    QVariant lastId=q.lastInsertId();
    if(!lastId.isValid())
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    unsigned int uLastId=lastId.toUInt();

    //Data are integer for fixed points arithmetics, stored with 3 decimals
    q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("create index idx_idnode_audit_DF_%1 on audit_DF_%1(id_node)").arg(uLastId));

    q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("create index idx_idnode_audit_RF_%1 on audit_RF_%1(id_node)").arg(uLastId));

    q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("create index idx_idnode_audit_DI_%1 on audit_DI_%1(id_node)").arg(uLastId));

    q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("create index idx_idnode_audit_RI_%1 on audit_RI_%1(id_node)").arg(uLastId));

    bool res=PCx_Query::createTableQueries(uLastId);

    if(res==false)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    //Populate tables with years for each node of the attached tree

    QList<unsigned int> nodes=PCx_TreeModel::getNodesId(attachedTreeId);
    //qDebug()<<"Nodes ids = "<<nodes;

    foreach(unsigned int node,nodes)
    {
        foreach(unsigned int annee,yearsList)
        {
            q.prepare(QString("insert into audit_DF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                QSqlDatabase::database().rollback();
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_RF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                QSqlDatabase::database().rollback();
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_DI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                QSqlDatabase::database().rollback();
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_RI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                QSqlDatabase::database().rollback();
                qCritical()<<q.lastError();
                die();
            }
        }
    }
    QSqlDatabase::database().commit();

    return uLastId;
}

bool deleteAudit(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q(QString("select count(*) from index_audits where id='%1'").arg(auditId));
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Attempting to delete an inexistant audit !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().transaction();
    q.exec(QString("delete from index_audits where id='%1'").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("drop table audit_DF_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("drop table audit_RF_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("drop table audit_DI_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("drop table audit_RI_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("drop table audit_queries_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().commit();
    qDebug()<<"Audit "<<auditId<<" deleted";
    return true;
}

int duplicateAudit(unsigned int auditId, const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI)
{
    Q_ASSERT(!newName.isEmpty());
    if(auditNameExists(newName))
    {
        qWarning()<<"Audit name exists !";
        return -1;
    }

    QStringList modes;

    if(copyDF)
        modes.append(modeToTableString(DF));
    if(copyRF)
        modes.append(modeToTableString(RF));
    if(copyDI)
        modes.append(modeToTableString(DI));
    if(copyRI)
        modes.append(modeToTableString(RI));


    QProgressDialog progress(QObject::tr("Données en cours de recopie..."),QObject::tr("Annuler"),0,modes.size()*4+2);
    progress.setMinimumDuration(600);
    progress.setCancelButton(0);

    progress.setWindowModality(Qt::ApplicationModal);

    int progval=0;

    progress.setValue(0);

    //A transaction is used in addNewAudit
    unsigned int newAuditId=addNewAudit(newName,years,getAttachedTreeId(auditId));
    if(newAuditId==0)
    {
        qCritical()<<"Unable to duplicate audit !";
        die();
    }

    qSort(years);
    unsigned int year1=years.first();
    unsigned int year2=years.last();

    QSqlDatabase::database().transaction();

    QSqlQuery q;

    progress.setValue(++progval);


    foreach(QString lemode,modes)
    {
        q.prepare(QString("update audit_%3_%1 set ouverts="
                          "(select ouverts from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newAuditId).arg(auditId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        q.exec();
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set realises="
                          "(select realises from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newAuditId).arg(auditId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        q.exec();
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set engages="
                          "(select engages from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newAuditId).arg(auditId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        q.exec();
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }


        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set disponibles="
                          "(select disponibles from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newAuditId).arg(auditId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        q.exec();
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            return -1;
        }

        progress.setValue(++progval);
    }

    progress.setValue(progress.maximum());
    q.exec(QString("insert into audit_queries_%1 select * from audit_queries_%2").arg(newAuditId).arg(auditId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        deleteAudit(newAuditId);
        qCritical()<<q.lastError();
        return -1;
    }

    QSqlDatabase::database().commit();
    return newAuditId;
}

bool auditNameExists(const QString &auditName)
{
    QSqlQuery q;
    q.prepare("select count(*) from index_audits where nom=:name");
    q.bindValue(":name",auditName);
    q.exec();
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.value(0).toLongLong()>0)
        return true;
    return false;
}

QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode)
{
    QList<QPair<unsigned int,QString> > listOfAudits;
    QDateTime dt;

    QSqlQuery query("select * from index_audits order by datetime(le_timestamp)");

    while(query.next())
    {
        QString item;
        dt=QDateTime::fromString(query.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();
        QPair<unsigned int, QString> p;
        if(query.value("termine").toBool()==true)
        {
            //Finished audit
            item=QString("%1 - %2").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            if(mode!=UnFinishedAuditsOnly)
            {
                p.first=query.value("id").toUInt();
                p.second=item;
                listOfAudits.append(p);
            }
        }
        else if(mode!=FinishedAuditsOnly)
        {
            //Unfinished audit
            item=QString("%1 - %2").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            p.first=query.value("id").toUInt();
            p.second=item;
            listOfAudits.append(p);
        }
    }
    return listOfAudits;
}

unsigned int getAttachedTreeId(unsigned int auditId)
{
    QSqlQuery q(QString("select id_arbre from index_audits where id=%1").arg(auditId));
    if(!q.next())
    {
        qCritical()<<"Invalid audit ID !";
        return 0;
    }
    return q.value(0).toUInt();
}

bool finishAudit(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;
    q.prepare("select count(*) from index_audits where id=:id");
    q.bindValue(":id",auditId);
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Trying to finish a missing audit !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }
    q.prepare("update index_audits set termine=1 where id=:id");
    q.bindValue(":id",auditId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
    return true;
}

bool unFinishAudit(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;
    q.prepare("select count(*) from index_audits where id=:id");
    q.bindValue(":id",auditId);
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Trying to un-finish a missing audit !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }
    q.prepare("update index_audits set termine=0 where id=:id");
    q.bindValue(":id",auditId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
    return true;
}

QString modeToTableString(DFRFDIRI mode)
{
    switch(mode)
    {
    case DF:
        return "DF";
    case RF:
        return "RF";
    case DI:
        return "DI";
    case RI:
        return "RI";
    default:
        qWarning()<<"Invalid mode specified !";
    }
    return QString();
}



QString modeToCompleteString(DFRFDIRI mode)
{
    switch(mode)
    {
    case DF:
        return QObject::tr("Dépenses de fonctionnement");
    case RF:
        return QObject::tr("Recettes de fonctionnement");
    case DI:
        return QObject::tr("Dépenses d'investissement");
    case RI:
        return QObject::tr("Recettes d'investissement");
    default:
        qWarning()<<"Invalid mode specified !";
    }
    return QString();
}


QString OREDtoTableString(ORED ored)
{
    switch(ored)
    {
    case OUVERTS:
        return "ouverts";
    case REALISES:
        return "realises";
    case ENGAGES:
        return "engages";
    case DISPONIBLES:
        return "disponibles";
    default:
        qWarning()<<"Invalid ORED specified !";
    }
    return QString();
}

ORED OREDFromTableString(const QString &ored)
{
    if(ored==OREDtoTableString(OUVERTS))
        return OUVERTS;
    if(ored==OREDtoTableString(REALISES))
        return REALISES;
    if(ored==OREDtoTableString(ENGAGES))
        return ENGAGES;
    if(ored==OREDtoTableString(DISPONIBLES))
        return DISPONIBLES;

    qWarning()<<"Invalid ORED string specified, defaulting to ouverts";
    return OUVERTS;
}

DFRFDIRI modeFromTableString(const QString &mode)
{
    if(mode==modeToTableString(DF))
        return DF;
    if(mode==modeToTableString(RF))
        return RF;
    if(mode==modeToTableString(DI))
        return DI;
    if(mode==modeToTableString(RI))
        return RI;

    qWarning()<<"Invalid DFRFDIRI string specified, defaulting to DF";
    return DF;
}



QString OREDtoCompleteString(ORED ored)
{
    switch(ored)
    {
    case OUVERTS:
        return QObject::tr("prévu");
    case REALISES:
        return QObject::tr("réalisé");
    case ENGAGES:
        return QObject::tr("engagé");
    case DISPONIBLES:
        return QObject::tr("disponible");
    default:
        qWarning()<<"Invalid ORED specified !";
    }
    return QString();
}



}
