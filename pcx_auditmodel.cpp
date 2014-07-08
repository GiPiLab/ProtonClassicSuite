#include "pcx_auditmodel.h"
#include "pcx_query.h"
#include "utils.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QDebug>

PCx_AuditModel::PCx_AuditModel(unsigned int auditId, QObject *parent, bool readOnly) :
    QObject(parent)
{
    attachedTree=NULL;
    modelDF=NULL;
    modelDI=NULL;
    modelRI=NULL;
    modelRF=NULL;
    this->auditId=auditId;
    loadFromDb(auditId,readOnly);
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

bool PCx_AuditModel::addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId)
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

    //Data are integer for fixed points arithmetics, stored with 3 decimals
    q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    bool res=PCx_Query::createTableQueries(uLastId);

    if(res==false)
    {
        QSqlDatabase::database().rollback();
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
    qDebug()<<"Audit "<<auditId<<" deleted";
    return true;
}



bool PCx_AuditModel::finishAudit()
{
    auditInfos.finished=true;
    return PCx_AuditModel::finishAudit(auditId);
}

bool PCx_AuditModel::unFinishAudit()
{
    auditInfos.finished=false;
    return PCx_AuditModel::unFinishAudit(auditId);
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
    case GLOBAL:return NULL;
    }
    return NULL;
}

bool PCx_AuditModel::setLeafValues(unsigned int leafId, PCx_AuditModel::DFRFDIRI mode, unsigned int year, QHash<ORED,double> vals)
{
    Q_ASSERT(!vals.isEmpty());
    if(vals.contains(DISPONIBLES))
    {
        qWarning()<<"Not modifying computed column DISPONIBLE";
        return false;
    }
    if(auditInfos.finished)
    {
        qWarning()<<"Not modifying finished audit";
        return false;
    }
    if(year<auditInfos.years.first() || year>auditInfos.years.last())
    {
        qCritical()<<"Invalid year in setLeaf";
        return false;
    }
    if(!attachedTree->isLeaf(leafId))
    {
        qWarning()<<"Not a leaf, abording";
        return false;
    }

    QString reqString;

    if(vals.contains(OUVERTS))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append(OREDtoTableString(OUVERTS)+"=:vouverts");
    }
    if(vals.contains(REALISES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append(OREDtoTableString(REALISES)+"=:vrealises");
    }
    if(vals.contains(ENGAGES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append(OREDtoTableString(ENGAGES)+"=:vengages");
    }

    QSqlDatabase::database().transaction();

    QSqlQuery q;
    QString tableName=QString("audit_%1_%2").arg(modeToTableString(mode)).arg(auditId);

    //qDebug()<<"Request String ="<<reqString;

    q.prepare(QString("update %1 set %2 where id_node=:id_node and annee=:year")
              .arg(tableName).arg(reqString));

    if(vals.contains(OUVERTS))
        q.bindValue(":vouverts",(qint64)vals.value(OUVERTS)*FIXEDPOINTCOEFF);

    if(vals.contains(REALISES))
        q.bindValue(":vrealises",(qint64)vals.value(REALISES)*FIXEDPOINTCOEFF);

    if(vals.contains(ENGAGES))
        q.bindValue(":vengages",(qint64)vals.value(ENGAGES)*FIXEDPOINTCOEFF);

    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<"Error while setting value"<<q.lastError();
        QSqlDatabase::database().rollback();
        return false;
    }

    q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) where id_node=:id_node and annee=:year").arg(tableName));
    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<"Error while setting disponibles values"<<q.lastError();
        QSqlDatabase::database().rollback();
        return false;
    }

    if(updateParent(tableName,year,leafId))
    {
        QSqlDatabase::database().commit();
    }
    else
    {
        qCritical()<<"ERROR DURING PROPAGATING VALUES TO ROOTS, CANCELLING";
        QSqlDatabase::database().rollback();
        return false;
    }

    QSqlTableModel *tblModel=getTableModel(mode);
    if(tblModel!=NULL)
        tblModel->select();

    return true;
}

qint64 PCx_AuditModel::getNodeValue(unsigned int nodeId, PCx_AuditModel::DFRFDIRI mode, PCx_AuditModel::ORED ored, unsigned int year) const
{
    if(year<auditInfos.years.first() || year>auditInfos.years.last())
    {
        qCritical()<<"Invalid year specified";
        return -MAX_NUM;
    }
    QSqlQuery q;
    q.prepare(QString("select %1 from audit_%2_%3 where annee=:year and id_node=:node").arg(OREDtoTableString(ored)).arg(modeToTableString(mode)).arg(auditId));
    q.bindValue(":year",year);
    q.bindValue(":node",nodeId);
    q.exec();
    if(!q.next())
    {
        qCritical()<<q.lastError();
        return false;
    }
    if(q.value(OREDtoTableString(ored)).isNull())
    {
        qDebug()<<"NULL value";
        return -MAX_NUM;
    }
    return q.value(OREDtoTableString(ored)).toLongLong();
}

bool PCx_AuditModel::clearAllData(PCx_AuditModel::DFRFDIRI mode)
{
    QSqlQuery q(QString("update audit_%1_%2 set ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL").arg(modeToTableString(mode)).arg(auditId));
    q.exec();
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        return false;
    }
    QSqlTableModel *tblModel=getTableModel(mode);
    if(tblModel!=NULL)
        tblModel->select();

    return true;
}

bool PCx_AuditModel::loadFromDb(unsigned int auditId,bool readOnly)
{
    Q_ASSERT(auditId>0);
    QSqlQuery q;
    auditInfos.updateInfos(auditId);
    if(auditInfos.valid)
    {
        unsigned int attachedTreeId=auditInfos.attachedTreeId;
        qDebug()<<"Attached tree ID = "<<attachedTreeId;

        if(attachedTree!=NULL)
        {
            delete attachedTree;
        }
        attachedTree=new PCx_TreeModel(attachedTreeId);

        if(readOnly==false)
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
            modelDF=new QSqlTableModel();
            modelDF->setTable(QString("audit_DF_%1").arg(auditId));
            modelDF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
            modelDF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
            modelDF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
            modelDF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
            modelDF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
            modelDF->setEditStrategy(QSqlTableModel::OnFieldChange);
            modelDF->select();

            modelDI=new QSqlTableModel();
            modelDI->setTable(QString("audit_DI_%1").arg(auditId));
            modelDI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
            modelDI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
            modelDI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
            modelDI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
            modelDI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
            modelDI->setEditStrategy(QSqlTableModel::OnFieldChange);
            modelDI->select();

            modelRI=new QSqlTableModel();
            modelRI->setTable(QString("audit_RI_%1").arg(auditId));
            modelRI->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
            modelRI->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
            modelRI->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
            modelRI->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
            modelRI->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
            modelRI->setEditStrategy(QSqlTableModel::OnFieldChange);
            modelRI->select();

            modelRF=new QSqlTableModel();
            modelRF->setTable(QString("audit_RF_%1").arg(auditId));
            modelRF->setHeaderData(COL_ANNEE,Qt::Horizontal,"");
            modelRF->setHeaderData(COL_OUVERTS,Qt::Horizontal,OREDtoCompleteString(OUVERTS));
            modelRF->setHeaderData(COL_REALISES,Qt::Horizontal,OREDtoCompleteString(REALISES));
            modelRF->setHeaderData(COL_ENGAGES,Qt::Horizontal,OREDtoCompleteString(ENGAGES));
            modelRF->setHeaderData(COL_DISPONIBLES,Qt::Horizontal,OREDtoCompleteString(DISPONIBLES));
            modelRF->setEditStrategy(QSqlTableModel::OnFieldChange);
            modelRF->select();

            connect(modelDF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
            connect(modelRF,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
            connect(modelDI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
            connect(modelRI,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onModelDataChanged(const QModelIndex &, const QModelIndex &)));
        }
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

    qint64 sumOuverts=0;
    qint64 sumRealises=0;
    qint64 sumEngages=0;
    qint64 sumDisponibles=0;

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
            sumOuverts+=q.value("ouverts").toLongLong();
            nullOuverts=false;
        }

        if(!q.value("realises").isNull())
        {
            sumRealises+=q.value("realises").toLongLong();
            nullRealises=false;
        }
        if(!q.value("engages").isNull())
        {
            sumEngages+=q.value("engages").toLongLong();
            nullEngages=false;
        }
        if(!q.value("disponibles").isNull())
        {
            sumDisponibles+=q.value("disponibles").toLongLong();
            nullDisponibles=false;
        }
    }

    q.prepare(QString("update %1 set ouverts=:ouverts,realises=:realises,engages=:engages,disponibles=:disponibles where annee=:annee and id_node=:id_node").arg(tableName));
    if(nullOuverts)
        q.bindValue(":ouverts",QVariant(QVariant::LongLong));
    else
        q.bindValue(":ouverts",sumOuverts);

    if(nullRealises)
        q.bindValue(":realises",QVariant(QVariant::LongLong));
    else
        q.bindValue(":realises",sumRealises);

    if(nullEngages)
        q.bindValue(":engages",QVariant(QVariant::LongLong));
    else
        q.bindValue(":engages",sumEngages);

    if(nullDisponibles)
        q.bindValue(":disponibles",QVariant(QVariant::LongLong));
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

QString PCx_AuditModel::modeToTableString(DFRFDIRI mode)
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
        qCritical()<<"Invalid mode specified !";
    }
    return QString();
}



QString PCx_AuditModel::modeToCompleteString(DFRFDIRI mode)
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
        qCritical()<<"Invalid mode specified !";
    }
    return QString();
}

QString PCx_AuditModel::OREDtoTableString(ORED ored)
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
        qCritical()<<"Invalid ORED specified !";
    }
    return QString();
}

PCx_AuditModel::ORED PCx_AuditModel::OREDFromTableString(const QString &ored)
{
    if(ored==OREDtoTableString(OUVERTS))
        return OUVERTS;
    if(ored==OREDtoTableString(REALISES))
        return REALISES;
    if(ored==OREDtoTableString(ENGAGES))
        return ENGAGES;
    if(ored==OREDtoTableString(DISPONIBLES))
        return DISPONIBLES;

    qCritical()<<"Invalid ORED string specified, defaulting to ouverts";
    return OUVERTS;
}

PCx_AuditModel::DFRFDIRI PCx_AuditModel::modeFromTableString(const QString &mode)
{
    if(mode==modeToTableString(DF))
        return DF;
    if(mode==modeToTableString(RF))
        return RF;
    if(mode==modeToTableString(DI))
        return DI;
    if(mode==modeToTableString(RI))
        return RI;

    qCritical()<<"Invalid DFRFDIRI string specified, defaulting to DF";
    return DF;
}

QString PCx_AuditModel::OREDtoCompleteString(ORED ored)
{
    switch(ored)
    {
    case OUVERTS:
        return tr("prévu");
    case REALISES:
        return tr("réalisé");
    case ENGAGES:
        return tr("engagé");
    case DISPONIBLES:
        return tr("disponible");
    default:
        qCritical()<<"Invalid ORED specified !";
    }
    return QString();
}

//Warning, be called twice (see isDirty)
void PCx_AuditModel::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(bottomRight);
    QSqlTableModel *model=(QSqlTableModel *)topLeft.model();

    qDebug()<<"Audit Data changed for model "<<model->tableName()<<": topleft column = "<<topLeft.column()<<" topleft row = "<<topLeft.row()<<"bottomRight column = "<<bottomRight.column()<<" bottomRight row = "<<bottomRight.row();
    qDebug()<<"Model dirty : "<<model->isDirty();
    int row=topLeft.row();

    QVariant vOuverts=model->index(row,COL_OUVERTS).data();
    QVariant vRealises=model->index(row,COL_REALISES).data();
    QVariant vEngages=model->index(row,COL_ENGAGES).data();

    qDebug()<<"VOUVERTS = "<<vOuverts;

    qint64 ouverts=vOuverts.toLongLong();
    qint64 realises=vRealises.toLongLong();
    qint64 engages=vEngages.toLongLong();

    if(!vRealises.isNull() && !vOuverts.isNull() && !vEngages.isNull())
    {
        QVariant disponibles=ouverts-(realises+engages);
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

bool PCx_AuditModel::unFinishAudit(unsigned int id)
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
            qCritical()<<"De-terminer Audit inexistant !";
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
        die();
    }
    q.prepare("update index_audits set termine=0 where id=:id");
    q.bindValue(":id",id);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError().text();
        die();
    }
    return true;
}

