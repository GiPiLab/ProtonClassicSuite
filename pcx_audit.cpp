#include "pcx_audit.h"
#include "pcx_prevision.h"
#include "pcx_report.h"
#include "xlsxdocument.h"
#include "pcx_query.h"
#include "utils.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <ctime>
#include <QSqlError>
#include <QSet>
#include <QDebug>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QFileInfo>
#include <QElapsedTimer>

using namespace NUMBERSFORMAT;

PCx_Audit::PCx_Audit(unsigned int auditId,bool _noLoadAttachedTree) :
    auditId(auditId)
{
    if(auditId==0)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;
    if(!q.exec(QString("select * from index_audits where id='%1'").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        auditName=q.value("nom").toString();
        attachedTreeId=q.value("id_arbre").toUInt();

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
        qCritical()<<"Invalid audit ID !";
        die();
    }
    if(_noLoadAttachedTree)
    {
        attachedTree=nullptr;
    }
    else
    {
        attachedTree=new PCx_Tree(attachedTreeId);
        attachedTreeName=attachedTree->getName();
    }

}

PCx_Audit::~PCx_Audit()
{
    if(attachedTree!=nullptr)
        delete attachedTree;
    /*idToChildren.clear();
    idToChildrenString.clear();
    idToPid.clear();*/
}

void PCx_Audit::finishAudit()
{
    finished=true;
    QSqlQuery q;
    q.prepare("update index_audits set termine=1 where id=:id");
    q.bindValue(":id",auditId);
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
}

void PCx_Audit::unFinishAudit()
{
    finished=false;
    QSqlQuery q;
    q.prepare("update index_audits set termine=0 where id=:id");
    q.bindValue(":id",auditId);
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
}

bool PCx_Audit::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED,double> vals, bool fastMode)
{
    if(!fastMode)
    {
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
        if(!getAttachedTree()->isLeaf(leafId))
        {
            qWarning()<<"Not a leaf !";
            return false;
        }
    }

    QString reqString;

    if(vals.contains(PCx_Audit::ORED::OUVERTS))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("ouverts=:vouverts");
    }
    if(vals.contains(PCx_Audit::ORED::REALISES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("realises=:vrealises");
    }
    if(vals.contains(PCx_Audit::ORED::ENGAGES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("engages=:vengages");
    }

    QSqlQuery q;
    QString tableName=QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditId);

    //qDebug()<<"Request String ="<<reqString;

    q.prepare(QString("update %1 set %2 where id_node=:id_node and annee=:year")
              .arg(tableName).arg(reqString));

    if(vals.contains(PCx_Audit::ORED::OUVERTS))
    {
        q.bindValue(":vouverts",doubleToFixedPoint(vals.value(PCx_Audit::ORED::OUVERTS)));
    }

    if(vals.contains(PCx_Audit::ORED::REALISES))
    {
        q.bindValue(":vrealises",doubleToFixedPoint(vals.value(PCx_Audit::ORED::REALISES)));
    }

    if(vals.contains(PCx_Audit::ORED::ENGAGES))
    {
        q.bindValue(":vengages",doubleToFixedPoint(vals.value(PCx_Audit::ORED::ENGAGES)));
    }

    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
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

    q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) where id_node=:id_node and annee=:year").arg(tableName));
    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
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

    updateParent(tableName,year,leafId);
    return true;
}

qint64 PCx_Audit::getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int year) const
{
    QSqlQuery q;
    q.prepare(QString("select %1 from audit_%2_%3 where annee=:year and id_node=:node").arg(OREDtoTableString(ored)).arg(modeToTableString(mode)).arg(auditId));
    q.bindValue(":year",year);
    q.bindValue(":node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qWarning()<<"No value for node"<<nodeId;
        return -MAX_NUM;
    }
    if(q.value(OREDtoTableString(ored)).isNull())
    {
        return -MAX_NUM;
    }
    return q.value(OREDtoTableString(ored)).toLongLong();
}


void PCx_Audit::clearAllData(MODES::DFRFDIRI mode)
{
    QSqlQuery q(QString("update audit_%1_%2 set ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL").arg(modeToTableString(mode)).arg(auditId));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()<0)
    {
        qCritical()<<q.lastError();
        die();
    }
}

int PCx_Audit::duplicateAudit(const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI,bool copyQueries) const
{
    if(newName.isEmpty()||newName.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }

    if(PCx_Audit::auditNameExists(newName))
    {
        qWarning()<<"Audit name already exists !";
        return -1;
    }

    QStringList modes;

    if(copyDF)
        modes.append(MODES::modeToTableString(MODES::DFRFDIRI::DF));
    if(copyRF)
        modes.append(MODES::modeToTableString(MODES::DFRFDIRI::RF));
    if(copyDI)
        modes.append(MODES::modeToTableString(MODES::DFRFDIRI::DI));
    if(copyRI)
        modes.append(MODES::modeToTableString(MODES::DFRFDIRI::RI));


    QProgressDialog progress(QObject::tr("Données en cours de recopie..."),0,0,modes.size()*4+1);
    progress.setMinimumDuration(0);

    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    int progval=1;
    progress.setValue(0);


    unsigned int newAuditId=PCx_Audit::addNewAudit(newName,years,attachedTreeId);

    if(newAuditId==0)
    {
        qFatal("Assertion failed");
    }

    qSort(years);
    progress.setValue(1);
    unsigned int year1=years.first();
    unsigned int year2=years.last();

    QSqlDatabase::database().transaction();

    QSqlQuery q;


    foreach(QString lemode,modes)
    {
        q.prepare(QString("update audit_%3_%1 set ouverts="
                          "(select ouverts from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newAuditId).arg(auditId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
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
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();

        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
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
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
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
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);
    }

    progress.setValue(progress.maximum());

    if(copyQueries)
    {
        q.exec(QString("insert into audit_queries_%1 select * from audit_queries_%2").arg(newAuditId).arg(auditId));
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Audit::deleteAudit(newAuditId);
            qCritical()<<q.lastError();
            die();
        }
    }

    QSqlDatabase::database().commit();
    return newAuditId;

}



void PCx_Audit::updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId)
{
    QString childrenString;
    unsigned int parent;
    parent=idToPid.value(nodeId);
    if(parent==0)
    {
        parent=getAttachedTree()->getParentId(nodeId);
        idToPid.insert(nodeId,parent);
    }

    //qDebug()<<"Parent of "<<nodeId<<" = "<<parent;
    QList<unsigned int> listOfChildren;
    if(idToChildren.contains(parent))
    {
        listOfChildren=idToChildren.value(parent);
        childrenString=idToChildrenString.value(parent);
    }
    else
    {
        listOfChildren=getAttachedTree()->getChildren(parent);
        idToChildren.insert(parent,listOfChildren);
        QStringList l;
        foreach (unsigned int childId, listOfChildren)
        {
            l.append(QString::number(childId));
        }
        childrenString=l.join(',');
        idToChildrenString.insert(parent,childrenString);
    }

    //qDebug()<<"Children of "<<nodeId<<" = "<<listOfChildren;
    QSqlQuery q;

    q.prepare(QString("select ouverts,realises,engages,disponibles from %1 where id_node in(%2) and annee=:annee").arg(tableName).arg(childrenString));
    q.bindValue(":annee",annee);
    if(!q.exec())
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
        //Uses index to speedup
        if(!q.value(0).isNull())
        {
            sumOuverts+=q.value(0).toLongLong();
            nullOuverts=false;
        }

        if(!q.value(1).isNull())
        {
            sumRealises+=q.value(1).toLongLong();
            nullRealises=false;
        }
        if(!q.value(2).isNull())
        {
            sumEngages+=q.value(2).toLongLong();
            nullEngages=false;
        }
        if(!q.value(3).isNull())
        {
            sumDisponibles+=q.value(3).toLongLong();
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
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(parent>1)
    {
        updateParent(tableName,annee,parent);
    }
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
               .arg(getAttachedTree()->getNumberOfNodes())
               .arg(yearsString)
               .arg(creationTimeLocal.toString(Qt::SystemLocaleLongDate).toHtmlEscaped())
               .arg(finishedString);


    out.append("\n<br><table cellpadding='5' border='1' align='center'>\n"
                       "<tr><th>&nbsp;</th><th>DF</th><th>RF</th><th>DI</th><th>RI</th></tr>");

    PCx_Tree tree(attachedTreeId);


    QList<QList<unsigned int> * > listOfListOfNodes;
    QList<unsigned int> nodesDF,nodesRF,nodesDI,nodesRI;
    listOfListOfNodes.append(&nodesDF);
    listOfListOfNodes.append(&nodesRF);
    listOfListOfNodes.append(&nodesDI);
    listOfListOfNodes.append(&nodesRI);

    foreach(unsigned int year,years)
    {
        nodesDF=getNodesWithNonNullValues(MODES::DFRFDIRI::DF,year);
        nodesRF=getNodesWithNonNullValues(MODES::DFRFDIRI::RF,year);
        nodesDI=getNodesWithNonNullValues(MODES::DFRFDIRI::DI,year);
        nodesRI=getNodesWithNonNullValues(MODES::DFRFDIRI::RI,year);
        out.append(QString("\n<tr><th colspan='5'>%1</th></tr>\n"
                           "<tr><td>Noeuds contenant au moins une valeur (même zéro)</td><td align='right'>%2</td><td align='right'>%3</td><td  align='right'>%4</td><td  align='right'>%5</td></tr>")
                   .arg(year)
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

        nodesDF=getNodesWithAllZeroValues(MODES::DFRFDIRI::DF,year);
        nodesRF=getNodesWithAllZeroValues(MODES::DFRFDIRI::RF,year);
        nodesDI=getNodesWithAllZeroValues(MODES::DFRFDIRI::DI,year);
        nodesRI=getNodesWithAllZeroValues(MODES::DFRFDIRI::RI,year);

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
                        out.append(QString("<li>%1</li>").arg(tree.getNodeName(node).toHtmlEscaped()));
                    }
                    out.append("</ul>");
                }
                out.append("</td>");
            }
            out.append("</tr>");
        }

        nodesDF=getNodesWithAllNullValues(MODES::DFRFDIRI::DF,year);
        nodesRF=getNodesWithAllNullValues(MODES::DFRFDIRI::RF,year);
        nodesDI=getNodesWithAllNullValues(MODES::DFRFDIRI::DI,year);
        nodesRI=getNodesWithAllNullValues(MODES::DFRFDIRI::RI,year);

        out.append(QString("<tr><td>Noeuds non remplis</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

    }

    out.append("</table>\n");

    return out;

}

QList<unsigned int> PCx_Audit::getNodesWithAllNullValues(MODES::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    if(!(year>=years.first()&& year<=years.last()))
    {
        qFatal("Assertion failed");
    }
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)
              <<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
              <<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 is null and %4 is null and %5 is null) and annee=:year").arg(tableMode).arg(auditId)
              .arg(oredStrings.at(0)).arg(oredStrings.at(1)).arg(oredStrings.at(2)));

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


QList<unsigned int> PCx_Audit::getNodesWithNonNullValues(MODES::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    if(!(year>=years.first()&& year<=years.last()))
    {
        qFatal("Assertion failed");
    }
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)
              <<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
                 <<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 not null or %4 not null or %5 not null) and annee=:year").arg(tableMode).arg(auditId)
                .arg(oredStrings.at(0)).arg(oredStrings.at(1)).arg(oredStrings.at(2)));

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

QList<unsigned int> PCx_Audit::getNodesWithAllZeroValues(MODES::DFRFDIRI mode, unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    if(!(year>=years.first()&& year<=years.last()))
    {
        qFatal("Assertion failed");
    }
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)<<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
                 <<PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
    QSqlQuery q;
    QString sq=QString("select id_node from audit_%1_%2 where (%3 = 0 and %4 = 0 and %5 = 0) and annee=:year").arg(tableMode).arg(auditId)
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

bool PCx_Audit::importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode)
{
    if(fileName.isEmpty())
    {
        qFatal("Assertion failed");
    }
    //QElapsedTimer timer;
    //timer.start();

    QFileInfo fi(fileName);
    if(!fi.isReadable()||!fi.isFile())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier invalide ou non lisible"));
        return false;
    }

    QXlsx::Document xlsx(fileName);

    if(!(xlsx.read(1,1).isValid() && xlsx.read(1,2).isValid() &&
         xlsx.read(1,3).isValid() && xlsx.read(1,4).isValid() &&
         xlsx.read(1,5).isValid() && xlsx.read(1,6).isValid()))
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide. Vous pouvez exporter un fichier pré-rempli à l'aide du bouton exporter dans la fenêtre de saisie des données."));
        return false;
    }

    int rowCount=xlsx.dimension().rowCount();


    int row=2;
    if(rowCount<2)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier vide. Vous pouvez exporter un fichier pré-rempli à l'aide du bouton exporter dans la fenêtre de saisie des données."));
        return false;
    }


    QProgressDialog progress(QObject::tr("Vérification en cours..."),QObject::tr("Annuler"),2,rowCount);
    progress.setMinimumDuration(200);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setValue(2);

    do
    {
        QVariant nodeType,nodeName,year,ouverts,realises,engages;
        nodeType=xlsx.read(row,1);
        nodeName=xlsx.read(row,2);
        year=xlsx.read(row,3);
        ouverts=xlsx.read(row,4);
        realises=xlsx.read(row,5);
        engages=xlsx.read(row,6);

        if(!(nodeType.isValid() && nodeName.isValid() && year.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, remplissez le type et le nom du noeud ainsi que l'année d'application").arg(row));
            return false;
        }

        QPair<QString,QString> typeAndNode;
        typeAndNode.first=nodeType.toString().simplified();
        typeAndNode.second=nodeName.toString().simplified();
        if(typeAndNode.first.isEmpty() || typeAndNode.second.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud ne peuvent pas être vides (ni composés uniquement d'espaces)").arg(row));
            return false;

        }

        if(typeAndNode.first.size()>PCx_Tree::MAXNODENAMELENGTH || typeAndNode.second.size()>PCx_Tree::MAXNODENAMELENGTH)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud sont trop longs").arg(row));
            return false;
        }

        if(!years.contains(year.toUInt()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("L'année ligne %1 n'est pas valable pour cet audit !").arg(row));
            return false;
        }
        double dblOuv=ouverts.toDouble();
        double dblReal=realises.toDouble();
        double dblEng=engages.toDouble();


        if(dblOuv>MAX_NUM||dblReal>MAX_NUM||dblEng>=MAX_NUM)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur trop grande ligne %1 (valeur maximale : %2) !").arg(row).arg(MAX_NUM));
            return false;
        }

        if(!qIsFinite(dblOuv)||!qIsFinite(dblReal)||!qIsFinite(dblEng))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur infinie ligne %1 non autorisée !").arg(row));
            return false;
        }

        if(dblOuv<0.0 || dblReal<0.0 || dblEng<0.0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur négative ligne %1 non autorisée !").arg(row));
            return false;
        }

        int nodeId;
        nodeId=getAttachedTree()->getNodeIdFromTypeAndNodeName(typeAndNode);
        if(nodeId<=0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Noeud introuvable ligne %1").arg(row));
            return false;
        }

        bool leaf=getAttachedTree()->isLeaf(nodeId);

        if(!leaf)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud ligne %1 n'est pas une feuille ! Remplissez les données pour les feuilles seulement, en utilisant un fichier modèle obtenu à l'aide du bouton d'exportation").arg(row));
            return false;
        }
        row++;

        if(!progress.wasCanceled())
        {
            progress.setValue(row);
        }
        else
        {
            return false;
        }


    }while(row<=rowCount);

    row=2;

    QSqlDatabase::database().transaction();

    this->clearAllData(mode);


    // QCoreApplication::processEvents(QEventLoop::AllEvents);

    QProgressDialog progress2(QObject::tr("Chargement des données en cours..."),QObject::tr("Annuler"),2,rowCount);

    progress2.setMinimumDuration(200);

    progress2.setWindowModality(Qt::ApplicationModal);

    progress2.setValue(2);

    do
    {
        QVariant nodeType,nodeName,year,ouverts,realises,engages;
        nodeType=xlsx.read(row,1);
        nodeName=xlsx.read(row,2);
        year=xlsx.read(row,3);
        ouverts=xlsx.read(row,4);
        realises=xlsx.read(row,5);
        engages=xlsx.read(row,6);

        //Here year and node are found and correct
        QPair<QString,QString> typeAndNode;
        typeAndNode.first=nodeType.toString().simplified();
        typeAndNode.second=nodeName.toString().simplified();
        int nodeId;
        nodeId=getAttachedTree()->getNodeIdFromTypeAndNodeName(typeAndNode);

        QHash<PCx_Audit::ORED,double> vals;
        if(ouverts.isValid())
        {
            double valDbl=ouverts.toDouble();
            vals.insert(PCx_Audit::ORED::OUVERTS,valDbl);
        }
        if(realises.isValid())
        {
            double valDbl=realises.toDouble();
            vals.insert(PCx_Audit::ORED::REALISES,valDbl);
        }
        if(engages.isValid())
        {
            double valDbl=engages.toDouble();
            vals.insert(PCx_Audit::ORED::ENGAGES,valDbl);
        }
        if(vals.size()>0)
        {
            //setLeafValues in fast mode only die on db error
            setLeafValues(nodeId,mode,year.toUInt(),vals,true);
        }
        vals.clear();

        row++;
        if(!progress2.wasCanceled())
        {
            progress2.setValue(row);
        }
        else
        {
            QSqlDatabase::database().rollback();
            return false;
        }

    }while(row<=rowCount);

    QSqlDatabase::database().commit();
    //qDebug()<<"Import done in "<<timer.elapsed()<<" ms";
    return true;

}

bool PCx_Audit::exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString & fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=getAttachedTree()->getLeavesId();
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Année");
    xlsx.write(1,4,PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    xlsx.write(1,5,PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    xlsx.write(1,6,PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));

    QSqlQuery q;
    int row=2;
    foreach(unsigned int leafId,leavesId)
    {
        QPair<QString,QString> typeAndNodeName=getAttachedTree()->getTypeNameAndNodeName(leafId);
            q.prepare(QString("select * from audit_%1_%2 where id_node=:idnode order by annee").
                      arg(MODES::modeToTableString(mode)).arg(auditId));

            q.bindValue(":idnode",leafId);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            while(q.next())
            {
                QVariant valOuverts=q.value("ouverts");
                QVariant valRealises=q.value("realises");
                QVariant valEngages=q.value("engages");

                xlsx.write(row,1,typeAndNodeName.first);
                xlsx.write(row,2,typeAndNodeName.second);
                xlsx.write(row,3,q.value("annee").toUInt());

                if(!valOuverts.isNull())
                {
                    //QString vOuverts=formatDouble((double)valOuverts.toLongLong()/FIXEDPOINTCOEFF,2,true);
                    xlsx.write(row,4,fixedPointToDouble(valOuverts.toLongLong()));
                }

                if(!valRealises.isNull())
                {
                    xlsx.write(row,5,fixedPointToDouble(valRealises.toLongLong()));
                }

                if(!valEngages.isNull())
                {
                    xlsx.write(row,6,fixedPointToDouble(valEngages.toLongLong()));
                }
                row++;
            }
    }

    return xlsx.saveAs(fileName);
}

void PCx_Audit::fillWithRandomData(MODES::DFRFDIRI mode)
{
    QList<unsigned int> leaves=getAttachedTree()->getLeavesId();

    QHash<PCx_Audit::ORED,double> data;

    int maxVal=leaves.size();

    QProgressDialog progress(QObject::tr("Génération des données aléatoires..."),QObject::tr("Annuler"),0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);

    QSqlDatabase::database().transaction();

    clearAllData(mode);

    int nbNode=0;
    qsrand(time(NULL));

    double randval;

    foreach(unsigned int leaf,leaves)
    {
        foreach(unsigned int year,years)
        {
            data.clear();
            randval=qrand()/(double)(RAND_MAX/(double)MAX_NUM*1000);

            data.insert(PCx_Audit::ORED::OUVERTS,randval);
            randval=qrand()/(double)(RAND_MAX/(double)MAX_NUM*1000);

            data.insert(PCx_Audit::ORED::REALISES,randval);
            randval=qrand()/(double)(RAND_MAX/(double)MAX_NUM*1000);

            data.insert(PCx_Audit::ORED::ENGAGES,randval);

            //the transaction will be rollback in setLeafValues=>die
            setLeafValues(leaf,mode,year,data,true);
        }
        nbNode++;
        if(!progress.wasCanceled())
        {
            progress.setValue(nbNode);
        }
        else
        {
            QSqlDatabase::database().rollback();
            return;
        }
    }
    QSqlDatabase::database().commit();
}

QString PCx_Audit::getCSS()
{
    QString css="\nbody{font-family:sans-serif;font-size:9pt;background-color:white;color:black;}"
            "\nh1{color:#A00;}"
            "\nh2{color:navy;}"
            "\nh3{color:green;font-size:larger}";

    css.append(PCx_Query::getCSS());
    css.append(PCx_Tables::getCSS());
    css.append(PCx_Graphics::getCSS());
    return css;
}


QString PCx_Audit::generateHTMLHeader() const
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head><title>Audit %1</title>\n<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>\n<style type='text/css'>\n%2\n</style>\n</head>\n<body>").arg(auditName.toHtmlEscaped()).arg(getCSS());
}

QString PCx_Audit::generateHTMLAuditTitle() const
{
    return QString("<h3>Audit %1 (%2), arbre %3</h3>").arg(auditName.toHtmlEscaped()).arg(yearsString).arg(attachedTreeName.toHtmlEscaped());
}





QString PCx_Audit::OREDtoCompleteString(ORED ored)
{
    switch(ored)
    {
    case ORED::OUVERTS:
        return QObject::tr("prévu");
    case ORED::REALISES:
        return QObject::tr("réalisé");
    case ORED::ENGAGES:
        return QObject::tr("engagé");
    case ORED::DISPONIBLES:
        return QObject::tr("disponible");
    default:
        qWarning()<<"Invalid ORED specified !";
    }
    return QString();
}

QString PCx_Audit::OREDtoTableString(ORED ored)
{
    switch(ored)
    {
    case ORED::OUVERTS:
        return "ouverts";
    case ORED::REALISES:
        return "realises";
    case ORED::ENGAGES:
        return "engages";
    case ORED::DISPONIBLES:
        return "disponibles";
    default:
        qWarning()<<"Invalid ORED specified !";
    }
    return QString();
}

PCx_Audit::ORED PCx_Audit::OREDFromTableString(const QString &oredString)
{
    if(oredString==OREDtoTableString(ORED::OUVERTS))
        return ORED::OUVERTS;
    else if(oredString==OREDtoTableString(ORED::REALISES))
        return ORED::REALISES;
    else if(oredString==OREDtoTableString(ORED::ENGAGES))
        return ORED::ENGAGES;
    else if(oredString==OREDtoTableString(ORED::DISPONIBLES))
        return ORED::DISPONIBLES;
    else
        qWarning()<<"Invalid ORED string !";
    return ORED::OUVERTS;
}


unsigned int PCx_Audit::addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId)
{
    if(years.isEmpty() || name.isEmpty() || attachedTreeId==0 || name.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }

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


    QList<unsigned int> treeIds=PCx_Tree::getListOfTreesId(true);
    if(!treeIds.contains(attachedTreeId))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Impossible de construire un audit sur un arbre non existant ou non terminé"));
        return 0;
    }


    if(auditNameExists(name))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un audit portant ce nom ! Choisissez un autre nom"));
        return 0;
    }

    QSqlQuery q;

    if(!q.exec(QString("select count(*) from index_arbres where id='%1'").arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qCritical()<<"Trying to create an audit on a non-existant tree !";
            die();
        }
    }

    QSqlDatabase::database().transaction();
    q.prepare("insert into index_audits (nom,id_arbre,annees) values (:nom,:id_arbre,:annees)");
    q.bindValue(":nom",name);
    q.bindValue(":id_arbre",attachedTreeId);
    q.bindValue(":annees",yearsString);
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

    //Data are integer for fixed points arithmetics, stored with 3 decimals
    if(!q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, "
                       "annee integer not null, ouverts integer, realises integer, engages integer, "
                       "disponibles integer)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_audit_DF_%1 on audit_DF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, "
                       "annee integer not null, ouverts integer, realises integer, engages integer, "
                       "disponibles integer)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_audit_RF_%1 on audit_RF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, "
                       "annee integer not null, ouverts integer, realises integer, engages integer, "
                       "disponibles integer)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_audit_DI_%1 on audit_DI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, "
                       "annee integer not null, ouverts integer, realises integer, engages integer, "
                       "disponibles integer)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_audit_RI_%1 on audit_RI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    PCx_Query::createTableQueries(uLastId);

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
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_RF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_DI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.numRowsAffected()==-1)
            {
                qCritical()<<q.lastError();
                die();
            }

            q.prepare(QString("insert into audit_RI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
            q.bindValue(":idnode",node);
            q.bindValue(":annee",annee);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.numRowsAffected()==-1)
            {
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
    if(auditId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q(QString("select count(*) from index_audits where id='%1'").arg(auditId));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qWarning()<<"Attempting to delete an inexistant audit !";
            return false;
        }
    }

    if(!q.exec(QString("select count(*) from index_previsions where id_audit='%1'").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(0,QObject::tr("Attention"),QObject::tr("Il existe des prévisions attachées à cet audit, supprimez-les d'abord"));
            return false;
        }
    }

    QSqlDatabase::database().transaction();
    if(!q.exec(QString("delete from index_audits where id='%1'").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table audit_DF_%1").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table audit_RF_%1").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table audit_DI_%1").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table audit_RI_%1").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("drop table audit_queries_%1").arg(auditId)))
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


bool PCx_Audit::auditNameExists(const QString &auditName)
{
    QSqlQuery q;
    q.prepare("select count(*) from index_audits where nom=:name");
    q.bindValue(":name",auditName);
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

QList<QPair<unsigned int, QString> > PCx_Audit::getListOfAudits(ListAuditsMode mode)
{
    QList<QPair<unsigned int,QString> > listOfAudits;
    QDateTime dt;

    QSqlQuery query;
    if(!query.exec("select * from index_audits order by datetime(le_timestamp)"))
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
        if(query.value("termine").toBool()==true)
        {
            //Finished audit
            item=QString("%1 - %2 (audit terminé)").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
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

QList<QPair<unsigned int, QString> > PCx_Audit::getListOfAuditsAttachedWithThisTree(unsigned int treeId, PCx_Audit::ListAuditsMode mode)
{
    if(treeId==0)
    {
        qFatal("Assertion failed");
    }
    QList<QPair<unsigned int,QString> > listOfAudits;
    QDateTime dt;

    QSqlQuery query;
    query.prepare("select * from index_audits where id_arbre=:idarbre order by datetime(le_timestamp)");
    query.bindValue(":idarbre",treeId);
    if(!query.exec())
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

bool PCx_Audit::exportLeavesSkeleton(const QString &fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=getAttachedTree()->getLeavesId();
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Année");
    xlsx.write(1,4,PCx_Audit::OREDtoCompleteString(ORED::OUVERTS));
    xlsx.write(1,5,PCx_Audit::OREDtoCompleteString(ORED::REALISES));
    xlsx.write(1,6,PCx_Audit::OREDtoCompleteString(ORED::ENGAGES));

    int row=2;
    foreach(unsigned int year, years)
    {
        foreach(unsigned int leaf, leavesId)
        {
            QPair<QString,QString> typeAndNodeName=getAttachedTree()->getTypeNameAndNodeName(leaf);
            xlsx.write(row,1,typeAndNodeName.first);
            xlsx.write(row,2,typeAndNodeName.second);
            xlsx.write(row,3,year);
            xlsx.write(row,4,0);
            xlsx.write(row,5,0);
            xlsx.write(row,6,0);
            row++;
        }
    }

    return xlsx.saveAs(fileName);
}

