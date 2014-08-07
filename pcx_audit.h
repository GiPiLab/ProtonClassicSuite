#ifndef PCX_AUDIT_H
#define PCX_AUDIT_H

#include "pcx_treemodel.h"
#include <QDateTime>


class PCx_Audit
{

public:

    enum DFRFDIRI
    {
        DF,
        RF,
        DI,
        RI,
        GLOBAL
    };

    enum ListAuditsMode
    {
        FinishedAuditsOnly,
        UnFinishedAuditsOnly,
        AllAudits
    };

    enum ORED
    {
        OUVERTS,
        REALISES,
        ENGAGES,
        DISPONIBLES
    };

    explicit PCx_Audit(unsigned int auditId,bool loadTreeModel=true);
    virtual ~PCx_Audit();

    static unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);

    //Getters
    unsigned int getAuditId() const{return auditId;}
    QString getAuditName() const{return auditName;}
    static QString getAuditName(unsigned int auditId);
    QString getAttachedTreeName() const{return attachedTreeName;}
    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    unsigned int getAttachedTreeId() const{return attachedTreeId;}
    static unsigned int getAttachedTreeId(unsigned int auditId);
    bool isFinished() const{return finished;}
    QString isFinishedString() const{return finishedString;}
    QList<unsigned int> getYears() const{return years;}
    QString getYearsString() const{return yearsString;}
    QStringList getYearsStringList() const{return yearsStringList;}
    QDateTime getCreationTimeUTC()const{return creationTimeUTC;}
    QDateTime getCreationTimeLocal()const{return creationTimeLocal;}

    bool finishAudit();
    bool unFinishAudit();
    static bool finishAudit(unsigned int id);
    static bool unFinishAudit(unsigned int id);


    virtual bool setLeafValues(unsigned int leafId, DFRFDIRI mode, unsigned int year, QHash<ORED, double> vals);
    qint64 getNodeValue(unsigned int nodeId, DFRFDIRI mode, ORED ored, unsigned int year) const;

    virtual bool clearAllData(DFRFDIRI mode);

    int duplicateAudit(const QString &newName,QList<unsigned int> years,
                                bool copyDF=true,bool copyRF=true, bool copyDI=true, bool copyRI=true) const;

    static int duplicateAudit(unsigned int auditId, const QString &newName, QList<unsigned int> years, bool copyDF, bool copyRF, bool copyDI, bool copyRI);

    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);

    static QString modeToTableString(DFRFDIRI mode);
    static QString modeToCompleteString(DFRFDIRI mode);
    static QString OREDtoCompleteString(ORED ored);
    static QString OREDtoTableString(ORED ored);

    static ORED OREDFromTableString(const QString &ored);
    static DFRFDIRI modeFromTableString(const QString &mode);

    static bool auditNameExists(const QString &auditName);


    QString getHTMLAuditStatistics() const;
    QList<unsigned int> getNodesWithNonNullValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllNullValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllZeroValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;

    static QStringList yearsListToStringList(QList<unsigned int> years);

signals:

public slots:


protected:

    unsigned int auditId;
    QString auditName;
    unsigned int attachedTreeId;
    PCx_TreeModel *attachedTree;

    bool loadTreeModel;

    QString attachedTreeName;

    QList<unsigned int> years;
    QString yearsString;
    QStringList yearsStringList;

    bool finished;
    QString finishedString;

    QDateTime creationTimeUTC;
    QDateTime creationTimeLocal;

    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);


private:
    PCx_Audit(const PCx_Audit &c);
    PCx_Audit &operator=(const PCx_Audit &);
};


#endif // PCX_AUDIT_H
