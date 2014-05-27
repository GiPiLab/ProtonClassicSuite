#ifndef PCX_AUDITINFOS_H
#define PCX_AUDITINFOS_H

#include <QString>
#include <QList>
#include <QDateTime>

class PCx_AuditInfos
{
public:
    PCx_AuditInfos();
    PCx_AuditInfos(unsigned int auditId);
    void updateInfos(unsigned int auditId);

    QString name;
    unsigned int id;
    QString attachedTreeName;
    unsigned int attachedTreeId;
    QList<unsigned int> years;

    QString yearsString;
    bool finished;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    bool valid;

};

#endif // PCX_AUDITINFOS_H
