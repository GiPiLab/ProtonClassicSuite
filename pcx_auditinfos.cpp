#include "pcx_auditinfos.h"
#include "pcx_treemodel.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSet>
#include <QDebug>
#include <QSqlError>

PCx_AuditInfos::PCx_AuditInfos(unsigned int auditId)
{
    Q_ASSERT(auditId>0);
    this->auditId=auditId;
    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        name=q.value("nom").toString();
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
 }





QString PCx_AuditInfos::getHTMLAuditStatistics() const
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



    foreach(unsigned int year,years)
    {
        out.append(QString("\n<tr><th colspan='5'>%1</th></tr>\n"
                           "<tr><td>Noeuds contenant au moins une valeur (même zéro)</td><td align='right'>%2</td><td align='right'>%3</td><td  align='right'>%4</td><td  align='right'>%5</td></tr>")
                   .arg(year)
                   .arg(getNodesWithNonNullValues(PCx_AuditModel::DF,year).size())
                   .arg(getNodesWithNonNullValues(PCx_AuditModel::RF,year).size())
                   .arg(getNodesWithNonNullValues(PCx_AuditModel::DI,year).size())
                   .arg(getNodesWithNonNullValues(PCx_AuditModel::RI,year).size()));

        out.append(QString("<tr><td>Noeuds dont les valeurs sont toutes à zéro</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(getNodesWithAllZeroValues(PCx_AuditModel::DF,year).size())
                   .arg(getNodesWithAllZeroValues(PCx_AuditModel::RF,year).size())
                   .arg(getNodesWithAllZeroValues(PCx_AuditModel::DI,year).size())
                   .arg(getNodesWithAllZeroValues(PCx_AuditModel::RI,year).size()));

        out.append(QString("<tr><td>Noeuds non remplis</td><td align='right'>%1</td><td align='right'>%2</td><td align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(getNodesWithAllNullValues(PCx_AuditModel::DF,year).size())
                   .arg(getNodesWithAllNullValues(PCx_AuditModel::RF,year).size())
                   .arg(getNodesWithAllNullValues(PCx_AuditModel::DI,year).size())
                   .arg(getNodesWithAllNullValues(PCx_AuditModel::RI,year).size()));
    }

    out.append("</table>\n");

    return out;

}

QList<unsigned int> PCx_AuditInfos::getNodesWithAllNullValues(PCx_AuditModel::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_AuditModel::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::OUVERTS)<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::REALISES)
              <<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::ENGAGES);
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


QList<unsigned int> PCx_AuditInfos::getNodesWithNonNullValues(PCx_AuditModel::DFRFDIRI mode,unsigned int year) const
{
    QString tableMode=PCx_AuditModel::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::OUVERTS)<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::REALISES)
                 <<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::ENGAGES);
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

QList<unsigned int> PCx_AuditInfos::getNodesWithAllZeroValues(PCx_AuditModel::DFRFDIRI mode, unsigned int year) const
{
    QString tableMode=PCx_AuditModel::modeToTableString(mode);
    Q_ASSERT(year>=years.first()&& year<=years.last());
    QList<unsigned int> nodes;
    QStringList oredStrings;
    oredStrings<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::OUVERTS)<<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::REALISES)
                 <<PCx_AuditModel::OREDtoTableString(PCx_AuditModel::ENGAGES);
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



QStringList PCx_AuditInfos::yearsListToStringList(QList<unsigned int> years)
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
