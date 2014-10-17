#ifndef PCX_AUDITMANAGE_H
#define PCX_AUDITMANAGE_H

#include <QString>
#include <QPair>
#include <QList>

namespace PCx_AuditManage{



enum ListAuditsMode
{
    FinishedAuditsOnly,
    UnFinishedAuditsOnly,
    AllAudits
};

unsigned int getAttachedTreeId(unsigned int auditId);
unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
bool deleteAudit(unsigned int auditId);
bool auditNameExists(const QString &auditName);
QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);


}

#endif // PCX_AUDITMANAGE_H
