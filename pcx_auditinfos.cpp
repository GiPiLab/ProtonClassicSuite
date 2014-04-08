#include "pcx_auditinfos.h"
#include "pcx_treemodel.h"
#include "utility.h"
#include <QtSql>
#include <QtGui>

PCx_AuditInfos::PCx_AuditInfos(unsigned int auditId)
{
    Q_ASSERT(auditId>0);

    QSqlQuery q(QString("select * from index_audits where id='%1'").arg(auditId));
    if(q.next())
    {
        id=auditId;
        name=q.value(1).toString();
        attachedTreeId=q.value(2).toUInt();
        attachedTreeName=PCx_TreeModel::idTreeToName(attachedTreeId);
        anneesString=q.value(3).toString();
        anneesString.replace(',',", ");
        QStringList anneesSplitted=anneesString.split(',');
        foreach (QString uneAnnee, anneesSplitted) {
            annees.insert(uneAnnee.toUInt());
        }
        finished=q.value(4).toBool();
        creationTimeUTC=QDateTime::fromString(q.value(5).toString(),"yyyy-MM-dd hh:mm:ss");
        creationTimeUTC.setTimeSpec(Qt::UTC);
        creationTimeLocal=creationTimeUTC.toLocalTime();
        valid=true;
    }
    else
    {
        valid=false;
    }
}
