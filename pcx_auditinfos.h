#ifndef PCX_AUDITINFOS_H
#define PCX_AUDITINFOS_H

#include <QStringList>
#include <QDateTime>

class PCx_AuditInfos
{
public:
    PCx_AuditInfos(unsigned int auditId);
    bool updateInfos(unsigned int auditId);

    QString name;
    unsigned int id;
    QString attachedTreeName;
    unsigned int attachedTreeId;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    static QStringList yearsListToStringList(QList<unsigned int> years);
};

#endif // PCX_AUDITINFOS_H
