#ifndef PCX_AUDIT_H
#define PCX_AUDIT_H

#include "pcx_tree.h"
#include "pcx_auditmanage.h"
#include <QDateTime>


class PCx_Audit
{

public:

    explicit PCx_Audit(unsigned int auditId);
    virtual ~PCx_Audit();

    unsigned int getAuditId() const{return auditId;}
    PCx_Tree *getAttachedTree() const{return attachedTree;}
    QString getAuditName() const{return auditName;}
    QString getAttachedTreeName() const{return attachedTreeName;}
    unsigned int getAttachedTreeId() const{return attachedTreeId;}

    bool isFinished() const{return finished;}
    QString isFinishedString() const{return finishedString;}
    QList<unsigned int> getYears() const{return years;}
    QString getYearsString() const{return yearsString;}
    QStringList getYearsStringList() const{return yearsStringList;}
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}

    bool finishAudit();
    bool unFinishAudit();

    //Fast mode disable few sanity checks
    virtual bool setLeafValues(unsigned int leafId, PCx_AuditManage::DFRFDIRI mode, unsigned int year, QHash<PCx_AuditManage::ORED, double> vals, bool fastMode=false);
    qint64 getNodeValue(unsigned int nodeId, PCx_AuditManage::DFRFDIRI mode, PCx_AuditManage::ORED ored, unsigned int year) const;
    QHash<PCx_AuditManage::ORED,qint64> getNodeValues(unsigned int nodeId, PCx_AuditManage::DFRFDIRI mode, unsigned int year) const;


    virtual bool clearAllData(PCx_AuditManage::DFRFDIRI mode);

    int duplicateAudit(const QString &newName,QList<unsigned int> years,
                                bool copyDF=true,bool copyRF=true, bool copyDI=true, bool copyRI=true) const;


    QString getHTMLAuditStatistics() const;
    QList<unsigned int> getNodesWithNonNullValues(PCx_AuditManage::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllNullValues(PCx_AuditManage::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllZeroValues(PCx_AuditManage::DFRFDIRI mode, unsigned int year) const;


    int importDataFromXLSX(const QString &fileName, PCx_AuditManage::DFRFDIRI mode);

    bool exportLeavesDataXLSX(PCx_AuditManage::DFRFDIRI mode, const QString &fileName) const;




signals:

public slots:


protected:

    unsigned int auditId;
    QString auditName;
    unsigned int attachedTreeId;

    QString attachedTreeName;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;
    QString finishedString;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);


protected:
    PCx_Tree *attachedTree;
    PCx_Audit(const PCx_Audit &c);
    PCx_Audit &operator=(const PCx_Audit &);

    QHash<unsigned int,unsigned int> idToPid;
    QHash<unsigned int,QList<unsigned int> >idToChildren;
    QHash<unsigned int,QString> idToChildrenString;
};


#endif // PCX_AUDIT_H
