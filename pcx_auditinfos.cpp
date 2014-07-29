#include "pcx_auditinfos.h"
#include "pcx_treemodel.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSet>
#include <QDebug>
#include <QSqlError>

PCx_AuditInfos::PCx_AuditInfos(unsigned int auditId)
{
    updateInfos(auditId);
}

bool PCx_AuditInfos::updateInfos(unsigned int auditId)
{
    Q_ASSERT(auditId>0);

    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        auditId=auditId;
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
        creationTimeUTC=QDateTime::fromString(q.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
        return true;
    }
    else
    {
        qCritical()<<q.lastError();
    }
    return false;
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
