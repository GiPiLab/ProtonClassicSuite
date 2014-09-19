#include "pcx_audit.h"
#include "xlsxdocument.h"
#include "pcx_auditmanage.h"
#include "pcx_query.h"
#include "pcx_treemanage.h"
#include "utils.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QDebug>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QFileInfo>
#include <QElapsedTimer>

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
        attachedTreeName=PCx_TreeManage::idTreeToName(attachedTreeId);
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


QString PCx_Audit::getAuditName(unsigned int auditId)
{
    QSqlQuery q(QString("select nom from index_audits where id=%1").arg(auditId));
    if(!q.next())
    {
        qCritical()<<"Invalid audit ID !";
        return QString();
    }
    return q.value("nom").toString();
}


bool PCx_Audit::finishAudit()
{
    finished=true;
    return PCx_AuditManage::finishAudit(auditId);
}

bool PCx_Audit::unFinishAudit()
{
    finished=false;
    return PCx_AuditManage::unFinishAudit(auditId);
}


bool PCx_Audit::setLeafValues(unsigned int leafId, PCx_AuditManage::DFRFDIRI mode, unsigned int year, QHash<PCx_AuditManage::ORED,double> vals, bool fastMode)
{
    Q_ASSERT(!vals.isEmpty());

    if(!fastMode)
    {
        if(vals.contains(PCx_AuditManage::DISPONIBLES))
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
    }


    if(leafId==96 && year==2005)
    {
        qDebug()<<"ID96 : "<<formatDouble(vals.value(PCx_AuditManage::OUVERTS));
    }


    QString reqString;

    if(vals.contains(PCx_AuditManage::OUVERTS))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("ouverts=:vouverts");
    }
    if(vals.contains(PCx_AuditManage::REALISES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("realises=:vrealises");
    }
    if(vals.contains(PCx_AuditManage::ENGAGES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("engages=:vengages");
    }

    QSqlQuery q;
    QString tableName=QString("audit_%1_%2").arg(PCx_AuditManage::modeToTableString(mode)).arg(auditId);

    //qDebug()<<"Request String ="<<reqString;

    q.prepare(QString("update %1 set %2 where id_node=:id_node and annee=:year")
              .arg(tableName).arg(reqString));

    if(vals.contains(PCx_AuditManage::OUVERTS))
    {
        q.bindValue(":vouverts",doubleToFixedPoint(vals.value(PCx_AuditManage::OUVERTS)));
    }

    if(vals.contains(PCx_AuditManage::REALISES))
    {
        q.bindValue(":vrealises",doubleToFixedPoint(vals.value(PCx_AuditManage::REALISES)));
    }

    if(vals.contains(PCx_AuditManage::ENGAGES))
    {
        q.bindValue(":vengages",doubleToFixedPoint(vals.value(PCx_AuditManage::ENGAGES)));
    }

    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<"Error while setting value"<<q.lastError();
        return false;
    }

    q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) where id_node=:id_node and annee=:year").arg(tableName));
    q.bindValue(":id_node",leafId);
    q.bindValue(":year",year);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<"Error while setting disponibles values"<<q.lastError();
        return false;
    }

    //return true;
    return(updateParent(tableName,year,leafId));
}

qint64 PCx_Audit::getNodeValue(unsigned int nodeId, PCx_AuditManage::DFRFDIRI mode, PCx_AuditManage::ORED ored, unsigned int year) const
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

QHash<PCx_AuditManage::ORED, qint64> PCx_Audit::getNodeValues(unsigned int nodeId, PCx_AuditManage::DFRFDIRI mode, unsigned int year) const
{
    QSqlQuery q;
    q.prepare(QString("select * from audit_%2_%3 where annee=:year and id_node=:node").arg(modeToTableString(mode)).arg(auditId));
    q.bindValue(":year",year);
    q.bindValue(":node",nodeId);
    q.exec();
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }

    QHash<PCx_AuditManage::ORED,qint64> output;
    if(q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::OUVERTS)).isNull())
    {
        qDebug()<<"NULL value";
        output.insert(PCx_AuditManage::OUVERTS,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_AuditManage::OUVERTS,q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::OUVERTS)).toLongLong());
    }

    if(q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::REALISES)).isNull())
    {
        qDebug()<<"NULL value";
        output.insert(PCx_AuditManage::REALISES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_AuditManage::REALISES,q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::REALISES)).toLongLong());
    }

    if(q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::ENGAGES)).isNull())
    {
        qDebug()<<"NULL value";
        output.insert(PCx_AuditManage::ENGAGES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_AuditManage::ENGAGES,q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::ENGAGES)).toLongLong());
    }

    if(q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::DISPONIBLES)).isNull())
    {
        qDebug()<<"NULL value";
        output.insert(PCx_AuditManage::DISPONIBLES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_AuditManage::DISPONIBLES,q.value(PCx_AuditManage::OREDtoTableString(PCx_AuditManage::DISPONIBLES)).toLongLong());
    }


    return output;
}

bool PCx_Audit::clearAllData(PCx_AuditManage::DFRFDIRI mode)
{
    QSqlQuery q(QString("update audit_%1_%2 set ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL").arg(modeToTableString(mode)).arg(auditId));
    q.exec();
    if(q.numRowsAffected()<0)
    {
        qCritical()<<q.lastError();
        return false;
    }
    return true;
}

int PCx_Audit::duplicateAudit(const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI) const
{
    return PCx_AuditManage::duplicateAudit(auditId,newName,years,copyDF,copyRF,copyDI,copyRI);
}



bool PCx_Audit::updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId)
{
    QString childrenString;
    unsigned int parent;
    parent=idToPid.value(nodeId);
    if(parent==0)
    {
        parent=attachedTree->getParentId(nodeId);
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
        listOfChildren=attachedTree->getChildren(parent);
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
        return false;
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
        nodesDF=getNodesWithNonNullValues(PCx_AuditManage::DF,year);
        nodesRF=getNodesWithNonNullValues(PCx_AuditManage::RF,year);
        nodesDI=getNodesWithNonNullValues(PCx_AuditManage::DI,year);
        nodesRI=getNodesWithNonNullValues(PCx_AuditManage::RI,year);
        out.append(QString("\n<tr><th colspan='5'>%1</th></tr>\n"
                           "<tr><td>Noeuds contenant au moins une valeur (même zéro)</td><td align='right'>%2</td><td align='right'>%3</td><td  align='right'>%4</td><td  align='right'>%5</td></tr>")
                   .arg(year)
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

        nodesDF=getNodesWithAllZeroValues(PCx_AuditManage::DF,year);
        nodesRF=getNodesWithAllZeroValues(PCx_AuditManage::RF,year);
        nodesDI=getNodesWithAllZeroValues(PCx_AuditManage::DI,year);
        nodesRI=getNodesWithAllZeroValues(PCx_AuditManage::RI,year);

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

        nodesDF=getNodesWithAllNullValues(PCx_AuditManage::DF,year);
        nodesRF=getNodesWithAllNullValues(PCx_AuditManage::RF,year);
        nodesDI=getNodesWithAllNullValues(PCx_AuditManage::DI,year);
        nodesRI=getNodesWithAllNullValues(PCx_AuditManage::RI,year);

        out.append(QString("<tr><td>Noeuds non remplis</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

    }

    out.append("</table>\n");

    return out;

}

QList<unsigned int> PCx_Audit::getNodesWithAllNullValues(PCx_AuditManage::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_AuditManage::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::OUVERTS)
              <<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::REALISES)
              <<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 is null and %4 is null and %5 is null) and annee=:year").arg(tableMode).arg(auditId)
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


QList<unsigned int> PCx_Audit::getNodesWithNonNullValues(PCx_AuditManage::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_AuditManage::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::OUVERTS)
              <<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::REALISES)
                 <<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 not null or %4 not null or %5 not null) and annee=:year").arg(tableMode).arg(auditId)
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

QList<unsigned int> PCx_Audit::getNodesWithAllZeroValues(PCx_AuditManage::DFRFDIRI mode, unsigned int year) const
{
    QString tableMode=PCx_AuditManage::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::OUVERTS)<<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::REALISES)
                 <<PCx_AuditManage::OREDtoTableString(PCx_AuditManage::ENGAGES);
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

int PCx_Audit::importDataFromXLSX(const QString &fileName, PCx_AuditManage::DFRFDIRI mode)
{
    Q_ASSERT(!fileName.isEmpty());
    QElapsedTimer timer;
    timer.start();

    QFileInfo fi(fileName);
    if(!fi.isReadable()||!fi.isFile())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier invalide ou non lisible"));
        return -1;
    }

    QXlsx::Document xlsx(fileName);

    if(!(xlsx.read(1,1).isValid() && xlsx.read(1,2).isValid() &&
         xlsx.read(1,3).isValid() && xlsx.read(1,4).isValid() &&
         xlsx.read(1,5).isValid() && xlsx.read(1,6).isValid()))
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide. Vous pouvez exporter un fichier pré-rempli à l'aide du bouton exporter dans la fenêtre de saisie des données."));
        return -1;
    }

    int rowCount=xlsx.dimension().rowCount();


    int row=2;
    if(rowCount<2)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier vide. Vous pouvez exporter un fichier pré-rempli à l'aide du bouton exporter dans la fenêtre de saisie des données."));
        return -1;
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
            return -1;
        }

        QPair<QString,QString> typeAndNode;
        typeAndNode.first=nodeType.toString().simplified();
        typeAndNode.second=nodeName.toString().simplified();
        if(typeAndNode.first.isEmpty() || typeAndNode.second.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud ne peuvent pas être vides (ni composés d'espaces)").arg(row));
            return -1;

        }

        if(!years.contains(year.toUInt()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("L'année ligne %1 n'est pas valable pour cet audit !").arg(row));
            return -1;
        }
        double dblOuv=ouverts.toDouble();
        double dblReal=realises.toDouble();
        double dblEng=engages.toDouble();


        if(dblOuv>=MAX_NUM||dblReal>=MAX_NUM||dblEng>=MAX_NUM)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur trop grande ligne %1 (valeur maximale : %2) !").arg(row).arg(MAX_NUM));
            return -1;
        }

        if(!qIsFinite(dblOuv)||!qIsFinite(dblReal)||!qIsFinite(dblEng))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur infinie ligne %1 non autorisée !").arg(row));
            return -1;
        }

        if(dblOuv<0.0 || dblReal<0.0 || dblEng<0.0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur négative ligne %1 non autorisée !").arg(row));
            return -1;
        }


        int nodeId;
        nodeId=attachedTree->getNodeIdFromTypeAndNodeName(typeAndNode);
        if(nodeId<=0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Noeud introuvable ligne %1").arg(row));
            return -1;
        }

        bool leaf=attachedTree->isLeaf(nodeId);

        if(!leaf)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud ligne %1 n'est pas une feuille ! Remplissez les données pour les feuilles seulement, en utilisant un fichier modèle obtenu à l'aide du bouton d'exportation").arg(row));
            return -1;
        }
        row++;

        if(!progress.wasCanceled())
        {
            progress.setValue(row);
        }
        else
        {
            return -1;
        }


    }while(row<=rowCount);

    row=2;

    QSqlDatabase::database().transaction();

    this->clearAllData(mode);


    QCoreApplication::processEvents(QEventLoop::AllEvents);

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
        nodeId=attachedTree->getNodeIdFromTypeAndNodeName(typeAndNode);

        QHash<PCx_AuditManage::ORED,double> vals;
        if(ouverts.isValid())
        {
            double valDbl=ouverts.toDouble();
            vals.insert(PCx_AuditManage::OUVERTS,valDbl);
        }
        if(realises.isValid())
        {
            double valDbl=realises.toDouble();
            vals.insert(PCx_AuditManage::REALISES,valDbl);
        }
        if(engages.isValid())
        {
            double valDbl=engages.toDouble();
            vals.insert(PCx_AuditManage::ENGAGES,valDbl);
        }
        if(vals.size()>0)
        {
            bool res;
            res=setLeafValues(nodeId,mode,year.toUInt(),vals,true);
            if(!res)
            {
                QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de mise à jour de la ligne %1").arg(row));
                QSqlDatabase::database().rollback();
                return -1;
            }
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
            return -1;
        }

}while(row<=rowCount);


    QSqlDatabase::database().commit();
    qDebug()<<"Import done in "<<timer.elapsed()<<" ms";
    return 1;

}

bool PCx_Audit::exportLeavesDataXLSX(PCx_AuditManage::DFRFDIRI mode, const QString & fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=attachedTree->getLeavesId();
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Année");
    xlsx.write(1,4,PCx_AuditManage::OREDtoCompleteString(PCx_AuditManage::OUVERTS));
    xlsx.write(1,5,PCx_AuditManage::OREDtoCompleteString(PCx_AuditManage::REALISES));
    xlsx.write(1,6,PCx_AuditManage::OREDtoCompleteString(PCx_AuditManage::ENGAGES));

    QSqlQuery q;
    int row=2;
    foreach(unsigned int leafId,leavesId)
    {
        QPair<QString,QString> typeAndNodeName=attachedTree->getTypeNameAndNodeName(leafId);
            q.prepare(QString("select * from audit_%1_%2 where id_node=:idnode order by annee").
                      arg(PCx_AuditManage::modeToTableString(mode)).arg(auditId));

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

