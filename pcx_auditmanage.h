#ifndef PCX_AUDITMANAGE_H
#define PCX_AUDITMANAGE_H

#include <QString>
#include <QPair>
#include <QList>

namespace PCx_AuditManage{

enum DFRFDIRI
{
    DF,
    RF,
    DI,
    RI,
    GLOBAL
};



enum ORED
{
    OUVERTS,
    REALISES,
    ENGAGES,
    DISPONIBLES
};


enum ListAuditsMode
{
    FinishedAuditsOnly,
    UnFinishedAuditsOnly,
    AllAudits
};

unsigned int getAttachedTreeId(unsigned int auditId);
unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
bool deleteAudit(unsigned int auditId);
int duplicateAudit(unsigned int auditId, const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI);
bool auditNameExists(const QString &auditName);
QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);
bool finishAudit(unsigned int auditId);
bool unFinishAudit(unsigned int auditId);


QString modeToTableString(DFRFDIRI mode);
QString modeToCompleteString(DFRFDIRI mode);
QString OREDtoCompleteString(ORED ored);
QString OREDtoTableString(ORED ored);

ORED OREDFromTableString(const QString &ored);
DFRFDIRI modeFromTableString(const QString &mode);


}

#endif // PCX_AUDITMANAGE_H
