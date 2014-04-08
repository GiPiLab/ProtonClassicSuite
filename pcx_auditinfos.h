#ifndef PCX_AUDITINFOS_H
#define PCX_AUDITINFOS_H

#include <QtGui>
class PCx_AuditInfos
{
public:
    PCx_AuditInfos(unsigned int auditId);

    QString name;
    unsigned int id;
    QString attachedTreeName;
    unsigned int attachedTreeId;
    QSet<unsigned int> annees;
    QString anneesString;
    bool finished;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    bool valid;

};

#endif // PCX_AUDITINFOS_H
