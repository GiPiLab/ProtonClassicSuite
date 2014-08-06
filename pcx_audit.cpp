#include "pcx_audit.h"
#include "pcx_query.h"
#include "utils.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QDebug>
#include <QProgressDialog>
#include <QCoreApplication>

PCx_Audit::PCx_Audit(unsigned int auditId, bool loadTreeModel) :
    auditId(auditId),loadTreeModel(loadTreeModel)
{
    attachedTree=NULL;

    Q_ASSERT(auditId>0);
    this->auditId=auditId;
    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        auditName=q.value("nom").toString();
        attachedTreeId=q.value("id_arbre").toUInt();
        attachedTreeName=PCx_TreeModel::idTreeToName(attachedTreeId);
        yearsString=q.value("annees").toString();
        QStringList yearsSplitted=yearsString.split(',');

        QSet<unsigned int> yearsTemp;
        foreach (QString uneAnnee, yearsSplitted) {
            yearsTemp.insert(uneAnnee.toUInt());
        }
        years=yearsTemp.toList();
        qSort(years);

        foreach(unsigned int annee,years)
        {
            yearsStringList.append(QString::number(annee));
        }

        yearsString=QString("%1 - %2").arg(years.first()).arg(years.last());

        finished=q.value("termine").toBool();
        if(finished==true)
        {
            finishedString=QObject::tr("oui");
        }
        else
        {
            finishedString=QObject::tr("non");
        }
        creationTimeUTC=QDateTime::fromString(q.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
    }
    else
    {
        qCritical()<<"Non existant audit !";
        die();
    }
    if(loadTreeModel)
        attachedTree=new PCx_TreeModel(attachedTreeId);
}

PCx_Audit::~PCx_Audit()
{
    delete attachedTree;
}

unsigned int PCx_Audit::addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId)
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

    q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, id_node integer not null, annee integer not null, ouverts integer, realises integer, engages integer, disponibles integer)").arg(uLastId));

    if(q.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
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

bool PCx_Audit::deleteAudit(unsigned int auditId)
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

QString PCx_Audit::getAuditName(unsigned int auditId)
{
    QSqlQuery q(QString("select * from index_audits where id=%1").arg(auditId));
    if(!q.next())
    {
        qCritical()<<"Invalid audit ID !";
        return QString();
    }
    return q.value("nom").toString();
}

unsigned int PCx_Audit::getAttachedTreeId(unsigned int auditId)
{
    QSqlQuery q(QString("select * from index_audits where id=%1").arg(auditId));
    if(!q.next())
    {
        qCritical()<<"Invalid audit ID !";
        return 0;
    }
    return q.value("id_arbre").toUInt();
}


bool PCx_Audit::finishAudit()
{
    finished=true;
    return PCx_Audit::finishAudit(auditId);
}

bool PCx_Audit::unFinishAudit()
{
    finished=false;
    return PCx_Audit::unFinishAudit(auditId);
}


bool PCx_Audit::setLeafValues(unsigned int leafId, PCx_Audit::DFRFDIRI mode, unsigned int year, QHash<ORED,double> vals)
{
    Q_ASSERT(!vals.isEmpty());

    if(vals.contains(DISPONIBLES))
    {
        qWarning()<<"Will not overwrite the computed column DISPONIBLE";
        return false;
    }
    if(finished)
    {
        qWarning()<<"Will not modify a finished audit";
        return false;
    }
    if(year<years.first() || year>years.last())
    {
        qWarning()<<"Invalid years !";
        return false;
    }
    if(!attachedTree->isLeaf(leafId))
    {
        qWarning()<<"Not a leaf !";
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
        QSqlDatabase::database().rollback();
        qCritical()<<"Error while setting value"<<q.lastError();
        die();
    }

    q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) where id_node=:id_node and annee=:year").arg(tableName));
    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<"Error while setting disponibles values"<<q.lastError();
        die();
    }

    if(updateParent(tableName,year,leafId))
    {
        QSqlDatabase::database().commit();
    }
    else
    {
        QSqlDatabase::database().rollback();
        qCritical()<<"ERROR DURING PROPAGATING VALUES TO ROOTS, CANCELLING";
        die();
    }
    return true;
}

qint64 PCx_Audit::getNodeValue(unsigned int nodeId, PCx_Audit::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int year) const
{
    QSqlQuery q;
    q.prepare(QString("select %1 from audit_%2_%3 where annee=:year and id_node=:node").arg(OREDtoTableString(ored)).arg(modeToTableString(mode)).arg(auditId));
    q.bindValue(":year",year);
    q.bindValue(":node",nodeId);
    q.exec();
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.value(OREDtoTableString(ored)).isNull())
    {
        qDebug()<<"NULL value";
        return -MAX_NUM;
    }
    return q.value(OREDtoTableString(ored)).toLongLong();
}

bool PCx_Audit::clearAllData(PCx_Audit::DFRFDIRI mode)
{
    QSqlQuery q(QString("update audit_%1_%2 set ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL").arg(modeToTableString(mode)).arg(auditId));
    q.exec();
    if(q.numRowsAffected()<0)
    {
        qCritical()<<q.lastError();
        die();
    }
    return true;
}

int PCx_Audit::duplicateAudit(const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI) const
{
    return PCx_Audit::duplicateAudit(auditId,newName,years,copyDF,copyRF,copyDI,copyRI);
}

int PCx_Audit::duplicateAudit(unsigned int auditId, const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI)
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
    unsigned int newAuditId=PCx_Audit::addNewAudit(newName,years,PCx_Audit::getAttachedTreeId(auditId));
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

    QCoreApplication::processEvents(QEventLoop::AllEvents,100);

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
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);


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

        QCoreApplication::processEvents(QEventLoop::AllEvents,100);

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


        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
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
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);

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

bool PCx_Audit::updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId)
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
        qCritical()<<q.lastError();
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
        qCritical()<<q.lastError();
        return false;
    }

    if(parent>1)
    {
        updateParent(tableName,annee,parent);
    }
    return true;
}

QString PCx_Audit::modeToTableString(DFRFDIRI mode)
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



QString PCx_Audit::modeToCompleteString(DFRFDIRI mode)
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


QString PCx_Audit::OREDtoTableString(ORED ored)
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

PCx_Audit::ORED PCx_Audit::OREDFromTableString(const QString &ored)
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

PCx_Audit::DFRFDIRI PCx_Audit::modeFromTableString(const QString &mode)
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

bool PCx_Audit::auditNameExists(const QString &auditName)
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

QString PCx_Audit::OREDtoCompleteString(ORED ored)
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




QList<QPair<unsigned int, QString> > PCx_Audit::getListOfAudits(ListAuditsMode mode)
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

bool PCx_Audit::finishAudit(unsigned int id)
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
    q.bindValue(":id",id);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
    return true;
}

bool PCx_Audit::unFinishAudit(unsigned int id)
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
    q.bindValue(":id",id);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
    return true;
}

QString PCx_Audit::getHTMLAuditStatistics() const
{
    QString out=QString("\n<table cellpadding='5' border='1' align='center'>\n"
                       "<tr><td>Arbre associé</td><td align='right'>%1 (%2 noeuds)</td></tr>"
                       "<tr><td>Années d'application</td><td align='right'>%3</td></tr>"
                       "<tr><td>Date de création</td><td align='right'>%4</td></tr>"
                       "<tr><td>Audit terminé</td><td align='right'>%5</td></tr>"
                       "</table>\n")
               .arg(attachedTreeName.toHtmlEscaped())
               .arg(PCx_TreeModel::getNumberOfNodes(attachedTreeId))
               .arg(yearsString)
               .arg(creationTimeLocal.toString(Qt::SystemLocaleLongDate).toHtmlEscaped())
               .arg(finishedString);


    out.append("\n<br><table cellpadding='5' border='1' align='center'>\n"
                       "<tr><th>&nbsp;</th><th>DF</th><th>RF</th><th>DI</th><th>RI</th></tr>");

    PCx_TreeModel treeModel(attachedTreeId,true,true);


    QList<QList<unsigned int> * > listOfListOfNodes;
    QList<unsigned int> nodesDF,nodesRF,nodesDI,nodesRI;
    listOfListOfNodes.append(&nodesDF);
    listOfListOfNodes.append(&nodesRF);
    listOfListOfNodes.append(&nodesDI);
    listOfListOfNodes.append(&nodesRI);

    foreach(unsigned int year,years)
    {
        nodesDF=getNodesWithNonNullValues(PCx_Audit::DF,year);
        nodesRF=getNodesWithNonNullValues(PCx_Audit::RF,year);
        nodesDI=getNodesWithNonNullValues(PCx_Audit::DI,year);
        nodesRI=getNodesWithNonNullValues(PCx_Audit::RI,year);
        out.append(QString("\n<tr><th colspan='5'>%1</th></tr>\n"
                           "<tr><td>Noeuds contenant au moins une valeur (même zéro)</td><td align='right'>%2</td><td align='right'>%3</td><td  align='right'>%4</td><td  align='right'>%5</td></tr>")
                   .arg(year)
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

        nodesDF=getNodesWithAllZeroValues(PCx_Audit::DF,year);
        nodesRF=getNodesWithAllZeroValues(PCx_Audit::RF,year);
        nodesDI=getNodesWithAllZeroValues(PCx_Audit::DI,year);
        nodesRI=getNodesWithAllZeroValues(PCx_Audit::RI,year);

        out.append(QString("<tr><td>Noeuds dont les valeurs sont toutes à zéro</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

        if(!nodesDF.isEmpty()||!nodesRF.isEmpty()||!nodesDI.isEmpty()||!nodesRI.isEmpty())
        {
            out.append("<tr><td></td>");

            foreach(QList<unsigned int> *listOfNodes, listOfListOfNodes)
            {
                out.append("<td>");
                if(!listOfNodes->isEmpty())
                {
                    out.append("<ul>");

                    foreach(unsigned int node,*listOfNodes)
                    {
                        out.append(QString("<li>%1</li>").arg(treeModel.getNodeName(node).toHtmlEscaped()));
                    }
                    out.append("</ul>");
                }
                out.append("</td>");
            }
            out.append("</tr>");
        }

        nodesDF=getNodesWithAllNullValues(PCx_Audit::DF,year);
        nodesRF=getNodesWithAllNullValues(PCx_Audit::RF,year);
        nodesDI=getNodesWithAllNullValues(PCx_Audit::DI,year);
        nodesRI=getNodesWithAllNullValues(PCx_Audit::RI,year);

        out.append(QString("<tr><td>Noeuds non remplis</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

    }

    out.append("</table>\n");

    return out;

}

QList<unsigned int> PCx_Audit::getNodesWithAllNullValues(PCx_Audit::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_Audit::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::OUVERTS)<<PCx_Audit::OREDtoTableString(PCx_Audit::REALISES)
              <<PCx_Audit::OREDtoTableString(PCx_Audit::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select * from audit_%1_%2 where (%3 is null and %4 is null and %5 is null) and annee=:year").arg(tableMode).arg(auditId)
              .arg(oredStrings.at(0)).arg(oredStrings.at(1)).arg(oredStrings.at(2)));

    q.bindValue(":year",year);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodes.append(q.value("id_node").toUInt());
    }
    return nodes;
}


QList<unsigned int> PCx_Audit::getNodesWithNonNullValues(PCx_Audit::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_Audit::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::OUVERTS)<<PCx_Audit::OREDtoTableString(PCx_Audit::REALISES)
                 <<PCx_Audit::OREDtoTableString(PCx_Audit::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select * from audit_%1_%2 where (%3 not null or %4 not null or %5 not null) and annee=:year").arg(tableMode).arg(auditId)
                .arg(oredStrings.at(0)).arg(oredStrings.at(1)).arg(oredStrings.at(2)));

    q.bindValue(":year",year);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodes.append(q.value("id_node").toUInt());
    }
    return nodes;
}

QList<unsigned int> PCx_Audit::getNodesWithAllZeroValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const
{
    QString tableMode=PCx_Audit::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::OUVERTS)<<PCx_Audit::OREDtoTableString(PCx_Audit::REALISES)
                 <<PCx_Audit::OREDtoTableString(PCx_Audit::ENGAGES);
    QSqlQuery q;
    QString sq=QString("select * from audit_%1_%2 where (%3 = 0 and %4 = 0 and %5 = 0) and annee=:year").arg(tableMode).arg(auditId)
                .arg(oredStrings.at(0)).arg(oredStrings.at(1)).arg(oredStrings.at(2));

    q.prepare(sq);
    q.bindValue(":year",year);

    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        nodes.append(q.value("id_node").toUInt());
    }
    return nodes;
}



QStringList PCx_Audit::yearsListToStringList(QList<unsigned int> years)
{
    //De-duplicate and sort
    QSet<unsigned int> yearsTemp;
    QList<unsigned int> years2;
    yearsTemp=years.toSet();
    years2=yearsTemp.toList();
    qSort(years2);

    QStringList yearsStringList;

    foreach(unsigned int annee,years2)
    {
        yearsStringList.append(QString::number(annee));
    }
    return yearsStringList;
}