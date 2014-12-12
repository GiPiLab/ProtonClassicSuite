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
    Q_ASSERT(reportingId>0);

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




bool PCx_Reporting::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, QDate date, QHash<PCx_Reporting::OREDPCR,double> vals, bool fastMode)
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
    QStringList reqSetList,bindSetList;
    foreach(PCx_Reporting::OREDPCR ored,vals.keys())
    {
        QString tblString=OREDPCRtoTableString(ored);
        reqSetList.append(tblString);
        bindSetList.append(":v"+tblString);
    }
    reqSetList.sort();
    bindSetList.sort();
    reqSet.append(reqSetList.join(","));
    reqBind.append(bindSetList.join(","));

    QSqlQuery q;
    QString tableName=QString("reporting_%1_%2").arg(MODES::modeToTableString(mode)).arg(reportingId);

    q.prepare(QString("insert into %1 (%2) values (%3)")
              .arg(tableName).arg(reqSet).arg(reqBind));

    foreach(PCx_Reporting::OREDPCR ored,vals.keys())
    {
        QString tblString=OREDPCRtoTableString(ored);
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

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
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
    q.prepare(QString("select %1 from reporting_%2_%3 where date=:date and id_node=:node").arg(OREDPCRtoTableString(ored)).arg(modeToTableString(mode)).arg(reportingId));
    q.bindValue(":date",QDateTime(date).toTime_t());
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
    if(q.value(OREDPCRtoTableString(ored)).isNull())
    {
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

    q.prepare(QString("select * from %1 where id_node in(%2) and date=:date").arg(tableName).arg(childrenString));
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
    Q_ASSERT(!fileName.isEmpty());
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
         xlsx.read(1,13).isValid()))
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
        QVariant nodeType,nodeName,date,ouverts,realises,engages,bp,reports,ocdm,vcdm,budgetvote,vcinternes,rattachesnmoins1;
        nodeType=xlsx.read(row,1);
        nodeName=xlsx.read(row,2);
        //Do not use read for date, always assume "1900" mode to ensure libreoffice compatibility
        date=xlsx.cellAt(row,3)->value();
        bp=xlsx.read(row,4);
        reports=xlsx.read(row,5);
        ocdm=xlsx.read(row,6);
        vcdm=xlsx.read(row,7);
        budgetvote=xlsx.read(row,8);
        vcinternes=xlsx.read(row,9);
        ouverts=xlsx.read(row,10);
        realises=xlsx.read(row,11);
        engages=xlsx.read(row,12);
        rattachesnmoins1=xlsx.read(row,13);


        if(!(nodeType.isValid() && nodeName.isValid() && date.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, remplissez le type et le nom du noeud ainsi que la date d'application").arg(row));
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

        if(dblOuv>=MAX_NUM||dblReal>=MAX_NUM||dblEng>=MAX_NUM||dblBP>=MAX_NUM||dblReports>=MAX_NUM
                ||dblOCDM>=MAX_NUM||dblVCDM>=MAX_NUM||dblBudgetVote>=MAX_NUM||dblVInternes>=MAX_NUM
                ||dblRattachesNMoins1>=MAX_NUM)
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

    QProgressDialog progress2(QObject::tr("Chargement des données en cours..."),QObject::tr("Annuler"),2,rowCount);

    progress2.setMinimumDuration(200);

    progress2.setWindowModality(Qt::ApplicationModal);

    progress2.setValue(2);

    do
    {
        QVariant nodeType,nodeName,date,ouverts,realises,engages,bp,reports,ocdm,vcdm,budgetvote,vcinternes,rattachesnmoins1;
        nodeType=xlsx.read(row,1);
        nodeName=xlsx.read(row,2);
        //Do not use read for date, always assume "1900" mode to ensure libreoffice compatibility
        date=xlsx.cellAt(row,3)->value();
        bp=xlsx.read(row,4);
        reports=xlsx.read(row,5);
        ocdm=xlsx.read(row,6);
        vcdm=xlsx.read(row,7);
        budgetvote=xlsx.read(row,8);
        vcinternes=xlsx.read(row,9);
        ouverts=xlsx.read(row,10);
        realises=xlsx.read(row,11);
        engages=xlsx.read(row,12);
        rattachesnmoins1=xlsx.read(row,13);

        QDate laDate=QXlsx::datetimeFromNumber(date.toDouble()).date();

        QPair<QString,QString> typeAndNode;
        typeAndNode.first=nodeType.toString().simplified();
        typeAndNode.second=nodeName.toString().simplified();
        int nodeId;
        nodeId=getAttachedTree()->getNodeIdFromTypeAndNodeName(typeAndNode);

        QHash<PCx_Reporting::OREDPCR,double> vals;
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
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Date");
    xlsx.write(1,4,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP));
    xlsx.write(1,5,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS));
    xlsx.write(1,6,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM));
    xlsx.write(1,7,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM));
    xlsx.write(1,8,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BUDGETVOTE));
    xlsx.write(1,9,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES));
    xlsx.write(1,10,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS));
    xlsx.write(1,11,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES));
    xlsx.write(1,12,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES));
    xlsx.write(1,13,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::RATTACHENMOINS1));

    int row=2;
    foreach(unsigned int leaf, leavesId)
    {
        QPair<QString,QString> typeAndNodeName=getAttachedTree()->getTypeNameAndNodeName(leaf);
        xlsx.write(row,1,typeAndNodeName.first);
        xlsx.write(row,2,typeAndNodeName.second);
//        xlsx.write(row,3,QDate(2002,05,13));
        row++;
    }

    return xlsx.saveAs(fileName);
}

QDate PCx_Reporting::getLastReportingDate(unsigned int node, MODES::DFRFDIRI mode) const
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

bool PCx_Reporting::exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString & fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=getAttachedTree()->getLeavesId();
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Date");
    xlsx.write(1,4,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP));
    xlsx.write(1,5,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS));
    xlsx.write(1,6,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM));
    xlsx.write(1,7,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM));
    xlsx.write(1,8,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BUDGETVOTE));
    xlsx.write(1,9,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES));
    xlsx.write(1,10,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS));
    xlsx.write(1,11,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES));
    xlsx.write(1,12,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES));
    xlsx.write(1,13,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::RATTACHENMOINS1));

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

                xlsx.write(row,1,typeAndNodeName.first);
                xlsx.write(row,2,typeAndNodeName.second);
                xlsx.write(row,3,QDateTime::fromTime_t(q.value("date").toUInt()).date());

                if(!valBp.isNull())
                {
                    xlsx.write(row,4,fixedPointToDouble(valBp.toLongLong()));
                }
                if(!valReports.isNull())
                {
                    xlsx.write(row,5,fixedPointToDouble(valReports.toLongLong()));
                }
                if(!valOCDM.isNull())
                {
                    xlsx.write(row,6,fixedPointToDouble(valOCDM.toLongLong()));
                }
                if(!valVCDM.isNull())
                {
                    xlsx.write(row,7,fixedPointToDouble(valVCDM.toLongLong()));
                }
                if(!valBudgetVote.isNull())
                {
                    xlsx.write(row,8,fixedPointToDouble(valBudgetVote.toLongLong()));
                }
                if(!valVCInternes.isNull())
                {
                    xlsx.write(row,9,fixedPointToDouble(valVCInternes.toLongLong()));
                }

                if(!valOuverts.isNull())
                {
                    xlsx.write(row,10,fixedPointToDouble(valOuverts.toLongLong()));
                }

                if(!valRealises.isNull())
                {
                    xlsx.write(row,11,fixedPointToDouble(valRealises.toLongLong()));
                }

                if(!valEngages.isNull())
                {
                    xlsx.write(row,12,fixedPointToDouble(valEngages.toLongLong()));
                }


                if(!valRattaches.isNull())
                {
                    xlsx.write(row,13,fixedPointToDouble(valRattaches.toLongLong()));
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
            "\nh3{color:green;font-size:larger}";

    css.append(PCx_Query::getCSS());
    css.append(PCx_Tables::getCSS());
    css.append(PCx_Graphics::getCSS());
    return css;
}


QString PCx_Reporting::generateHTMLHeader() const
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head><title>Reporting %1</title>\n<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>\n<style type='text/css'>\n%2\n</style>\n</head>\n<body>").arg(reportingName.toHtmlEscaped()).arg(getCSS());
}

QString PCx_Reporting::generateHTMLReportingTitle() const
{
    return QString("<h3>Reporting %1, arbre %3</h3>").arg(reportingName.toHtmlEscaped()).arg(attachedTreeName.toHtmlEscaped());
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

    QHash<PCx_Reporting::OREDPCR,double> data;

    int maxVal=leaves.size();

    QProgressDialog progress(QObject::tr("Génération des données aléatoires..."),QObject::tr("Annuler"),0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);

    QSqlDatabase::database().transaction();

    int nbNode=0;
    qsrand(time(NULL));

    double randval;

    QDate lastDate=getLastReportingDate(1,mode);
    QDate nextDate;
    if(!lastDate.isNull())
        nextDate=lastDate.addDays(15);
    else
        nextDate=QDate(2013,1,1);

    foreach(unsigned int leaf,leaves)
    {
            data.clear();

            for(int i=PCx_Reporting::OREDPCR::OUVERTS;i<PCx_Reporting::OREDPCR::NONELAST;i++)
            {
                if(i==PCx_Reporting::OREDPCR::DISPONIBLES)
                    continue;
                randval=qrand()/(double)(RAND_MAX/(double)MAX_NUM*1000);

                data.insert(PCx_Reporting::OREDPCR(i),randval);
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
        combo->addItem(OREDPCRtoCompleteString((OREDPCR)i),(OREDPCR)i);
}


QString PCx_Reporting::OREDPCRtoCompleteString(OREDPCR ored,bool capitalizeFirstLetter)
{
    if(capitalizeFirstLetter==false)
    {
        switch(ored)
        {
        case OREDPCR::OUVERTS:
            return QObject::tr("prévu");
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
            return QObject::tr("Prévu");
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
    Q_ASSERT(!name.isEmpty() && attachedTreeId>0);

    QList<unsigned int> treeIds=PCx_Tree::getListOfTreesId(true);
    if(!treeIds.contains(attachedTreeId))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Impossible de construire un reporting sur un arbre non existant ou non terminé"));
        return 0;
    }

    if(reportingNameExists(name))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un reporting portant ce nom !"));
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

    if(!q.exec(QString("create table reporting_DF_%1(id integer primary key autoincrement, id_node integer not null, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_RF_%1(id integer primary key autoincrement, id_node integer not null, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_DI_%1(id integer primary key autoincrement, id_node integer not null, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_RI_%1(id integer primary key autoincrement, id_node integer not null, date integer not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer, unique(id_node,date) on conflict replace)").arg(uLastId)))
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
    Q_ASSERT(reportingId>0);
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
        item=QString("%1 - %2").arg(query.value("nom").toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
        p.first=query.value("id").toUInt();
        p.second=item;
        listOfReportings.append(p);
    }

    return listOfReportings;
}
