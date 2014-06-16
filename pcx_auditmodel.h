#ifndef PCX_AUDITMODEL_H
#define PCX_AUDITMODEL_H

#include "pcx_auditinfos.h"
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
        ouverts,
        realises,
        engages,
        disponibles
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

    static bool addNewAudit(const QString &name, QList<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);

    unsigned int getAuditId() const{return auditId;}
    const PCx_AuditInfos &getAuditInfos() const{return auditInfos;}

    bool finishAudit();
    static bool finishAudit(unsigned int id);

    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    QSqlTableModel *getTableModel(const QString &mode) const;
    QSqlTableModel *getTableModel(DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);

    QString modeToTableString(DFRFDIRI mode) const;
    QString modeToCompleteString(DFRFDIRI mode) const;
    QString OREDtoCompleteString(ORED ored) const;
    QString OREDtoTableString(ORED ored) const;

signals:

public slots:

private:
    unsigned int auditId;

    PCx_AuditInfos auditInfos;
    PCx_TreeModel *attachedTree;

    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    bool loadFromDb(unsigned int auditId, bool readOnly=true);
    bool propagateToAncestors(const QModelIndex &node);
    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);

private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};


#endif // PCX_AUDITMODEL_H
