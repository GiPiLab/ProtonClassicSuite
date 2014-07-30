#ifndef PCX_AUDITMODEL_H
#define PCX_AUDITMODEL_H

#include "pcx_treemodel.h"


class PCx_AuditModel : public QObject
{
    Q_OBJECT


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

    //Column indexes in sql tables
    enum COLINDEXES
    {
        COL_ID=0,
        COL_IDNODE,
        COL_ANNEE,
        COL_OUVERTS,
        COL_REALISES,
        COL_ENGAGES,
        COL_DISPONIBLES
    };


    explicit PCx_AuditModel(unsigned int auditId,QObject *parent = 0,bool readOnly=true);
    virtual ~PCx_AuditModel();

    static unsigned int addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);

    unsigned int getAuditId() const{return auditId;}

    bool finishAudit();
    bool unFinishAudit();
    static bool finishAudit(unsigned int id);
    static bool unFinishAudit(unsigned int id);

    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    QSqlTableModel *getTableModel(const QString &mode) const;
    QSqlTableModel *getTableModel(DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    bool setLeafValues(unsigned int leafId, DFRFDIRI mode, unsigned int year, QHash<ORED, double> vals);
    qint64 getNodeValue(unsigned int nodeId, DFRFDIRI mode, ORED ored, unsigned int year) const;

    bool clearAllData(DFRFDIRI mode);

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


signals:

public slots:

private:
    unsigned int auditId;
    PCx_TreeModel *attachedTree;

    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    bool loadFromDb(unsigned int auditId, bool readOnly=true);
    bool propagateToAncestors(const QModelIndex &node);
    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);

private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};


#endif // PCX_AUDITMODEL_H
