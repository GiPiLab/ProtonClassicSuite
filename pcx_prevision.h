#ifndef PCX_PREVISION_H
#define PCX_PREVISION_H

#include "pcx_auditwithtreemodel.h"
#include "pcx_previsionitemcriteria.h"
#include <QDateTime>

class PCx_Prevision
{
public:

    PCx_Prevision(unsigned int previsionId);

    unsigned int getPrevisionId()const{return previsionId;}
    QString getPrevisionName()const{return previsionName;}
    unsigned int getAttachedAuditId()const{return attachedAuditId;}
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}

    ~PCx_Prevision();

    static bool addNewPrevision(unsigned int auditId, const QString &name);
    static bool previsionNameExists(const QString &previsionName);
    static QList<QPair<unsigned int, QString> > getListOfPrevisions();
    static bool deletePrevision(unsigned int previsionId);


    qint64 computePrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, QList<PCx_PrevisionItemCriteria> criteriaToAdd,
                            QList<PCx_PrevisionItemCriteria> criteriaToSubstract) const;

    qint64 computePrevisionItem(unsigned int previsionItemId, MODES::DFRFDIRI mode) const;

    int setPrevisionItem(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year, const QString &label,
                          QList<PCx_PrevisionItemCriteria> criteriaToAdd, QList<PCx_PrevisionItemCriteria> criteriaToSubstract) const;

    QHash<QString,QString> getPrevisionItemDescription(unsigned int nodeId, MODES::DFRFDIRI mode, unsigned int year) const;




private:
    unsigned int previsionId;
    unsigned int attachedAuditId;
    QString previsionName;
    QDateTime creationTimeLocal;
    QDateTime creationTimeUTC;



    PCx_Prevision(const PCx_Prevision &c);
    PCx_Prevision &operator=(const PCx_Prevision &);


};



#endif // PCX_PREVISION_H
