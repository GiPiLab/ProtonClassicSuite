#include "pcx_auditmodel.h"
#include "utility.h"
#include <QMessageBox>

PCx_AuditModel::PCx_AuditModel(unsigned int auditId, QObject *parent) :
    QObject(parent)
{
    attachedTree=NULL;
    modelDF=NULL;
    modelDI=NULL;
    modelRI=NULL;
    modelRF=NULL;
    loadFromDb(auditId);
}

PCx_AuditModel::~PCx_AuditModel()
{
    if(modelDF!=NULL)
    {
        modelDF->clear();
        delete modelDF;
    }
    if(modelRF!=NULL)
    {
        modelRF->clear();
        delete modelRF;
    }
    if(modelDI!=NULL)
    {
        modelDI->clear();
        delete modelDI;
    }
    if(modelRI!=NULL)
    {
        modelRI->clear();
        delete modelRI;
    }
    delete attachedTree;
}

bool PCx_AuditModel::addNewAudit(const QString &name, QSet<unsigned int> years, unsigned int attachedTreeId)
{
    Q_ASSERT(!years.isEmpty() && !name.isEmpty() && attachedTreeId>0);

    QList<unsigned int> yearsList=years.toList();
    qSort(yearsList);

    QString yearsString;
    foreach(unsigned int annee,yearsList)
    {
        yearsString.append(QString::number(annee));
        yearsString.append(',');
    }
    yearsString.chop(1);

    qDebug()<<"years string = "<<yearsString;

    QSqlQuery q;
    q.prepare("select count(*) from index_audits where nom=:nom");
    q.bindValue(":nom",name);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un audit portant ce nom !"));
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
        die();
    }

    q.exec(QString("select count(*) from index_arbres where id='%1'").arg(attachedTreeId));
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Ajout audit sur arbre inexistant !";
            die();
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
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
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    QVariant lastId=q.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Problème d'id, vérifiez la consistance de la base";
        QSqlDatabase::database().rollback();
        die();
    }
    unsigned int uLastId=lastId.toUInt();

    q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real, realises real, engages real, disponibles real)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        //q.exec(QString("delete from index_audits where id=%1").arg(uLastId));
        die();
    }

    q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real, realises real, engages real, disponibles real)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        //q.exec(QString("delete from index_audits where id=%1").arg(uLastId));
        //q.exec(QString("drop table audit_DF_%1").arg(uLastId));
        die();
    }

    q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real, realises real, engages real, disponibles real)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        //q.exec(QString("delete from index_audits where id=%1").arg(uLastId));
        //q.exec(QString("drop table audit_DF_%1").arg(uLastId));
        //q.exec(QString("drop table audit_RF_%1").arg(uLastId));
        die();
    }

    q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real, realises real, engages real, disponibles real)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        //q.exec(QString("delete from index_audits where id=%1").arg(uLastId));
        //q.exec(QString("drop table audit_DF_%1").arg(uLastId));
        //q.exec(QString("drop table audit_RF_%1").arg(uLastId));
        //q.exec(QString("drop table audit_DI_%1").arg(uLastId));
        die();
    }

    //Populate tables with years for each node of the attached tree

    QList<unsigned int> nodes=PCx_TreeModel::getNodesId(attachedTreeId);
    qDebug()<<"Nodes ids = "<<nodes;

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
                qCritical()<<q.lastError().text();
                QSqlDatabase::database().rollback();
                die();
            }

            q.prepare(QString("insert into audit_RF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError().text();
                QSqlDatabase::database().rollback();
                die();
            }

            q.prepare(QString("insert into audit_DI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError().text();
                QSqlDatabase::database().rollback();
                die();
            }

            q.prepare(QString("insert into audit_RI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            q.exec();
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError().text();
                QSqlDatabase::database().rollback();
                die();
            }
        }
    }
    QSqlDatabase::database().commit();

    return true;
}

bool PCx_AuditModel::deleteAudit(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q(QString("select count(*) from index_audits where id='%1'").arg(auditId));
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Audit inexistant !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
        die();
    }

    QSqlDatabase::database().transaction();
    q.exec(QString("delete from index_audits where id='%1'").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    q.exec(QString("drop table audit_DF_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    q.exec(QString("drop table audit_RF_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    q.exec(QString("drop table audit_DI_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    q.exec(QString("drop table audit_RI_%1").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    QSqlDatabase::database().commit();
    qDebug()<<"Audit "<<auditId<<" supprimé";
    return true;
}

QDateTime PCx_AuditModel::getCreationTime() const
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
}

bool PCx_AuditModel::loadFromDb(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;
    q.prepare("select * from index_audits where id=:id limit 1");
    q.bindValue(":id",auditId);
    q.exec();
    if(q.next())
    {
        unsigned int attachedTreeId=q.value("id_arbre").toUInt();
        qDebug()<<"Attached tree ID = "<<attachedTreeId;
        if(attachedTree!=NULL)
        {
            delete attachedTree;
        }
        attachedTree=new PCx_TreeModel(attachedTreeId);
        if(modelDF!=NULL)
        {
            modelDF->clear();
            delete modelDF;
        }
        if(modelRF!=NULL)
        {
            modelRF->clear();
            delete modelRF;
        }
        if(modelDI!=NULL)
        {
            modelDI->clear();
            delete modelDI;
        }
        if(modelRI!=NULL)
        {
            modelRI->clear();
            delete modelRI;
        }
        modelDF=new QSqlTableModel();
        modelDF->setTable(QString("audit_DF_%1").arg(auditId));
        modelDF->setEditStrategy(QSqlTableModel::OnManualSubmit);
        modelDF->select();

        modelDI=new QSqlTableModel();
        modelDI->setTable(QString("audit_DI_%1").arg(auditId));
        modelDI->setEditStrategy(QSqlTableModel::OnManualSubmit);
        modelDI->select();

        modelRI=new QSqlTableModel();
        modelRI->setTable(QString("audit_RI_%1").arg(auditId));
        modelRI->setEditStrategy(QSqlTableModel::OnManualSubmit);
        modelRI->select();

        modelRF=new QSqlTableModel();
        modelRF->setTable(QString("audit_RF_%1").arg(auditId));
        modelRF->setEditStrategy(QSqlTableModel::OnManualSubmit);
        modelRF->select();
    }
    else
    {
        qCritical()<<"Invalid audit ID";
        die();
    }
    return true;
}


QHash<int, QString> PCx_AuditModel::getListOfAudits(bool finishedOnly)
{
    QHash<int,QString> listOfAudits;
    QDateTime dt;

    QSqlQuery query("select * from index_audits");

    while(query.next())
    {
        QString item;
        dt=QDateTime::fromString(query.value(5).toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();
        if(query.value(4).toBool()==true)
        {
            item=QString("%1 - %2 (audit terminé)").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            listOfAudits.insert(query.value(0).toInt(),item);
        }
        else if(finishedOnly==false)
        {
             item=QString("%1 - %2").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
             listOfAudits.insert(query.value(0).toInt(),item);
        }
    }
    return listOfAudits;
}

