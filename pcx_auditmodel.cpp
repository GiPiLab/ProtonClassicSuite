#include "pcx_auditmodel.h"
#include "utils.h"
#include <QMessageBox>

PCx_AuditModel::PCx_AuditModel(unsigned int auditId, QObject *parent) :
    QObject(parent)
{
    attachedTree=NULL;
    modelDF=NULL;
    modelDI=NULL;
    modelRI=NULL;
    modelRF=NULL;
    this->auditId=auditId;
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

QSqlTableModel *PCx_AuditModel::getTableModel(const QString &mode) const
{
    if(0==mode.compare("DF",Qt::CaseInsensitive))
    {
        return modelDF;
    }
    if(0==mode.compare("RF",Qt::CaseInsensitive))
    {
        return modelRF;
    }
    if(0==mode.compare("DI",Qt::CaseInsensitive))
    {
        return modelDI;
    }
    if(0==mode.compare("RI",Qt::CaseInsensitive))
    {
        return modelRI;
    }
    return NULL;
}

QSqlTableModel *PCx_AuditModel::getTableModel(DFRFDIRI mode) const
{
    switch(mode)
    {
        case DF:return modelDF;
        case RF:return modelRF;
        case DI:return modelDI;
        case RI:return modelRI;
    }
    return NULL;
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

bool PCx_AuditModel::propagateToAncestors(const QModelIndex &node)
{
    QSqlTableModel *model=(QSqlTableModel *)node.model();
    int row=node.row();
    unsigned int nodeId=model->index(row,COL_IDNODE).data().toUInt();
    unsigned int annee=model->index(row,COL_ANNEE).data().toUInt();
    QString tableName=model->tableName();
    qDebug()<<"Propagate from node "<<nodeId<<" in "<<annee<<" on "<<tableName;

    QSqlDatabase::database().transaction();
    if(updateParent(tableName,annee,nodeId))
    {
        QSqlDatabase::database().commit();
        return true;
    }
    else
    {
        qCritical()<<"ERROR DURING PROPAGATING VALUES TO ROOTS, CANCELLING";
        QSqlDatabase::database().rollback();
        return false;
    }

}

bool PCx_AuditModel::updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId)
{
    unsigned int parent=attachedTree->getParentId(nodeId);
    //qDebug()<<"Parent of "<<nodeId<<" = "<<parent;
    QList<unsigned int> listOfChildren=attachedTree->getChildren(parent);
    //qDebug()<<"Children of "<<nodeId<<" = "<<listOfChildren;
    QSqlQuery q;
    QStringList l;
    foreach (unsigned int childId, listOfChildren)
    {
        l.append(QString::number(childId));
    }

    q.exec(QString("select * from %1 where id_node in(%2) and annee=%3").arg(tableName).arg(l.join(',')).arg(annee));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    double sumOuverts=0.0;
    double sumRealises=0.0;
    double sumEngages=0.0;
    double sumDisponibles=0.0;

    //To check if we need to insert 0.0 or NULL
    bool nullOuverts=true;
    bool nullEngages=true;
    bool nullRealises=true;
    bool nullDisponibles=true;


    while(q.next())
    {
        //qDebug()<<"Node ID = "<<q.value("id_node")<< "Ouverts = "<<q.value("ouverts")<<" Realises = "<<q.value("realises")<<" Engages = "<<q.value("engages")<<" Disponibles = "<<q.value("disponibles");
        if(!q.value("ouverts").isNull())
        {
            sumOuverts+=q.value("ouverts").toDouble();
            nullOuverts=false;
        }

        if(!q.value("realises").isNull())
        {
            sumRealises+=q.value("realises").toDouble();
            nullRealises=false;
        }
        if(!q.value("engages").isNull())
        {
            sumEngages+=q.value("engages").toDouble();
            nullEngages=false;
        }
        if(!q.value("disponibles").isNull())
        {
            sumDisponibles+=q.value("disponibles").toDouble();
            nullDisponibles=false;
        }
    }

    q.prepare(QString("update %1 set ouverts=:ouverts,realises=:realises,engages=:engages,disponibles=:disponibles where annee=:annee and id_node=:id_node").arg(tableName));
    if(nullOuverts)
        q.bindValue(":ouverts",QVariant(QVariant::Double));
    else
        q.bindValue(":ouverts",sumOuverts);

    if(nullRealises)
        q.bindValue(":realises",QVariant(QVariant::Double));
    else
        q.bindValue(":realises",sumRealises);

    if(nullEngages)
        q.bindValue(":engages",QVariant(QVariant::Double));
    else
        q.bindValue(":engages",sumEngages);

    if(nullDisponibles)
        q.bindValue(":disponibles",QVariant(QVariant::Double));
    else
        q.bindValue(":disponibles",sumDisponibles);

    q.bindValue(":annee",annee);
    q.bindValue(":id_node",parent);
    q.exec();
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError().text();
        return false;
    }

    if(parent>1)
    {
        updateParent(tableName,annee,parent);
    }
    return true;
}

QString PCx_AuditModel::modetoTableString(DFRFDIRI mode) const
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
        qDebug()<<"Invalid mode specified !";
    }
    return QString();
}

QString PCx_AuditModel::modeToCompleteString(DFRFDIRI mode) const
{
    switch(mode)
    {
    case DF:
        return tr("Dépenses de fonctionnement");
    case RF:
        return tr("Recettes de fonctionnement");
    case DI:
        return tr("Dépenses d'investissement");
    case RI:
        return tr("Recettes d'investissement");
    default:
        qDebug()<<"Invalid mode specified !";
    }
    return QString();
}


//Warning, be called twice (see isDirty)
void PCx_AuditModel::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(bottomRight);
    QSqlTableModel *model=(QSqlTableModel *)topLeft.model();

   // qDebug()<<"Audit Data changed for model "<<model->tableName()<<": topleft column = "<<topLeft.column()<<" topleft row = "<<topLeft.row()<<"bottomRight column = "<<bottomRight.column()<<" bottomRight row = "<<bottomRight.row();

    //qDebug()<<"Model dirty : "<<model->isDirty();
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

    //Only propagate after the database has been updated (called for refreshing the view, see https://bugreports.qt-project.org/browse/QTBUG-30672
    if(!model->isDirty())
    {
        propagateToAncestors(topLeft);
    }

}


QList<QPair<unsigned int, QString> > PCx_AuditModel::getListOfAudits(ListAuditsMode mode)
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

QString PCx_AuditModel::getT1(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);

    QString output=QString("<table align='center' width='100%' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' colspan=8><b>%1 (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Pr&eacute;vu</th>"
                           "<th>R&eacute;alis&eacute;</th><th>%/pr&eacute;vu</th>"
                           "<th>Engag&eacute;</th><th>%/pr&eacute;vu</th><th>Disponible</th><th>%/pr&eacute;vu</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();

        double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

        if(ouverts!=0.0)
        {
            percentRealisesOuverts=realises*100/ouverts;
            percentEngagesOuverts=engages*100/ouverts;
            percentDisponiblesOuverts=disponibles*100/ouverts;
        }
        output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' class='t1valeur'>%2</td><td align='right' class='t1valeur'>%3</td>"
                      "<td align='right' class='t1pourcent'>%4\%</td><td align='right' class='t1valeur'>%5</td><td align='right' class='t1pourcent'>%6\%</td>"
                      "<td align='right' class='t1valeur'>%7</td><td align='right' class='t1pourcent'>%8\%</td></tr>").arg(q.value("annee").toUInt())
                      .arg(formatDouble(ouverts)).arg(formatDouble(realises)).arg(formatDouble(percentRealisesOuverts))
                      .arg(formatDouble(engages)).arg(formatDouble(percentEngagesOuverts))
                      .arg(formatDouble(disponibles)).arg(formatDouble(percentDisponiblesOuverts)));
    }

    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT4(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);
    QString output=QString("<table align='center' width='65%' cellpadding='5'>"
                           "<tr class='t4entete'><td colspan=3 align='center'><b>Poids relatif de [ %1 ]<br>au sein de la Collectivit&eacute; "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t4entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));

    QHash<unsigned int,double> ouvertsRoot,realisesRoot;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        ouvertsRoot.insert(q.value("annee").toUInt(),q.value("ouverts").toDouble());
        realisesRoot.insert(q.value("annee").toUInt(),q.value("realises").toDouble());
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        unsigned int annee=q.value("annee").toUInt();
        Q_ASSERT(annee>0);

        double percentOuvertsRoot=0.0,percentRealisesRoot=0.0;

        if(ouvertsRoot[annee]!=0.0)
        {
            percentOuvertsRoot=ouverts*100/ouvertsRoot[annee];
        }
        if(realisesRoot[annee]!=0.0)
        {
            percentRealisesRoot=realises*100/realisesRoot[annee];
        }

        output.append(QString("<tr><td class='t4annee'>%1</td><td align='right' class='t4pourcent'>%2\%</td><td align='right' class='t4pourcent'>"
                              "%3\%</td></tr>").arg(annee).arg(formatDouble(percentOuvertsRoot)).arg(formatDouble(percentRealisesRoot)));

    }

    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT8(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);
    QString output=QString("<table width='50%' align='center' cellpadding='5'>"
                           "<tr class='t8entete'><td colspan=2 align='center'><b>Moyennes budg&eacute;taires de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b></td></tr>")
                  .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));


    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    double sumOuverts=0.0,sumRealises=0.0,sumEngages=0.0,sumDisponibles=0.0;

    while(q.next())
    {
        sumOuverts+=q.value("ouverts").toDouble();
        sumRealises+=q.value("realises").toDouble();
        sumEngages+=q.value("engages").toDouble();
        sumDisponibles+=q.value("disponibles").toDouble();
    }

    double percentRealisesOuverts=0.0,percentEngagesOuverts=0.0,percentDisponiblesOuverts=0.0;

    if(sumOuverts!=0.0)
    {
        percentRealisesOuverts=sumRealises*100/sumOuverts;
        percentEngagesOuverts=sumEngages*100/sumOuverts;
        percentDisponiblesOuverts=sumDisponibles*100/sumOuverts;
    }

    output.append(QString("<tr><td class='t8annee' align='center'>R&eacute;alis&eacute;</td><td align='right' class='t8pourcent'>%1\%</td></tr>"
                          "<tr><td class='t8annee' align='center' style='font-weight:normal'>Non utilis&eacute;</td><td align='right' class='t8valeur'>%2\%</td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont engag&eacute;</i></td><td align='right' class='t8pourcent'><i>%3\%</i></td></tr>"
                          "<tr><td class='t8annee' align='center'><i>dont disponible</i></td><td align='right' class='t8pourcent'><i>%4\%</i></td></tr>")
                  .arg(formatDouble(percentRealisesOuverts)).arg(formatDouble(percentDisponiblesOuverts+percentEngagesOuverts))
                  .arg(formatDouble(percentEngagesOuverts)).arg(formatDouble(percentDisponiblesOuverts)));

    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT2(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);
    QString output=QString("<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>Evolution cumul&eacute;e du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de [ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));


    QMap<unsigned int,double> ouvertsRoot,realisesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        ouvertsRoot.insert(q.value("annee").toUInt(),ouverts);
        realisesRoot.insert(q.value("annee").toUInt(),realises);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0;
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            output.append(QString("<tr><td class='t2annee'>%1</td><td class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(diffFirstYearRootNodeOuverts!=0.0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/diffFirstYearRootNodeOuverts;
            }
            if(diffFirstYearRootNodeRealises!=0.0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT2bis(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);

    //The classes "t3xxx" are not a mistake
    QString output=QString("<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>Evolution du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de [ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));


    QMap<unsigned int,double> ouvertsRoot,realisesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        ouvertsRoot.insert(q.value("annee").toUInt(),ouverts);
        realisesRoot.insert(q.value("annee").toUInt(),realises);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0;
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            //ouverts_1 in PCA
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            output.append(QString("<tr><td class='t3annee'>%1</td><td class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            //ouverts_2
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffCurrentYearFirstYearOuverts=diffRootNodeOuverts-diffFirstYearRootNodeOuverts;
            diffCurrentYearFirstYearRealises=diffRootNodeRealises-diffFirstYearRootNodeRealises;
            if(diffFirstYearRootNodeOuverts!=0.0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/diffFirstYearRootNodeOuverts;
            }
            if(diffFirstYearRootNodeRealises!=0.0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/diffFirstYearRootNodeRealises;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

            //Here is the trick between T2 and T2bis, change the reference each year
            diffFirstYearRootNodeOuverts=diffRootNodeOuverts;
            diffFirstYearRootNodeRealises=diffRootNodeRealises;
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT3(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);

    //The classes "t2xxx" are not a mistake
    QString output=QString("<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t2entete'><td colspan=3 align='center'>"
                           "<b>Evolution cumul&eacute;e du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t2entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;

    while(q.next())
    {
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t2annee'>%1</td><td class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(firstYearOuvertsNode!=0.0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/firstYearOuvertsNode;
            }
            if(firstYearRealisesNode!=0.0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' class='t2pourcent'>%2\%</td><td align='right' class='t2pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

        }
    }
    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT5(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    //TODO : T5, verifier les resultats

    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);

    QString output=QString("<table width='80%' align='center' cellpadding='5'>"
                           "<tr class='t5entete'><td colspan=6 align='center'>"
                           "<b>Analyse en base 100 du compte administratif de la Collectivit&eacute; "
                           "<u>hors celui de<br>[ %1 ]</u> (<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t5entete'><th valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Pr&eacute;vu</th>"
                           "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr>"
                           "<tr class='t5entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th style='font-weight:normal'>dont<br>disponible</th></tr>")
                        .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));

    QMap<unsigned int,double> ouvertsRoot,realisesRoot,engagesRoot,disponiblesRoot;
    double firstYearOuvertsRoot=0.0,firstYearRealisesRoot=0.0,firstYearEngagesRoot=0.0,firstYearDisponiblesRoot=0.0;

    QSqlQuery q;
    q.exec(QString("select * from %1 where id_node=1 order by annee").arg(tableName));
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForRoot=false;
    while(q.next())
    {
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
        unsigned int annee=q.value("annee").toUInt();
        ouvertsRoot.insert(annee,ouverts);
        realisesRoot.insert(annee,realises);
        engagesRoot.insert(annee,engages);
        disponiblesRoot.insert(annee,disponibles);
        if(doneFirstForRoot==false)
        {
            firstYearOuvertsRoot=ouverts;
            firstYearRealisesRoot=realises;
            firstYearEngagesRoot=engages;
            firstYearDisponiblesRoot=disponibles;
            doneFirstForRoot=true;
        }
    }

    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0,firstYearEngagesNode=0.0,firstYearDisponiblesNode=0.0;
    double diffFirstYearRootNodeOuverts=0.0,diffFirstYearRootNodeRealises=0.0,diffFirstYearRootNodeEngages=0.0,
            diffFirstYearRootNodeDisponibles=0.0,diffFirstYearRootNodeNC=0.0;


    while(q.next())
    {
        double diffRootNodeOuverts=0.0,diffRootNodeRealises=0.0,diffRootNodeEngages=0.0,diffRootNodeDisponibles=0.0,diffRootNodeNC=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double engages=q.value("engages").toDouble();
        double disponibles=q.value("disponibles").toDouble();
        double percentOuverts=0.0,percentRealises=0.0,percentEngages=0.0,percentDisponibles=0.0,percentNC=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            firstYearEngagesNode=engages;
            firstYearDisponiblesNode=disponibles;

            //ouverts_1
            diffFirstYearRootNodeOuverts=firstYearOuvertsRoot-firstYearOuvertsNode;
            diffFirstYearRootNodeRealises=firstYearRealisesRoot-firstYearRealisesNode;
            diffFirstYearRootNodeEngages=firstYearEngagesRoot-firstYearEngagesNode;
            diffFirstYearRootNodeDisponibles=firstYearDisponiblesRoot-firstYearDisponiblesNode;
            diffFirstYearRootNodeNC=diffFirstYearRootNodeEngages+diffFirstYearRootNodeDisponibles;

            output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' class='t5pourcent'>100</td>"
                                  "<td align='right' class='t5pourcent'>100</td><td align='right' class='t5pourcent'>100</td>"
                                  "<td align='right' class='t5valeur'>100</td><td align='right' class='t5valeur'>100</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            //ouverts_2
            diffRootNodeOuverts=ouvertsRoot[annee]-ouverts;
            diffRootNodeRealises=realisesRoot[annee]-realises;
            diffRootNodeEngages=engagesRoot[annee]-engages;
            diffRootNodeDisponibles=disponiblesRoot[annee]-disponibles;
            diffRootNodeNC=diffRootNodeEngages+diffRootNodeDisponibles;

            if(diffFirstYearRootNodeOuverts*diffRootNodeOuverts!=0.0)
                percentOuverts=100/diffFirstYearRootNodeOuverts*diffRootNodeOuverts;

            if(diffFirstYearRootNodeRealises*diffRootNodeRealises!=0.0)
                percentRealises=100/diffFirstYearRootNodeRealises*diffRootNodeRealises;

            if(diffFirstYearRootNodeEngages*diffRootNodeEngages!=0.0)
                percentEngages=100/diffFirstYearRootNodeEngages*diffRootNodeEngages;

            if(diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles!=0.0)
                percentDisponibles=100/diffFirstYearRootNodeDisponibles*diffRootNodeDisponibles;

            if(diffRootNodeNC+diffFirstYearRootNodeNC!=0.0)
                percentNC=100/diffFirstYearRootNodeNC*diffRootNodeNC;


            output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' class='t5pourcent'>%2</td>"
                                  "<td align='right' class='t5pourcent'>%3</td><td align='right' class='t5pourcent'>%4</td>"
                                  "<td align='right' class='t5valeur'>%5</td><td align='right' class='t5valeur'>%6</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts,0)).arg(formatDouble(percentRealises,0)).arg(formatDouble(percentNC,0))
                          .arg(formatDouble(percentEngages,0)).arg(formatDouble(percentDisponibles,0)));
        }
    }
    output.append("</table>");
    return output;
}

QString PCx_AuditModel::getT6(unsigned int node, DFRFDIRI mode) const
{
    //TODO : T6
    QString out;
    return out;
}

QString PCx_AuditModel::getT7(unsigned int node, DFRFDIRI mode) const
{
    //TODO : T7
    QString out;
    return out;
}

QString PCx_AuditModel::getT9(unsigned int node, DFRFDIRI mode) const
{
    //TODO : T9
    QString out;
    return out;
}


QString PCx_AuditModel::getT3bis(unsigned int node, DFRFDIRI mode) const
{
    Q_ASSERT(node>0);
    QString tableName=QString("audit_%1_%2").arg(modetoTableString(mode)).arg(auditId);
    QString output=QString("<table width='70%' align='center' cellpadding='5'>"
                           "<tr class='t3entete'><td colspan=3 align='center'>"
                           "<b>Evolution du compte administratif de<br>[ %1 ] "
                           "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
                           "<tr class='t3entete'><th>Exercice</th><th>Pour le pr&eacute;vu</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
            .arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));

    QSqlQuery q;
    q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    bool doneFirstForNode=false;
    double firstYearOuvertsNode=0.0,firstYearRealisesNode=0.0;

    while(q.next())
    {
        double diffCurrentYearFirstYearOuverts=0.0,diffCurrentYearFirstYearRealises=0.0;

        unsigned int annee=q.value("annee").toUInt();
        double ouverts=q.value("ouverts").toDouble();
        double realises=q.value("realises").toDouble();
        double percentOuverts=0.0,percentRealises=0.0;
        if(doneFirstForNode==false)
        {
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;
            output.append(QString("<tr><td class='t3annee'>%1</td><td class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr>").arg(annee));
            doneFirstForNode=true;
        }
        else
        {
            diffCurrentYearFirstYearOuverts=ouverts-firstYearOuvertsNode;
            diffCurrentYearFirstYearRealises=realises-firstYearRealisesNode;
            if(firstYearOuvertsNode!=0.0)
            {
                percentOuverts=diffCurrentYearFirstYearOuverts*100/firstYearOuvertsNode;
            }
            if(firstYearRealisesNode!=0.0)
            {
                percentRealises=diffCurrentYearFirstYearRealises*100/firstYearRealisesNode;
            }
            output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' class='t3pourcent'>%2\%</td><td align='right' class='t3pourcent'>%3\%</td></tr>")
                          .arg(annee).arg(formatDouble(percentOuverts)).arg(formatDouble(percentRealises)));

            //Here is the only difference between T3 and T3bis, slide the reference
            firstYearOuvertsNode=ouverts;
            firstYearRealisesNode=realises;

        }
    }
    output.append("</table>");
    return output;
}



QString PCx_AuditModel::getCSS() const
{
    //Original PCA stylesheet with slight incoherencies between t2, t2bis, t3 and t3bis
    QString css=
            ".bg1{padding-left:5px;padding-right:5px;background-color:#ABC;}"
            ".bg2{padding-left:5px;padding-right:5px;background-color:#CAC;}"
            ".bg3{padding-left:5px;padding-right:5px;background-color:#CDB;}"
            ".req1{background-color:#FAB;padding:5px;}"
            ".req1normal{background-color:#FAB;}"
            ".req2{background-color:#BFA;padding:5px;}"
            ".req2normal{background-color:#BFA;}"
            ".req3{background-color:#BAF;padding:5px;}"
            ".req3normal{background-color:#BAF;}"
            "a:visited{font-weight:700;}"
            "dd.synop{margin:0;}"
            "dt.synop{font-weight:700;padding-top:1em;}"
            "h1{color:#A00;}"
            "h2{color:navy;}"
            "h3{color:green;font-size:larger;}"
            "li{line-height:30px;}"
            "table{color:navy;font-weight:400;font-size:8pt;page-break-inside:avoid;}"
            "td.t1annee,td.t3annee{background-color:#b3b3b3;}"
            "td.t1pourcent{background-color:#b3b3b3;color:#FFF;}"
            "td.t1valeur,td.t2valeur,td.t3valeur,td.t4annee,td.t4valeur,td.t4pourcent,td.t5annee,td.t7annee{background-color:#e6e6e6;}"
            "td.t2annee{background-color:#b3b3b3;color:green;}"
            "td.t2pourcent,td.t3pourcent{background-color:#e6e6e6;color:#000;}"
            "td.t5pourcent,td.t6pourcent{background-color:#b3b3b3;color:#000;}"
            "td.t5valeur,td.t6valeur,td.t7valeur{background-color:#b3b3b3;color:#000;font-weight:400;}"
            "td.t6annee{background-color:#e6e6e6;color:green;}"
            "td.t7pourcent{background-color:#666;color:#FFF;}"
            "td.t8pourcent{background-color:#e6e6e6;text-align:center;color:#000;}"
            "td.t8valeur{background-color:#e6e6e6;font-weight:400;text-align:center;color:#000;}"
            "td.t9pourcent{background-color:#666;text-align:center;color:#FFF;}"
            "td.t9valeur{background-color:#666;color:#FFF;font-weight:400;text-align:center;}"
            "tr.t1entete,tr.t3entete,td.t9annee{background-color:#b3b3b3;text-align:center;}"
            "tr.t2entete,td.t8annee{background-color:#b3b3b3;color:green;text-align:center;}"
            "tr.t4entete,tr.t5entete,tr.t7entete,tr.t9entete{background-color:#e6e6e6;text-align:center;}"
            "tr.t6entete{background-color:#e6e6e6;color:green;text-align:center;}"
            "tr.t8entete{background-color:#e6e6e6;text-align:center;color:green;}";

    return css;
}


QString PCx_AuditModel::getTabRecap(unsigned int node, DFRFDIRI mode) const
{
    QString out=getT1(node,mode)+"<br>"+getT4(node,mode)+"<br>"+getT8(node,mode)+"<br>";
    return out;
}

QString PCx_AuditModel::getTabEvolution(unsigned int node, DFRFDIRI mode) const
{
    QString out=getT2bis(node,mode)+"<br>"+getT3bis(node,mode)+"<br>";
    return out;
}



QString PCx_AuditModel::getTabEvolutionCumul(unsigned int node, DFRFDIRI mode) const
{
   QString out=getT2(node,mode)+"<br>"+getT3(node,mode)+"<br>";
   return out;
}


QString PCx_AuditModel::getTabBase100(unsigned int node, DFRFDIRI mode) const
{
    QString out=getT5(node,mode)+"<br>"+getT6(node,mode)+"<br>";
    return out;
}


QString PCx_AuditModel::getTabJoursAct(unsigned int node, DFRFDIRI mode) const
{
    QString out=getT7(node,mode)+"<br>"+getT9(node,mode)+"<br>";
    return out;
}




























