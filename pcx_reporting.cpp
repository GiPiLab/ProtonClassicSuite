/*
* ProtonClassicSuite
* 
* Copyright Thibault Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/

#include "pcx_reporting.h"
#include "pcx_report.h"
#include "xlsxdocument.h"
#include "pcx_query.h"
#include "xlsxutility_p.h"
#include "utils.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <ctime>
#include <QDebug>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QFileInfo>
#include <QElapsedTimer>

using namespace NUMBERSFORMAT;

PCx_Reporting::PCx_Reporting(unsigned int reportingId,bool _noLoadAttachedTree) :
    reportingId(reportingId)
{
    if(reportingId==0)
    {
        qFatal("Assertion failed");
    }

    this->reportingId=reportingId;
    QSqlQuery q;
    if(!q.exec(QString("select * from index_reportings where id='%1'").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        reportingName=q.value("nom").toString();
        attachedTreeId=q.value("id_arbre").toUInt();

        creationTimeUTC=QDateTime::fromString(q.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
    }
    else
    {
        qCritical()<<"Invalid reporting ID !";
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

PCx_Reporting::~PCx_Reporting()
{
    if(attachedTree!=nullptr)
        delete attachedTree;
}

unsigned int PCx_Reporting::getAttachedTreeId(unsigned int reportingId)
{
    QSqlQuery q;
    if(!q.exec(QString("select id_arbre from index_reportings where id=%1").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qWarning()<<"Invalid reporting ID !";
        return 0;
    }
    return q.value(0).toUInt();
}




bool PCx_Reporting::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, QDate date, QMap<PCx_Reporting::OREDPCR,double> vals, bool fastMode)
{
    if(!fastMode)
    {
        if(!getAttachedTree()->isLeaf(leafId))
        {
            qWarning()<<"Not a leaf !";
            return false;
        }
    }

    QString reqSet="id_node,date,";
    QString reqBind=":id_node,:date,";
    QString reqUpdateSet;

    QStringList reqSetList,bindSetList,reqUpdateSetList;

    for(QMap<PCx_Reporting::OREDPCR,double>::const_iterator it=vals.constBegin(),end=vals.constEnd();it!=end;++it)
    {
        QString tblString=OREDPCRtoTableString(it.key());
        reqSetList.append(tblString);
        bindSetList.append(":v"+tblString);
        reqUpdateSetList.append(tblString+"=:v"+tblString);
    }
    reqSetList.sort();
    bindSetList.sort();
    reqUpdateSetList.sort();
    reqSet.append(reqSetList.join(","));
    reqBind.append(bindSetList.join(","));
    reqUpdateSet=reqUpdateSetList.join(",");

    QSqlQuery q;
    QString tableName=QString("reporting_%1_%2").arg(MODES::modeToTableString(mode)).arg(reportingId);

    //UPSERT simulation: INSERT the row if not existing, UPDATE if the row exists
    q.prepare(QString("insert or ignore into %1 (%2) values (%3)")
              .arg(tableName,reqSet,reqBind));


    for(QMap<PCx_Reporting::OREDPCR,double>::const_iterator it=vals.constBegin(),end=vals.constEnd();it!=end;++it)
    {
        PCx_Reporting::OREDPCR ored=it.key();
        QString tblString=OREDPCRtoTableString(ored);
        if(ored==PCx_Reporting::OREDPCR::DISPONIBLES)
        {
            qDebug()<<"DISPONIBLES will be recomputed and overwritten";
        }
        q.bindValue(":v"+tblString,doubleToFixedPoint(vals.value(ored)));
    }

    q.bindValue(":id_node",leafId);

    QDateTime dt(date);
    unsigned int sSinceEpoch=dt.toTime_t();

    q.bindValue(":date",sSinceEpoch);

    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    //INSERT has failed, row already exists so UPDATE the row
    if(q.numRowsAffected()<1)
    {
        q.prepare(QString("update %1 set %2 where id_node=:id_node and date=:date").arg(tableName,reqUpdateSet));
        for(QMap<PCx_Reporting::OREDPCR,double>::const_iterator it=vals.constBegin(),end=vals.constEnd();it!=end;++it)
        {
            PCx_Reporting::OREDPCR ored=it.key();

            QString tblString=OREDPCRtoTableString(ored);
            if(ored==PCx_Reporting::OREDPCR::DISPONIBLES)
            {
                qDebug()<<"DISPONIBLES will be recomputed and overwritten";
            }
            q.bindValue(":v"+tblString,doubleToFixedPoint(vals.value(ored)));
        }

        q.bindValue(":id_node",leafId);
        q.bindValue(":date",sSinceEpoch);
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

    q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) where id_node=:id_node and date=:date").arg(tableName));
    q.bindValue(":id_node",leafId);
    q.bindValue(":date",sSinceEpoch);
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

    updateParent(tableName,date,leafId);
    return true;
}

qint64 PCx_Reporting::getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, OREDPCR ored, QDate date) const
{
    QSqlQuery q;
    q.prepare(QString("select %1 from reporting_%2_%3 where date=:date and id_node=:node").arg(OREDPCRtoTableString(ored),modeToTableString(mode)).arg(reportingId));
    q.bindValue(":date",QDateTime(date).toTime_t());
    q.bindValue(":node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qDebug()<<"No value for node"<<nodeId;
        return -MAX_NUM;
    }
    if(q.value(OREDPCRtoTableString(ored)).isNull())
    {
        qDebug()<<"No value for node"<<nodeId;
        return -MAX_NUM;
    }
    return q.value(OREDPCRtoTableString(ored)).toLongLong();
}

void PCx_Reporting::clearAllData(MODES::DFRFDIRI mode)
{
    QSqlQuery q(QString("delete from reporting_%1_%2").arg(modeToTableString(mode)).arg(reportingId));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
}

void PCx_Reporting::updateParent(const QString &tableName, QDate date, unsigned int nodeId)
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

    q.prepare(QString("select * from %1 where id_node in(%2) and date=:date").arg(tableName,childrenString));
    unsigned int tDate=QDateTime(date).toTime_t();


    q.bindValue(":date",tDate);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    qint64 sumOuverts=0;
    qint64 sumRealises=0;
    qint64 sumEngages=0;
    qint64 sumDisponibles=0;
    qint64 sumBP=0;
    qint64 sumReports=0;
    qint64 sumOCDM=0;
    qint64 sumVCDM=0;
    qint64 sumBudgetVote=0;
    qint64 sumVCInternes=0;
    qint64 sumRattachesNMoins1=0;

    //To check if we need to insert 0.0 or NULL
    bool nullOuverts=true;
    bool nullEngages=true;
    bool nullRealises=true;
    bool nullDisponibles=true;
    bool nullBP=true;
    bool nullReports=true;
    bool nullOCDM=true;
    bool nullVCDM=true;
    bool nullBudgetVote=true;
    bool nullVCInternes=true;
    bool nullRattachesNMoins1=true;

    while(q.next())
    {
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
        if(!q.value("bp").isNull())
        {
            sumBP+=q.value("bp").toLongLong();
            nullBP=false;
        }
        if(!q.value("reports").isNull())
        {
            sumReports+=q.value("reports").toLongLong();
            nullReports=false;
        }
        if(!q.value("ocdm").isNull())
        {
            sumOCDM+=q.value("ocdm").toLongLong();
            nullOCDM=false;
        }
        if(!q.value("vcdm").isNull())
        {
            sumVCDM+=q.value("vcdm").toLongLong();
            nullVCDM=false;
        }
        if(!q.value("budgetvote").isNull())
        {
            sumBudgetVote+=q.value("budgetvote").toLongLong();
            nullBudgetVote=false;
        }
        if(!q.value("vcinterne").isNull())
        {
            sumVCInternes+=q.value("vcinterne").toLongLong();
            nullVCInternes=false;
        }
        if(!q.value("rattachenmoins1").isNull())
        {
            sumRattachesNMoins1+=q.value("rattachenmoins1").toLongLong();
            nullRattachesNMoins1=false;
        }
    }

    q.prepare(QString("insert into %1 (id_node,date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1) "
                      "values(:id_node,:date,:ouverts,:realises,:engages,:disponibles,:bp,:reports,:ocdm,:vcdm,:budgetvote,:vcinterne,"
                      ":rattachenmoins1)").arg(tableName));
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

    if(nullBP)
        q.bindValue(":bp",QVariant(QVariant::LongLong));
    else
        q.bindValue(":bp",sumBP);

    if(nullReports)
        q.bindValue(":reports",QVariant(QVariant::LongLong));
    else
        q.bindValue(":reports",sumReports);

    if(nullOCDM)
        q.bindValue(":ocdm",QVariant(QVariant::LongLong));
    else
        q.bindValue(":ocdm",sumOCDM);

    if(nullVCDM)
        q.bindValue(":vcdm",QVariant(QVariant::LongLong));
    else
        q.bindValue(":vcdm",sumVCDM);

    if(nullBudgetVote)
        q.bindValue(":budgetvote",QVariant(QVariant::LongLong));
    else
        q.bindValue(":budgetvote",sumBudgetVote);

    if(nullVCInternes)
        q.bindValue(":vcinterne",QVariant(QVariant::LongLong));
    else
        q.bindValue(":vcinterne",sumVCInternes);

    if(nullRattachesNMoins1)
        q.bindValue(":rattachenmoins1",QVariant(QVariant::LongLong));
    else
        q.bindValue(":rattachenmoins1",sumRattachesNMoins1);


    q.bindValue(":date",tDate);
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
        updateParent(tableName,date,parent);
    }
}



bool PCx_Reporting::importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode)
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
         xlsx.read(1,5).isValid() && xlsx.read(1,6).isValid() &&
         xlsx.read(1,7).isValid() && xlsx.read(1,8).isValid() &&
         xlsx.read(1,9).isValid() && xlsx.read(1,10).isValid() &&
         xlsx.read(1,11).isValid() && xlsx.read(1,12).isValid() &&
         xlsx.read(1,13).isValid() && xlsx.read(1,14).isValid()))
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide. Vous pouvez exporter un fichier squelette dans l'interface de gestion des reportings"));
        return false;
    }

    int rowCount=xlsx.dimension().rowCount();


    int row=2;
    if(rowCount<2)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide. Vous pouvez exporter un fichier squelette dans l'interface de gestion des reportings"));
        return false;
    }


    QProgressDialog progress(QObject::tr("Vérification en cours..."),QObject::tr("Annuler"),2,rowCount);
    progress.setMinimumDuration(200);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setValue(2);

    do
    {
        QVariant cellNodeId,nodeType,nodeName,date,ouverts,realises,engages,bp,reports,ocdm,vcdm,budgetvote,vcinternes,rattachesnmoins1;
        cellNodeId=xlsx.read(row,1);
        nodeType=xlsx.read(row,2);
        nodeName=xlsx.read(row,3);
        //Do not use read for date, always assume "1900" mode to ensure libreoffice compatibility
        if(xlsx.read(row,4).isNull())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, spécifiez une date").arg(row));
            return false;
        }
        date=xlsx.cellAt(row,4)->value();
        bp=xlsx.read(row,5);
        reports=xlsx.read(row,6);
        ocdm=xlsx.read(row,7);
        vcdm=xlsx.read(row,8);
        budgetvote=xlsx.read(row,9);
        vcinternes=xlsx.read(row,10);
        ouverts=xlsx.read(row,11);
        realises=xlsx.read(row,12);
        engages=xlsx.read(row,13);
        rattachesnmoins1=xlsx.read(row,14);


        if(!(cellNodeId.isValid() && nodeType.isValid() && nodeName.isValid() && date.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, remplissez l'identifiant, le type et le nom du noeud ainsi que la date d'application").arg(row));
            return false;
        }

        QPair<QString,QString> typeAndNode;
        typeAndNode.first=nodeType.toString().simplified();
        typeAndNode.second=nodeName.toString().simplified();
        if(typeAndNode.first.isEmpty() || typeAndNode.second.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud ne peuvent pas être vides (ni composés d'espaces)").arg(row));
            return false;

        }
        if(typeAndNode.first.size()>PCx_Tree::MAXNODENAMELENGTH || typeAndNode.second.size()>PCx_Tree::MAXNODENAMELENGTH)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud sont trop longs").arg(row));
            return false;
        }

        double dblOuv=ouverts.toDouble();
        double dblReal=realises.toDouble();
        double dblEng=engages.toDouble();
        double dblBP=bp.toDouble();
        double dblReports=reports.toDouble();
        double dblOCDM=ocdm.toDouble();
        double dblVCDM=vcdm.toDouble();
        double dblBudgetVote=budgetvote.toDouble();
        double dblVInternes=vcinternes.toDouble();
        double dblRattachesNMoins1=rattachesnmoins1.toDouble();

        if(dblOuv>MAX_NUM||dblReal>MAX_NUM||dblEng>MAX_NUM||dblBP>MAX_NUM||dblReports>MAX_NUM
                ||dblOCDM>MAX_NUM||dblVCDM>MAX_NUM||dblBudgetVote>MAX_NUM||dblVInternes>MAX_NUM
                ||dblRattachesNMoins1>MAX_NUM)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur trop grande ligne %1 (valeur maximale : %2) !").arg(row).arg(MAX_NUM));
            return false;
        }

        if(!qIsFinite(dblOuv)||!qIsFinite(dblReal)||!qIsFinite(dblEng)
                ||!qIsFinite(dblBP)||!qIsFinite(dblReports)||!qIsFinite(dblOCDM)
                ||!qIsFinite(dblVCDM)||!qIsFinite(dblBudgetVote)||!qIsFinite(dblVInternes)
                ||!qIsFinite(dblRattachesNMoins1))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur infinie ligne %1 non autorisée !").arg(row));
            return false;
        }

        if(dblOuv<0.0 || dblReal<0.0 || dblEng<0.0 || dblBP<0.0|| dblReports <0.0
                ||dblBudgetVote<0.0 || dblRattachesNMoins1<0.0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Valeur négative ligne %1 non autorisée !").arg(row));
            return false;
        }

        unsigned int nodeId=cellNodeId.toUInt();
        if(nodeId==0)
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("L'identifiant du noeud ligne %1 est invalide").arg(row));
            return false;
        }
        //nodeId=getAttachedTree()->getNodeIdFromTypeAndNodeName(typeAndNode);
        if(!getAttachedTree()->checkIdToTypeAndName(nodeId,typeAndNode.first,typeAndNode.second))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("L'identifiant du noeud ligne %1 ne correspond pas aux type et nom indiqués sur la même ligne").arg(row));
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

    QProgressDialog progress2(QObject::tr("Chargement des données en cours..."),QObject::tr("Annuler"),2,rowCount);

    progress2.setMinimumDuration(200);

    progress2.setWindowModality(Qt::ApplicationModal);

    progress2.setValue(2);

    do
    {
        QVariant cellNodeId,nodeType,nodeName,date,ouverts,realises,engages,bp,reports,ocdm,vcdm,budgetvote,vcinternes,rattachesnmoins1;
        cellNodeId=xlsx.read(row,1);
        nodeType=xlsx.read(row,2);
        nodeName=xlsx.read(row,3);
        //Do not use read for date, always assume "1900" mode to ensure libreoffice compatibility
        date=xlsx.cellAt(row,4)->value();
        bp=xlsx.read(row,5);
        reports=xlsx.read(row,6);
        ocdm=xlsx.read(row,7);
        vcdm=xlsx.read(row,8);
        budgetvote=xlsx.read(row,9);
        vcinternes=xlsx.read(row,10);
        ouverts=xlsx.read(row,11);
        realises=xlsx.read(row,12);
        engages=xlsx.read(row,13);
        rattachesnmoins1=xlsx.read(row,14);

        QDate laDate=QXlsx::datetimeFromNumber(date.toDouble()).date();

        unsigned int nodeId=cellNodeId.toUInt();
        if(nodeId==0)
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("L'identifiant du noeud ligne %1 est invalide").arg(row));
            return false;
        }


        QMap<PCx_Reporting::OREDPCR,double> vals;
        if(ouverts.isValid())
        {
            double valDbl=ouverts.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::OUVERTS,valDbl);
        }
        if(realises.isValid())
        {
            double valDbl=realises.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::REALISES,valDbl);
        }
        if(engages.isValid())
        {
            double valDbl=engages.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::ENGAGES,valDbl);
        }
        if(bp.isValid())
        {
            double valDbl=bp.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::BP,valDbl);
        }
        if(reports.isValid())
        {
            double valDbl=reports.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::REPORTS,valDbl);
        }
        if(ocdm.isValid())
        {
            double valDbl=ocdm.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::OCDM,valDbl);
        }
        if(vcdm.isValid())
        {
            double valDbl=vcdm.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::VCDM,valDbl);
        }
        if(budgetvote.isValid())
        {
            double valDbl=budgetvote.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::BUDGETVOTE,valDbl);
        }
        if(rattachesnmoins1.isValid())
        {
            double valDbl=rattachesnmoins1.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::RATTACHENMOINS1,valDbl);
        }
        if(vcinternes.isValid())
        {
            double valDbl=vcinternes.toDouble();
            vals.insert(PCx_Reporting::OREDPCR::VIREMENTSINTERNES,valDbl);
        }

        if(vals.size()>0)
        {
            //setLeafValues in fast mode only die on db error
            setLeafValues(nodeId,mode,laDate,vals,true);
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

bool PCx_Reporting::exportLeavesSkeleton(const QString &fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=getAttachedTree()->getLeavesId();
    xlsx.write(1,1,"Identifiant noeud");
    xlsx.write(1,2,"Type noeud");
    xlsx.write(1,3,"Nom noeud");
    xlsx.write(1,4,"Date");
    xlsx.write(1,5,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP));
    xlsx.write(1,6,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS));
    xlsx.write(1,7,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM));
    xlsx.write(1,8,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM));
    xlsx.write(1,9,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BUDGETVOTE));
    xlsx.write(1,10,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES));
    xlsx.write(1,11,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS));
    xlsx.write(1,12,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES));
    xlsx.write(1,13,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES));
    xlsx.write(1,14,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::RATTACHENMOINS1));

    int row=2;
    foreach(unsigned int leaf, leavesId)
    {
        QPair<QString,QString> typeAndNodeName=getAttachedTree()->getTypeNameAndNodeName(leaf);
        xlsx.write(row,1,leaf);
        xlsx.write(row,2,typeAndNodeName.first);
        xlsx.write(row,3,typeAndNodeName.second);
        //        xlsx.write(row,3,QDate(2002,05,13));
        row++;
    }

    return xlsx.saveAs(fileName);
}

QDate PCx_Reporting::getLastReportingDate(MODES::DFRFDIRI mode,unsigned int node) const
{
    QSqlQuery q;
    q.prepare(QString("select date from reporting_%1_%2 where id_node=:idnode order by date desc limit 1").arg(MODES::modeToTableString(mode)).arg(reportingId));
    q.bindValue(":idnode",node);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
        return QDate();

    return QDateTime::fromTime_t(q.value(0).toUInt()).date();
}

bool PCx_Reporting::deleteLastReportingDate(MODES::DFRFDIRI mode)
{
    QSqlQuery q;
    QDate date=getLastReportingDate(mode);
    q.prepare(QString("delete from reporting_%1_%2 where date=:date").arg(MODES::modeToTableString(mode)).arg(reportingId));
    q.bindValue(":date",QDateTime(date).toTime_t());
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()<1)
        return false;

    return true;
}


bool PCx_Reporting::addLastReportingDateToExistingAudit(PCx_Audit *audit) const
{
    if(audit==nullptr)
    {
        qDebug()<<"nullptr audit";
        return false;
    }
    if(audit->getAttachedTreeId()!=attachedTreeId)
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("L'audit et le reporting ne portent pas sur le même arbre !"));
        return false;
    }
    if(audit->isFinished())
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("L'audit est terminé ! Vous pouvez le rendre modifiable en le dé-terminant ou en le dupliquant."));
        return false;
    }

    QDate dateDF=getLastReportingDate(MODES::DFRFDIRI::DF);
    QDate dateRF=getLastReportingDate(MODES::DFRFDIRI::RF);
    QDate dateDI=getLastReportingDate(MODES::DFRFDIRI::DI);
    QDate dateRI=getLastReportingDate(MODES::DFRFDIRI::RI);

    int yearDF=dateDF.year();
    int yearRF=dateRF.year();
    int yearDI=dateDI.year();
    int yearRI=dateRI.year();

    int auditLastYear=audit->getYears().constLast();
    int auditFirstYear=audit->getYears().constFirst();
    if((yearDF>auditLastYear || yearDF<auditFirstYear) &&
            (yearRF>auditLastYear || yearRF<auditFirstYear) &&
            (yearDI>auditLastYear || yearDI<auditFirstYear) &&
            (yearRI>auditLastYear || yearRI<auditFirstYear))

    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Aucune année du reporting ne se retrouve dans l'audit. Vous devez tout d'abord étendre les années sur lesquelles porte l'audit en le dupliquant.\n"
                                                                          "L'audit porte sur %1 - %2\n"                                                                        )
                             .arg(auditFirstYear).arg(auditLastYear));

        return false;
    }

    QList<unsigned int> leaves=attachedTree->getLeavesId();

    bool doDF=false,doRF=false,doDI=false,doRI=false;
    if(yearDF<=auditLastYear && yearDF>=auditFirstYear)
        doDF=true;
    if(yearRF<=auditLastYear && yearRF>=auditFirstYear)
        doRF=true;
    if(yearDI<=auditLastYear && yearDI>=auditFirstYear)
        doDI=true;
    if(yearRI<=auditLastYear && yearRI>=auditFirstYear)
        doRI=true;

    int nbDo=(int)doDF+(int)doRF+(int)doDI+(int)doRI;
    int progressMax=nbDo*leaves.count();

    QProgressDialog progress(QObject::tr("Mise à jour des données de l'audit..."),QObject::tr("Annuler"),0,progressMax);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);
    int progressStep=0;

    QSqlDatabase::database().transaction();
    if(doDF)
    {
        foreach(unsigned int leaf,leaves)
        {
            QSqlQuery q;
            q.prepare(QString("select ouverts,realises,engages from reporting_DF_%1 where date=:date and id_node=:node").arg(reportingId));
            q.bindValue(":date",QDateTime(dateDF).toTime_t());
            q.bindValue(":node",leaf);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.next())
            {
                QMap<PCx_Audit::ORED,double> vals;
                vals.insert(PCx_Audit::ORED::OUVERTS,fixedPointToDouble(q.value("ouverts").toLongLong()));
                vals.insert(PCx_Audit::ORED::REALISES,fixedPointToDouble(q.value("realises").toLongLong()));
                vals.insert(PCx_Audit::ORED::ENGAGES,fixedPointToDouble(q.value("engages").toLongLong()));
                if(audit->setLeafValues(leaf,MODES::DFRFDIRI::DF,yearDF,vals)==false)
                {
                    QSqlDatabase::database().rollback();
                    return false;
                }
            }
            if(!progress.wasCanceled())
            {
                progress.setValue(++progressStep);
            }
            else
            {
                QSqlDatabase::database().rollback();
                return false;
            }
        }
    }

    if(doRF)
    {
        foreach(unsigned int leaf,leaves)
        {
            QSqlQuery q;
            q.prepare(QString("select ouverts,realises,engages from reporting_RF_%1 where date=:date and id_node=:node").arg(reportingId));
            q.bindValue(":date",QDateTime(dateRF).toTime_t());
            q.bindValue(":node",leaf);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.next())
            {
                QMap<PCx_Audit::ORED,double> vals;
                vals.insert(PCx_Audit::ORED::OUVERTS,fixedPointToDouble(q.value("ouverts").toLongLong()));
                vals.insert(PCx_Audit::ORED::REALISES,fixedPointToDouble(q.value("realises").toLongLong()));
                vals.insert(PCx_Audit::ORED::ENGAGES,fixedPointToDouble(q.value("engages").toLongLong()));
                if(audit->setLeafValues(leaf,MODES::DFRFDIRI::RF,yearRF,vals)==false)
                {
                    QSqlDatabase::database().rollback();
                    return false;
                }
            }

            if(!progress.wasCanceled())
            {
                progress.setValue(++progressStep);
            }
            else
            {
                QSqlDatabase::database().rollback();
                return false;
            }
        }

    }

    if(doDI)
    {
        foreach(unsigned int leaf,leaves)
        {
            QSqlQuery q;
            q.prepare(QString("select ouverts,realises,engages from reporting_DI_%1 where date=:date and id_node=:node").arg(reportingId));
            q.bindValue(":date",QDateTime(dateDI).toTime_t());
            q.bindValue(":node",leaf);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.next())
            {
                QMap<PCx_Audit::ORED,double> vals;
                vals.insert(PCx_Audit::ORED::OUVERTS,fixedPointToDouble(q.value("ouverts").toLongLong()));
                vals.insert(PCx_Audit::ORED::REALISES,fixedPointToDouble(q.value("realises").toLongLong()));
                vals.insert(PCx_Audit::ORED::ENGAGES,fixedPointToDouble(q.value("engages").toLongLong()));
                if(audit->setLeafValues(leaf,MODES::DFRFDIRI::DI,yearDI,vals)==false)
                {
                    QSqlDatabase::database().rollback();
                    return false;
                }
            }

            if(!progress.wasCanceled())
            {
                progress.setValue(++progressStep);
            }
            else
            {
                QSqlDatabase::database().rollback();
                return false;
            }
        }

    }

    if(doRI)
    {
        foreach(unsigned int leaf,leaves)
        {
            QSqlQuery q;
            q.prepare(QString("select ouverts,realises,engages from reporting_RI_%1 where date=:date and id_node=:node").arg(reportingId));
            q.bindValue(":date",QDateTime(dateRI).toTime_t());
            q.bindValue(":node",leaf);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.next())
            {
                QMap<PCx_Audit::ORED,double> vals;
                vals.insert(PCx_Audit::ORED::OUVERTS,fixedPointToDouble(q.value("ouverts").toLongLong()));
                vals.insert(PCx_Audit::ORED::REALISES,fixedPointToDouble(q.value("realises").toLongLong()));
                vals.insert(PCx_Audit::ORED::ENGAGES,fixedPointToDouble(q.value("engages").toLongLong()));
                if(audit->setLeafValues(leaf,MODES::DFRFDIRI::RI,yearRI,vals)==false)
                {
                    QSqlDatabase::database().rollback();
                    return false;
                }
            }

            if(!progress.wasCanceled())
            {
                progress.setValue(++progressStep);
            }
            else
            {
                QSqlDatabase::database().rollback();
                return false;
            }
        }

    }
    progress.setValue(progressMax);
    QSqlDatabase::database().commit();

    return true;
}

int PCx_Reporting::duplicateReporting(const QString &newName) const
{
    if(newName.isEmpty()||newName.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed, name out-of-bound");
    }
    int newReportingId=addNewReporting(newName,attachedTreeId);
    if(newReportingId<=0)
    {
        return newReportingId;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery q;
    q.exec(QString("insert into reporting_DF_%1 select * from reporting_DF_%2").arg(newReportingId).arg(reportingId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        PCx_Reporting::deleteReporting(newReportingId);
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("insert into reporting_RF_%1 select * from reporting_RF_%2").arg(newReportingId).arg(reportingId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        PCx_Reporting::deleteReporting(newReportingId);
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("insert into reporting_DI_%1 select * from reporting_DI_%2").arg(newReportingId).arg(reportingId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        PCx_Reporting::deleteReporting(newReportingId);
        qCritical()<<q.lastError();
        die();
    }
    q.exec(QString("insert into reporting_RI_%1 select * from reporting_RI_%2").arg(newReportingId).arg(reportingId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        PCx_Reporting::deleteReporting(newReportingId);
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return newReportingId;
}

bool PCx_Reporting::exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString & fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=getAttachedTree()->getLeavesId();
    xlsx.write(1,1,"Identifiant noeud");
    xlsx.write(1,2,"Type noeud");
    xlsx.write(1,3,"Nom noeud");
    xlsx.write(1,4,"Date");
    xlsx.write(1,5,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP));
    xlsx.write(1,6,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS));
    xlsx.write(1,7,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM));
    xlsx.write(1,8,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM));
    xlsx.write(1,9,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BUDGETVOTE));
    xlsx.write(1,10,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES));
    xlsx.write(1,11,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS));
    xlsx.write(1,12,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES));
    xlsx.write(1,13,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES));
    xlsx.write(1,14,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::RATTACHENMOINS1));

    QSqlQuery q;
    int row=2;
    foreach(unsigned int leafId,leavesId)
    {
        QPair<QString,QString> typeAndNodeName=getAttachedTree()->getTypeNameAndNodeName(leafId);
        q.prepare(QString("select * from reporting_%1_%2 where id_node=:idnode order by date").
                  arg(MODES::modeToTableString(mode)).arg(reportingId));

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
            QVariant valBp=q.value("bp");
            QVariant valReports=q.value("reports");
            QVariant valOCDM=q.value("ocdm");
            QVariant valVCDM=q.value("vcdm");
            QVariant valBudgetVote=q.value("budgetvote");
            QVariant valVCInternes=q.value("vcinterne");
            QVariant valRattaches=q.value("rattachenmoins1");

            xlsx.write(row,1,leafId);
            xlsx.write(row,2,typeAndNodeName.first);
            xlsx.write(row,3,typeAndNodeName.second);
            xlsx.write(row,4,QDateTime::fromTime_t(q.value("date").toUInt()).date());

            if(!valBp.isNull())
            {
                xlsx.write(row,5,fixedPointToDouble(valBp.toLongLong()));
            }
            if(!valReports.isNull())
            {
                xlsx.write(row,6,fixedPointToDouble(valReports.toLongLong()));
            }
            if(!valOCDM.isNull())
            {
                xlsx.write(row,7,fixedPointToDouble(valOCDM.toLongLong()));
            }
            if(!valVCDM.isNull())
            {
                xlsx.write(row,8,fixedPointToDouble(valVCDM.toLongLong()));
            }
            if(!valBudgetVote.isNull())
            {
                xlsx.write(row,9,fixedPointToDouble(valBudgetVote.toLongLong()));
            }
            if(!valVCInternes.isNull())
            {
                xlsx.write(row,10,fixedPointToDouble(valVCInternes.toLongLong()));
            }

            if(!valOuverts.isNull())
            {
                xlsx.write(row,11,fixedPointToDouble(valOuverts.toLongLong()));
            }

            if(!valRealises.isNull())
            {
                xlsx.write(row,12,fixedPointToDouble(valRealises.toLongLong()));
            }

            if(!valEngages.isNull())
            {
                xlsx.write(row,13,fixedPointToDouble(valEngages.toLongLong()));
            }


            if(!valRattaches.isNull())
            {
                xlsx.write(row,14,fixedPointToDouble(valRattaches.toLongLong()));
            }

            row++;
        }
    }

    return xlsx.saveAs(fileName);

}

QString PCx_Reporting::getCSS()
{
    QString css="\nbody{font-family:sans-serif;font-size:9pt;background-color:white;color:black;}"
                "\nh1{color:#A00;}"
                "\nh2{color:navy;}"
                "\nh3{color:green;font-size:larger}"
                "\n.reportingNodeContainer{}"
            ;

    css.append(PCx_Query::getCSS());
    css.append(PCx_Tables::getCSS());
    css.append(PCx_Graphics::getCSS());
    return css;
}


QString PCx_Reporting::generateHTMLHeader() const
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head><title>Reporting %1</title>\n<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>\n<style type='text/css'>\n%2\n</style>\n</head>\n<body>").arg(reportingName.toHtmlEscaped(),getCSS());
}

QString PCx_Reporting::generateHTMLReportingTitle() const
{
    return QString("<h3>Reporting %1, arbre %3</h3>").arg(reportingName.toHtmlEscaped(),attachedTreeName.toHtmlEscaped());
}

QSet<QDate> PCx_Reporting::getDatesForNodeAndMode( unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QSqlQuery q;
    QSet<QDate> out;
    q.prepare(QString("select date from reporting_%1_%2 where id_node=:idnode order by date").arg(MODES::modeToTableString(mode)).arg(reportingId));
    q.bindValue(":idnode",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        unsigned int rowDate=q.value(0).toUInt();
        out.insert(QDateTime::fromTime_t(rowDate).date());
    }
    return out;
}

bool PCx_Reporting::dateExistsForNodeAndMode(unsigned int timeT,unsigned int nodeId,MODES::DFRFDIRI mode) const
{
    return dateExistsForNodeAndMode(QDateTime::fromTime_t(timeT).date(),nodeId,mode);
}

void PCx_Reporting::addRandomDataForNext15(MODES::DFRFDIRI mode)
{
    QList<unsigned int> leaves=getAttachedTree()->getLeavesId();

    QMap<PCx_Reporting::OREDPCR,double> data;

    int maxVal=leaves.size();

    QProgressDialog progress(QObject::tr("Génération des données aléatoires..."),QObject::tr("Annuler"),0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);

    QSqlDatabase::database().transaction();

    int nbNode=0;
    qsrand(time(NULL));

    double randval;

    QDate lastDate=getLastReportingDate(mode);
    QDate nextDate;
    if(!lastDate.isNull())
        nextDate=lastDate.addDays(15);
    else
        nextDate=QDate(QDate::currentDate().year(),1,1);

    foreach(unsigned int leaf,leaves)
    {
        data.clear();

        for(int i=(int)PCx_Reporting::OREDPCR::OUVERTS;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
        {
            if(i==(int)PCx_Reporting::OREDPCR::DISPONIBLES)
                continue;
            randval=qrand()/(double)(RAND_MAX/(double)MAX_NUM*1000);

            data.insert((PCx_Reporting::OREDPCR)(i),randval);
        }

        //the transaction will be rollback in setLeafValues=>die
        setLeafValues(leaf,mode,nextDate,data,true);

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


bool PCx_Reporting::dateExistsForNodeAndMode(QDate date, unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QSqlQuery q;
    QDateTime dt(date);
    unsigned int dateToCheck=dt.toTime_t();
    q.prepare(QString("select count(*) from reporting_%1_%2 where date=:date and id_node=:idnode").arg(MODES::modeToTableString(mode)).arg(reportingId));
    q.bindValue(":date",dateToCheck);
    q.bindValue(":idnode",nodeId);
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
    if(q.value(0).toInt()>0)
        return true;
    return false;
}

void PCx_Reporting::OREDPCRToComboBox(QComboBox *combo)
{
    combo->clear();
    for(int i=(int)OREDPCR::OUVERTS;i<=(int)OREDPCR::NONELAST;i++)
        combo->addItem(OREDPCRtoCompleteString((OREDPCR)i),i);
}


QString PCx_Reporting::OREDPCRtoCompleteString(OREDPCR ored,bool capitalizeFirstLetter)
{
    if(capitalizeFirstLetter==false)
    {
        switch(ored)
        {
        case OREDPCR::OUVERTS:
            return QObject::tr("crédits ouverts");
        case OREDPCR::REALISES:
            return QObject::tr("réalisé");
        case OREDPCR::ENGAGES:
            return QObject::tr("engagé");
        case OREDPCR::DISPONIBLES:
            return QObject::tr("disponible");
        case OREDPCR::BP:
            return QObject::tr("BP");
        case OREDPCR::REPORTS:
            return QObject::tr("reports");
        case OREDPCR::OCDM:
            return QObject::tr("OCDM");
        case OREDPCR::VCDM:
            return QObject::tr("VCDM");
        case OREDPCR::BUDGETVOTE:
            return QObject::tr("budget voté");
        case OREDPCR::RATTACHENMOINS1:
            return QObject::tr("rattachés N-1");
        case OREDPCR::VIREMENTSINTERNES:
            return QObject::tr("v. internes");
        case OREDPCR::NONELAST:
            return QString();

        default:
            qWarning()<<"Invalid ORED specified !";
        }
    }

    else
    {

        switch(ored)
        {
        case OREDPCR::OUVERTS:
            return QObject::tr("Crédits ouverts");
        case OREDPCR::REALISES:
            return QObject::tr("Réalisé");
        case OREDPCR::ENGAGES:
            return QObject::tr("Engagé");
        case OREDPCR::DISPONIBLES:
            return QObject::tr("Disponible");
        case OREDPCR::BP:
            return QObject::tr("BP");
        case OREDPCR::REPORTS:
            return QObject::tr("Reports");
        case OREDPCR::OCDM:
            return QObject::tr("OCDM");
        case OREDPCR::VCDM:
            return QObject::tr("VCDM");
        case OREDPCR::BUDGETVOTE:
            return QObject::tr("Budget voté");
        case OREDPCR::RATTACHENMOINS1:
            return QObject::tr("Rattachés N-1");
        case OREDPCR::VIREMENTSINTERNES:
            return QObject::tr("V. internes");
        case OREDPCR::NONELAST:
            return QString();

        default:
            qWarning()<<"Invalid ORED specified !";
        }
    }
    return QString();
}

QString PCx_Reporting::OREDPCRtoTableString(OREDPCR ored)
{
    switch(ored)
    {
    case OREDPCR::OUVERTS:
        return "ouverts";
    case OREDPCR::REALISES:
        return "realises";
    case OREDPCR::ENGAGES:
        return "engages";
    case OREDPCR::DISPONIBLES:
        return "disponibles";
    case OREDPCR::BP:
        return "bp";
    case OREDPCR::REPORTS:
        return "reports";
    case OREDPCR::OCDM:
        return "ocdm";
    case OREDPCR::VCDM:
        return "vcdm";
    case OREDPCR::BUDGETVOTE:
        return "budgetvote";
    case OREDPCR::RATTACHENMOINS1:
        return "rattachenmoins1";
    case OREDPCR::VIREMENTSINTERNES:
        return "vcinterne";

    default:
        qWarning()<<"Invalid ORED specified !";
    }
    return QString();
}

PCx_Reporting::OREDPCR PCx_Reporting::OREDPCRFromTableString(const QString &ored)
{
    if(ored==OREDPCRtoTableString(OREDPCR::OUVERTS))
        return OREDPCR::OUVERTS;
    if(ored==OREDPCRtoTableString(OREDPCR::REALISES))
        return OREDPCR::REALISES;
    if(ored==OREDPCRtoTableString(OREDPCR::ENGAGES))
        return OREDPCR::ENGAGES;
    if(ored==OREDPCRtoTableString(OREDPCR::DISPONIBLES))
        return OREDPCR::DISPONIBLES;

    if(ored==OREDPCRtoTableString(OREDPCR::BP))
        return OREDPCR::BP;
    if(ored==OREDPCRtoTableString(OREDPCR::REPORTS))
        return OREDPCR::REPORTS;
    if(ored==OREDPCRtoTableString(OREDPCR::OCDM))
        return OREDPCR::OCDM;
    if(ored==OREDPCRtoTableString(OREDPCR::VCDM))
        return OREDPCR::VCDM;
    if(ored==OREDPCRtoTableString(OREDPCR::VIREMENTSINTERNES))
        return OREDPCR::VIREMENTSINTERNES;
    if(ored==OREDPCRtoTableString(OREDPCR::RATTACHENMOINS1))
        return OREDPCR::RATTACHENMOINS1;
    if(ored==OREDPCRtoTableString(OREDPCR::BUDGETVOTE))
        return OREDPCR::BUDGETVOTE;

    qWarning()<<"Invalid ORED string"<<ored<<"specified, defaulting to NONE";
    return OREDPCR::NONELAST;
}

unsigned int PCx_Reporting::addNewReporting(const QString &name, unsigned int attachedTreeId)
{
    if(name.isEmpty() || attachedTreeId==0|| name.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }

    QList<unsigned int> treeIds=PCx_Tree::getListOfTreesId(true);
    if(!treeIds.contains(attachedTreeId))
    {
        qFatal("Assertion failed");
    }

    //Must be checked in UI
    if(reportingNameExists(name))
    {
        qFatal("Assertion failed");
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
            qCritical()<<"Trying to create a reporting on a non-existant tree !";
            die();
        }
    }

    QSqlDatabase::database().transaction();
    q.prepare("insert into index_reportings (nom,id_arbre) values (:nom,:id_arbre)");
    q.bindValue(":nom",name);
    q.bindValue(":id_arbre",attachedTreeId);
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

    if(!q.exec(QString("create table reporting_DF_%1(id integer primary key autoincrement,"
                       " id_node integer not null references arbre_%2(id) on delete restrict, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_reporting_DF_%1 on reporting_DF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table reporting_RF_%1(id integer primary key autoincrement,"
                       " id_node integer not null references arbre_%2(id) on delete restrict, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create index idx_idnode_reporting_RF_%1 on reporting_RF_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table reporting_DI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_reporting_DI_%1 on reporting_DI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(!q.exec(QString("create table reporting_RI_%1(id integer primary key autoincrement, "
                       "id_node integer not null references arbre_%2(id) on delete restrict, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId).arg(attachedTreeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("create index idx_idnode_reporting_RI_%1 on reporting_RI_%1(id_node)").arg(uLastId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().commit();

    return uLastId;
}

bool PCx_Reporting::deleteReporting(unsigned int reportingId)
{
    if(reportingId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q(QString("select count(*) from index_reportings where id='%1'").arg(reportingId));
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()==0)
        {
            qWarning()<<"Attempting to delete an inexistant reporting !";
            return false;
        }
    }

    QSqlDatabase::database().transaction();
    if(!q.exec(QString("delete from index_reportings where id='%1'").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table reporting_DF_%1").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table reporting_RF_%1").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table reporting_DI_%1").arg(reportingId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.exec(QString("drop table reporting_RI_%1").arg(reportingId)))
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


bool PCx_Reporting::reportingNameExists(const QString &reportingName)
{
    QSqlQuery q;
    q.prepare("select count(*) from index_reportings where nom=:name");
    q.bindValue(":name",reportingName);
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

QList<QPair<unsigned int, QString> > PCx_Reporting::getListOfReportings()
{
    QList<QPair<unsigned int,QString> > listOfReportings;
    QDateTime dt;

    QSqlQuery query;
    if(!query.exec("select * from index_reportings order by datetime(le_timestamp)"))
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
        item=QString("%1 - %2").arg(query.value("nom").toString(),dtLocal.toString(Qt::SystemLocaleShortDate));
        p.first=query.value("id").toUInt();
        p.second=item;
        listOfReportings.append(p);
    }

    return listOfReportings;
}
