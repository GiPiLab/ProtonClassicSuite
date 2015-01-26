#ifndef PCX_PREVISION_H
#define PCX_PREVISION_H

#include "pcx_auditwithtreemodel.h"
#include "pcx_previsionitemcriteria.h"
#include <QDateTime>
#include <QAbstractListModel>

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
