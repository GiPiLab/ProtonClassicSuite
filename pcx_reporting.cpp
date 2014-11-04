#include "pcx_reporting.h"
#include "pcx_audit.h"
#include "pcx_report.h"
#include "xlsxdocument.h"
#include "pcx_query.h"
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

using namespace NUMBERSFORMAT;

PCx_Reporting::PCx_Reporting(unsigned int reportingId) :
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
    attachedTree=new PCx_Tree(attachedTreeId);
    attachedTreeName=attachedTree->getName();

}

PCx_Reporting::~PCx_Reporting()
{
    delete attachedTree;
}

unsigned int PCx_Reporting::getAttachedTreeId(unsigned int auditId)
{
        QSqlQuery q;
        if(!q.exec(QString("select id_arbre from index_reportings where id=%1").arg(auditId)))
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




bool PCx_Reporting::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, unsigned int year, QHash<PCx_Reporting::OREDPCR,double> vals, bool fastMode)
{
    if(!fastMode)
    {
        if(!attachedTree->isLeaf(leafId))
        {
            qWarning()<<"Not a leaf !";
            return false;
        }
    }

    QString reqString;

    if(vals.contains(PCx_Reporting::OREDPCR::OUVERTS))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("ouverts=:vouverts");
    }
    if(vals.contains(PCx_Reporting::OREDPCR::REALISES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("realises=:vrealises");
    }
    if(vals.contains(PCx_Reporting::OREDPCR::ENGAGES))
    {
        if(!reqString.isEmpty())
            reqString.append(",");
        reqString.append("engages=:vengages");
    }

    QSqlQuery q;
    QString tableName=QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(reportingId);

    //qDebug()<<"Request String ="<<reqString;

    q.prepare(QString("update %1 set %2 where id_node=:id_node and annee=:year")
              .arg(tableName).arg(reqString));

    if(vals.contains(PCx_Reporting::OREDPCR::OUVERTS))
    {
        q.bindValue(":vouverts",doubleToFixedPoint(vals.value(PCx_Reporting::OREDPCR::OUVERTS)));
    }

    if(vals.contains(PCx_Reporting::OREDPCR::REALISES))
    {
        q.bindValue(":vrealises",doubleToFixedPoint(vals.value(PCx_Reporting::OREDPCR::REALISES)));
    }

    if(vals.contains(PCx_Reporting::OREDPCR::ENGAGES))
    {
        q.bindValue(":vengages",doubleToFixedPoint(vals.value(PCx_Reporting::OREDPCR::ENGAGES)));
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

qint64 PCx_Reporting::getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, OREDPCR ored, unsigned int year) const
{
    QSqlQuery q;
    q.prepare(QString("select %1 from audit_%2_%3 where annee=:year and id_node=:node").arg(OREDPCRtoTableString(ored)).arg(modeToTableString(mode)).arg(reportingId));
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
    if(q.value(OREDPCRtoTableString(ored)).isNull())
    {
        return -MAX_NUM;
    }
    return q.value(OREDPCRtoTableString(ored)).toLongLong();
}

QHash<PCx_Reporting::OREDPCR, qint64> PCx_Reporting::getNodeValues(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year) const
{
    QSqlQuery q;
    QHash<PCx_Reporting::OREDPCR,qint64> output;
    q.prepare(QString("select * from audit_%2_%3 where annee=:year and id_node=:node").arg(modeToTableString(mode)).arg(reportingId));
    q.bindValue(":year",year);
    q.bindValue(":node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qWarning()<<"Missing node values for audit"<<reportingId<<", node"<<nodeId<<"and"<<modeToCompleteString(mode);
        return output;
    }


    if(q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)).isNull())
    {
        output.insert(PCx_Reporting::OREDPCR::OUVERTS,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_Reporting::OREDPCR::OUVERTS,q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)).toLongLong());
    }

    if(q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)).isNull())
    {
        output.insert(PCx_Reporting::OREDPCR::REALISES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_Reporting::OREDPCR::REALISES,q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)).toLongLong());
    }

    if(q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES)).isNull())
    {
        output.insert(PCx_Reporting::OREDPCR::ENGAGES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_Reporting::OREDPCR::ENGAGES,q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES)).toLongLong());
    }

    if(q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::DISPONIBLES)).isNull())
    {
        output.insert(PCx_Reporting::OREDPCR::DISPONIBLES,-MAX_NUM);
    }
    else
    {
        output.insert(PCx_Reporting::OREDPCR::DISPONIBLES,q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::DISPONIBLES)).toLongLong());
    }

    return output;
}

void PCx_Reporting::clearAllData(MODES::DFRFDIRI mode)
{
    QSqlQuery q(QString("update audit_%1_%2 set ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL").arg(modeToTableString(mode)).arg(reportingId));
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

int PCx_Reporting::duplicateReporting(const QString &newName, bool copyDF, bool copyRF, bool copyDI, bool copyRI) const
{
    /*
    Q_ASSERT(!newName.isEmpty());
    if(PCx_Reporting::reportingNameExists(newName))
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


    QProgressDialog progress(QObject::tr("Données en cours de recopie..."),QObject::tr("Annuler"),0,modes.size()*4+2);
    progress.setMinimumDuration(600);
    progress.setCancelButton(0);

    progress.setWindowModality(Qt::ApplicationModal);

    int progval=0;

    progress.setValue(0);

    unsigned int newReportingId=PCx_Reporting::addNewReporting(newName,attachedTreeId);

    Q_ASSERT(newReportingId>0);


    QSqlDatabase::database().transaction();

    QSqlQuery q;

    progress.setValue(++progval);


    foreach(QString lemode,modes)
    {
        q.prepare(QString("update audit_%3_%1 set ouverts="
                          "(select ouverts from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newReportingId).arg(reportingId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set realises="
                          "(select realises from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newReportingId).arg(reportingId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();

        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set engages="
                          "(select engages from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newReportingId).arg(reportingId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }


        progress.setValue(++progval);

        q.prepare(QString("update audit_%3_%1 set disponibles="
                          "(select disponibles from audit_%3_%2 where audit_%3_%2.id_node=audit_%3_%1.id_node "
                          "and audit_%3_%2.annee=audit_%3_%1.annee and audit_%3_%2.annee>=:year1 "
                          "and audit_%3_%2.annee<=:year2)").arg(newReportingId).arg(reportingId).arg(lemode));
        q.bindValue(":year1",year1);
        q.bindValue(":year2",year2);
        if(!q.exec())
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()<0)
        {
            QSqlDatabase::database().rollback();
            PCx_Reporting::deleteReporting(newReportingId);
            qCritical()<<q.lastError();
            die();
        }

        progress.setValue(++progval);
    }

    progress.setValue(progress.maximum());
    q.exec(QString("insert into audit_queries_%1 select * from audit_queries_%2").arg(newReportingId).arg(reportingId));
    if(q.numRowsAffected()<0)
    {
        QSqlDatabase::database().rollback();
        PCx_Reporting::deleteReporting(newReportingId);
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().commit();
    return newReportingId;
    */

}


void PCx_Reporting::updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId)
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


QList<unsigned int> PCx_Reporting::getNodesWithAllNullValues(MODES::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)
              <<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)
              <<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 is null and %4 is null and %5 is null) and annee=:year").arg(tableMode).arg(reportingId)
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


QList<unsigned int> PCx_Reporting::getNodesWithNonNullValues(MODES::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)
              <<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)
                 <<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES);
    QSqlQuery q;
    q.prepare(QString("select id_node from audit_%1_%2 where (%3 not null or %4 not null or %5 not null) and annee=:year").arg(tableMode).arg(reportingId)
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

QList<unsigned int> PCx_Reporting::getNodesWithAllZeroValues(MODES::DFRFDIRI mode, unsigned int year) const
{
    QString tableMode=MODES::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)<<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)
                 <<PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES);
    QSqlQuery q;
    QString sq=QString("select id_node from audit_%1_%2 where (%3 = 0 and %4 = 0 and %5 = 0) and annee=:year").arg(tableMode).arg(reportingId)
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

bool PCx_Reporting::importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode)
{
    Q_ASSERT(!fileName.isEmpty());
    QElapsedTimer timer;
    timer.start();

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
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur de format ligne %1, le type et le nom du noeud ne peuvent pas être vides (ni composés d'espaces)").arg(row));
            return false;

        }


        double dblOuv=ouverts.toDouble();
        double dblReal=realises.toDouble();
        double dblEng=engages.toDouble();


        if(dblOuv>=MAX_NUM||dblReal>=MAX_NUM||dblEng>=MAX_NUM)
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
        nodeId=attachedTree->getNodeIdFromTypeAndNodeName(typeAndNode);
        if(nodeId<=0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Noeud introuvable ligne %1").arg(row));
            return false;
        }

        bool leaf=attachedTree->isLeaf(nodeId);

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
        nodeId=attachedTree->getNodeIdFromTypeAndNodeName(typeAndNode);

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
    qDebug()<<"Import done in "<<timer.elapsed()<<" ms";
    return true;

}

bool PCx_Reporting::exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString & fileName) const
{
    QXlsx::Document xlsx;
    QList<unsigned int> leavesId=attachedTree->getLeavesId();
    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Année");
    xlsx.write(1,4,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS));
    xlsx.write(1,5,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES));
    xlsx.write(1,6,PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES));

    QSqlQuery q;
    int row=2;
    foreach(unsigned int leafId,leavesId)
    {
        QPair<QString,QString> typeAndNodeName=attachedTree->getTypeNameAndNodeName(leafId);
            q.prepare(QString("select * from audit_%1_%2 where id_node=:idnode order by annee").
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
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head><title>Audit %1</title>\n<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>\n<style type='text/css'>\n%2\n</style>\n</head>\n<body>").arg(reportingName.toHtmlEscaped()).arg(getCSS());
}

QString PCx_Reporting::generateHTMLReportingTitle() const
{
    return QString("<h3>Reporting %1, arbre %3</h3>").arg(reportingName.toHtmlEscaped()).arg(attachedTreeName.toHtmlEscaped());
}


QString PCx_Reporting::OREDPCRtoCompleteString(OREDPCR ored)
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
    default:
        qWarning()<<"Invalid ORED specified !";
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

    qWarning()<<"Invalid ORED string specified, defaulting to ouverts";
    return OREDPCR::OUVERTS;
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

    //Data are integer for fixed points arithmetics, stored with 3 decimals
    if(!q.exec(QString("create table reporting_DF_%1(id integer primary key autoincrement, id_node integer not null, date text not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_RF_%1(id integer primary key autoincrement, id_node integer not null, date text not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_DI_%1(id integer primary key autoincrement, id_node integer not null, date text not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer)").arg(uLastId)))
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

    if(!q.exec(QString("create table reporting_RI_%1(id integer primary key autoincrement, id_node integer not null, date text not null, "
                       "ouverts integer, realises integer, engages integer, disponibles integer, "
                       "bp integer, reports integer, ocdm integer, vcdm integer, budgetvote integer, "
                       "vcinterne integer, rattachenmoins1 integer)").arg(uLastId)))
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
            qWarning()<<"Attempting to delete an inexistant audit !";
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




