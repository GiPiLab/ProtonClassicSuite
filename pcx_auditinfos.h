#ifndef PCX_AUDITINFOS_H
#define PCX_AUDITINFOS_H

#include <QStringList>
#include <QDateTime>
#include "pcx_auditmodel.h"

class PCx_AuditInfos
{
public:
    PCx_AuditInfos(unsigned int auditId);


    QString name;
    QString attachedTreeName;
    unsigned int attachedTreeId;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;
    QString finishedString;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    QString getHTMLAuditStatistics() const;
    QList<unsigned int> getNodesWithNonNullValues(PCx_AuditModel::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllNullValues(PCx_AuditModel::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllZeroValues(PCx_AuditModel::DFRFDIRI mode, unsigned int year) const;

    static QStringList yearsListToStringList(QList<unsigned int> years);

private:
    unsigned int auditId;
};

#endif // PCX_AUDITINFOS_H
