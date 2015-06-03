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
    virtual ~PCx_Prevision();

    unsigned int getPrevisionId()const{return previsionId;}
    QString getPrevisionName()const{return previsionName;}
    unsigned int getAttachedAuditId()const{return attachedAuditId;}
    unsigned int getAttachedTreeId()const{return attachedTreeId;}
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}
    PCx_Audit *getAttachedAudit()const{return attachedAudit;}
    PCx_Tree *getAttachedTree()const{return attachedTree;}

    int toPrevisionalExtendedAudit(const QString &newAuditName);

    bool isPrevisionEmpty()const;
    int duplicatePrevision(const QString &newName) const;

    static unsigned int addNewPrevision(unsigned int auditId, const QString &name);
    static bool previsionNameExists(const QString &previsionName);
    static QList<QPair<unsigned int, QString> > getListOfPrevisions();
    static bool deletePrevision(unsigned int previsionId);

    QString generateHTMLHeader() const;
    QString generateHTMLPrevisionTitle() const;

private:
    unsigned int previsionId;
    unsigned int attachedAuditId;
    unsigned int attachedTreeId;
    QString previsionName;
    QDateTime creationTimeLocal;
    QDateTime creationTimeUTC;

    PCx_Audit *attachedAudit;
    PCx_Tree *attachedTree;

    PCx_Prevision(const PCx_Prevision &c);
    PCx_Prevision &operator=(const PCx_Prevision &);
};

#endif // PCX_PREVISION_H
