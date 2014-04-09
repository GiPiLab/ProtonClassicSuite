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
    q.prepare("insert into index_audits (nom,id_arbre,years) values (:nom,:id_arbre,:years)");
    q.bindValue(":nom",name);
    q.bindValue(":id_arbre",attachedTreeId);
    q.bindValue(":years",yearsString);
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
    q.exec(QString("delete from index_audits where id='%1'").arg(auditId));
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        die();
    }
    q.exec(QString("drop table audit_DF_%1").arg(auditId));
    q.exec(QString("drop table audit_RF_%1").arg(auditId));
    q.exec(QString("drop table audit_DI_%1").arg(auditId));
    q.exec(QString("drop table audit_RI_%1").arg(auditId));
    qDebug()<<"Audit "<<auditId<<" supprimé";
    return true;
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

