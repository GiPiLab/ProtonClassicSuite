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
        yearsString=q.value(3).toString();
        QStringList yearsSplitted=yearsString.split(',');

        QList<unsigned int> yearsTemp;
        foreach (QString uneAnnee, yearsSplitted) {
            yearsTemp.append(uneAnnee.toUInt());
        }
        qSort(yearsTemp);
        yearsString=QString("%1 - %2").arg(yearsTemp.first()).arg(yearsTemp.last());
        years=yearsTemp.toSet();

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
