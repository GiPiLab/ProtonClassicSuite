#include "pcx_auditinfos.h"
#include "pcx_treemodel.h"
#include "utils.h"
#include <QtSql>
#include <QtGui>

PCx_AuditInfos::PCx_AuditInfos()
{
    valid=false;
}

PCx_AuditInfos::PCx_AuditInfos(unsigned int auditId)
{
    updateInfos(auditId);
}

void PCx_AuditInfos::updateInfos(unsigned int auditId)
{
    Q_ASSERT(auditId>0);

    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        id=auditId;
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

        yearsString=QString("%1 - %2").arg(years.first()).arg(years.last());

        finished=q.value("termine").toBool();
        creationTimeUTC=QDateTime::fromString(q.value("le_timestamp").toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
        valid=true;
    }
    else
    {
        valid=false;
    }
}
