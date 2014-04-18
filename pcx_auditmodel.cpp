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

bool PCx_AuditModel::finishAudit()
{
    finished=true;
    return PCx_AuditModel::finishAudit(auditId);
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
        modelDF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
        modelDF->setHeaderData(COL_OUVERTS,Qt::Horizontal,tr("Ouverts"));
        modelDF->setHeaderData(COL_REALISES,Qt::Horizontal,tr("Réalisés"));
        modelDF->setHeaderData(COL_ENGAGES,Qt::Horizontal,tr("Engagés"));
        modelDF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,tr("Disponibles"));
        modelDF->setEditStrategy(QSqlTableModel::OnFieldChange);
        modelDF->select();

        modelDI=new QSqlTableModel();
        modelDI->setTable(QString("audit_DI_%1").arg(auditId));
        modelDI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
        modelDI->setHeaderData(COL_OUVERTS,Qt::Horizontal,tr("Ouverts"));
        modelDI->setHeaderData(COL_REALISES,Qt::Horizontal,tr("Réalisés"));
        modelDI->setHeaderData(COL_ENGAGES,Qt::Horizontal,tr("Engagés"));
        modelDI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,tr("Disponibles"));
        modelDI->setEditStrategy(QSqlTableModel::OnFieldChange);
        modelDI->select();

        modelRI=new QSqlTableModel();
        modelRI->setTable(QString("audit_RI_%1").arg(auditId));
        modelRI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
        modelRI->setHeaderData(COL_OUVERTS,Qt::Horizontal,tr("Ouverts"));
        modelRI->setHeaderData(COL_REALISES,Qt::Horizontal,tr("Réalisés"));
        modelRI->setHeaderData(COL_ENGAGES,Qt::Horizontal,tr("Engagés"));
        modelRI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,tr("Disponibles"));
        modelRI->setEditStrategy(QSqlTableModel::OnFieldChange);
        modelRI->select();

        modelRF=new QSqlTableModel();
        modelRF->setTable(QString("audit_RF_%1").arg(auditId));
        modelRF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
        modelRF->setHeaderData(COL_OUVERTS,Qt::Horizontal,tr("Ouverts"));
        modelRF->setHeaderData(COL_REALISES,Qt::Horizontal,tr("Réalisés"));
        modelRF->setHeaderData(COL_ENGAGES,Qt::Horizontal,tr("Engagés"));
        modelRF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,tr("Disponibles"));
        modelRF->setEditStrategy(QSqlTableModel::OnFieldChange);
        modelRF->select();

        connect(modelDF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
        connect(modelRF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
        connect(modelDI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
        connect(modelRI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
    }
    else
    {
        qCritical()<<"Invalid audit ID";
        die();
    }
    return true;
}


void PCx_AuditModel::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(bottomRight);
    QSqlTableModel *model=(QSqlTableModel *)topLeft.model();

   // qDebug()<<"Audit Data changed for model "<<model->tableName()<<": topleft column = "<<topLeft.column()<<" topleft row = "<<topLeft.row()<<"bottomRight column = "<<bottomRight.column()<<" bottomRight row = "<<bottomRight.row();

    int row=topLeft.row();

    QVariant vOuverts=model->index(row,COL_OUVERTS).data();
    QVariant vRealises=model->index(row,COL_REALISES).data();
    QVariant vEngages=model->index(row,COL_ENGAGES).data();

    double ouverts=vOuverts.toDouble();
    double realises=vRealises.toDouble();
    double engages=vEngages.toDouble();

    if(!vRealises.isNull() && !vOuverts.isNull() && !vEngages.isNull())
    {
        QVariant disponibles=ouverts-(realises+engages);
       // qDebug()<<disponibles;
        QModelIndex indexDispo=model->index(row,COL_DISPONIBLES);
        model->setData(indexDispo,disponibles);
    }
}


QHash<int, QString> PCx_AuditModel::getListOfAudits(ListAuditsMode mode)
{
    QHash<int,QString> listOfAudits;
    QDateTime dt;

    QSqlQuery query("select * from index_audits");

    while(query.next())
    {
        QString item;
        dt=QDateTime::fromString(query.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();
        if(query.value("termine").toBool()==true)
        {
            item=QString("%1 - %2 (audit terminé)").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            if(mode!=UnFinishedAuditsOnly)
                listOfAudits.insert(query.value("id").toInt(),item);
        }
        else if(mode!=FinishedAuditsOnly)
        {
             item=QString("%1 - %2").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
             listOfAudits.insert(query.value("id").toInt(),item);
        }
    }
    return listOfAudits;
}

bool PCx_AuditModel::finishAudit(unsigned int id)
{
    Q_ASSERT(id>0);
    QSqlQuery q;
    q.prepare("select count(*) from index_audits where id=:id");
    q.bindValue(":id",id);
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Terminer Audit inexistant !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
        die();
    }
    q.prepare("update index_audits set termine=1 where id=:id");
    q.bindValue(":id",id);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError().text();
        die();
    }
    return true;
}

