#include "pcx_auditmanage.h"
#include "pcx_treemanage.h"
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
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un audit portant ce nom !"));
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

    PCx_Tree tree(attachedTreeId);
    QList<unsigned int> nodes=tree.getNodesId();
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

}
