#include "pcx_auditmodel.h"
#include "utility.h"
#include <QMessageBox>

PCx_AuditModel::PCx_AuditModel(unsigned int auditId, QObject *parent) :
    QObject(parent)
{

}

PCx_AuditModel::~PCx_AuditModel()
{

}

bool PCx_AuditModel::addNewAudit(const QString &name, QSet<unsigned int> annees, unsigned int attachedTreeId)
{
    Q_ASSERT(!annees.isEmpty() && !name.isEmpty() && attachedTreeId>0);

    QList<unsigned int> anneesList=annees.toList();
    qSort(anneesList);

    QString anneesString;
    foreach(unsigned int annee,anneesList)
    {
        anneesString.append(QString::number(annee));
        anneesString.append(',');
    }
    anneesString.chop(1);

    qDebug()<<"Annees string = "<<anneesString;

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
    q.prepare("insert into index_audits (nom,id_arbre,annees) values (:nom,:id_arbre,:annees)");
    q.bindValue(":nom",name);
    q.bindValue(":id_arbre",attachedTreeId);
    q.bindValue(":annees",anneesString);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError().text();
        die();
    }

    QVariant lastId=q.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Problème d'id, vérifiez la consistance de la base";
        die();
    }

    q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real not null, realises real not null, engages real not null, disponibles real not null)").arg(lastId.toInt()));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        q.exec(QString("delete from index_audits where id=%1").arg(lastId.toInt()));
        die();
    }

    q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real not null, realises real not null, engages real not null, disponibles real not null)").arg(lastId.toInt()));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        q.exec(QString("delete from index_audits where id=%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_DF_%1").arg(lastId.toInt()));
        die();
    }


    q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real not null, realises real not null, engages real not null, disponibles real not null)").arg(lastId.toInt()));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        q.exec(QString("delete from index_audits where id=%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_DF_%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_RF_%1").arg(lastId.toInt()));
        die();
    }

    q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts real not null, realises real not null, engages real not null, disponibles real not null)").arg(lastId.toInt()));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        q.exec(QString("delete from index_audits where id=%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_DF_%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_RF_%1").arg(lastId.toInt()));
        q.exec(QString("drop table audit_DI_%1").arg(lastId.toInt()));
        die();
    }
    return true;

}

bool PCx_AuditModel::deleteAudit(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
}

QDateTime PCx_AuditModel::getCreationTime() const
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
}

void PCx_AuditModel::loadFromDb(unsigned int auditId)
{
    Q_ASSERT(auditId>0);

}


QHash<int, QString> PCx_AuditModel::getListOfAudits(bool finishedOnly)
{
    QHash<int,QString> listOfAudits;

    QSqlQuery query("select * from index_audits");
    while(query.next())
    {
        QString item;
        if(query.value(4).toBool()==true)
        {
            item=QString("%1 - %2 (audit terminé)").arg(query.value(1).toString()).arg(query.value(5).toString());
            listOfAudits.insert(query.value(0).toInt(),item);
        }
        else if(finishedOnly==false)
        {
             item=QString("%1 - %2").arg(query.value(1).toString()).arg(query.value(5).toString());
             listOfAudits.insert(query.value(0).toInt(),item);
        }
    }
    return listOfAudits;
}

QHash<QString, QVariant> PCx_AuditModel::getAuditInfos(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    QHash<QString,QVariant> infos;
    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        infos.insert("id",q.value(0));
        infos.insert("nom",q.value(1));
        infos.insert("nom_arbre",QVariant(PCx_TreeModel::idTreeToName(q.value(2).toUInt())));
        infos.insert("annees",q.value(3));
        infos.insert("termine",q.value(4));
        QDateTime dt(QDateTime::fromString(q.value(5).toString(),"yyyy-MM-dd hh:mm:ss"));
        infos.insert("date_creation",QVariant(dt));

    }
    return infos;
}




