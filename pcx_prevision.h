#ifndef PCX_PREVISION_H
#define PCX_PREVISION_H

#include "pcx_auditwithtreemodel.h"
#include "pcx_previsioncriteria.h"
#include <QDateTime>

class PCx_Prevision
{
public:

    PCx_Prevision(unsigned int previsionId);




    qint64 computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionCriteria> criteriaToAdd,
                            QList<PCx_PrevisionCriteria> criteriaToSubstract) const;

    qint64 computePrevision(unsigned int nodeId, MODES::DFRFDIRI mode) const;

    PCx_AuditWithTreeModel *getAttachedAudit()const{return attachedAudit;}
    unsigned int getPrevisionId()const{return previsionId;}
    QString getPrevisionName()const{return previsionName;}
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}


    ~PCx_Prevision();


    static bool addNewPrevision(unsigned int auditId, const QString &name);

    bool setPrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year, const QString &label,
                          QList<PCx_PrevisionCriteria> criteriaToAdd, QList<PCx_PrevisionCriteria> criteriaToSubstract) const;



    static bool previsionNameExists(const QString &previsionName);

    static QList<QPair<unsigned int, QString> > getListOfPrevisions();


    static bool deletePrevision(unsigned int previsionId);

private:
    PCx_AuditWithTreeModel *attachedAudit;
    unsigned int previsionId;
    QString previsionName;
    QDateTime creationTimeLocal;
    QDateTime creationTimeUTC;

    PCx_Prevision(const PCx_Prevision &c);
    PCx_Prevision &operator=(const PCx_Prevision &);


};



#endif // PCX_PREVISION_H
