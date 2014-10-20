#ifndef PCX_AUDIT_H
#define PCX_AUDIT_H

#include "pcx_tree.h"
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




    explicit PCx_Audit(unsigned int auditId);
    virtual ~PCx_Audit();

    unsigned int getAuditId() const{return auditId;}
    PCx_Tree *getAttachedTree() const{return attachedTree;}
    QString getAuditName() const{return auditName;}
    QString getAttachedTreeName() const{return attachedTreeName;}
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

    //Fast mode disable few sanity checks
    virtual bool setLeafValues(unsigned int leafId, PCx_Audit::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED, double> vals, bool fastMode=false);
    qint64 getNodeValue(unsigned int nodeId, PCx_Audit::DFRFDIRI mode, PCx_Audit::ORED ored, unsigned int year) const;
    QHash<PCx_Audit::ORED,qint64> getNodeValues(unsigned int nodeId, PCx_Audit::DFRFDIRI mode, unsigned int year) const;


    virtual bool clearAllData(PCx_Audit::DFRFDIRI mode);

    int duplicateAudit(const QString &newName,QList<unsigned int> years,
                                bool copyDF=true,bool copyRF=true, bool copyDI=true, bool copyRI=true) const;

    QList<unsigned int> getNodesWithNonNullValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllNullValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;
    QList<unsigned int> getNodesWithAllZeroValues(PCx_Audit::DFRFDIRI mode, unsigned int year) const;


    int importDataFromXLSX(const QString &fileName, PCx_Audit::DFRFDIRI mode);

    bool exportLeavesDataXLSX(PCx_Audit::DFRFDIRI mode, const QString &fileName) const;

    static QString getCSS();

    QString generateHTMLHeader() const;
    QString getHTMLAuditStatistics() const;
    QString generateHTMLAuditTitle() const;

    static QString modeToCompleteString(DFRFDIRI mode);
    static DFRFDIRI modeFromTableString(const QString &mode);
    static QString modeToTableString(DFRFDIRI mode);
    static QString OREDtoCompleteString(ORED ored);
    static QString OREDtoTableString(ORED ored);
    static ORED OREDFromTableString(const QString &ored);

    static unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);
    static bool auditNameExists(const QString &auditName);
    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);




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
