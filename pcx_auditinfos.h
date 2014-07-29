#ifndef PCX_AUDITINFOS_H
#define PCX_AUDITINFOS_H

#include <QStringList>
#include <QDateTime>
#include "pcx_auditmodel.h"

class PCx_AuditInfos
{
public:
    PCx_AuditInfos(unsigned int auditId);
    bool updateInfos(unsigned int auditId);

    QString name;
    QString attachedTreeName;
    unsigned int attachedTreeId;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

   // QString getNodesStatistics(PCx_AuditModel::DFRFDIRI mode);

    static QStringList yearsListToStringList(QList<unsigned int> years);


private:
    unsigned int auditId;
};

#endif // PCX_AUDITINFOS_H
