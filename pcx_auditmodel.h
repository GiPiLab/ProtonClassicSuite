#ifndef PCX_AUDITMODEL_H
#define PCX_AUDITMODEL_H

#include <QtGui>
#include <QtSql>

#include "pcx_treemodel.h"

//Column indexes in sql tables
#define COL_ID 0
#define COL_IDNODE 1
#define COL_ANNEE 2
#define COL_OUVERTS 3
#define COL_REALISES 4
#define COL_ENGAGES 5
#define COL_DISPONIBLES 6


enum DFRFDIRI
{
    DF,
    RF,
    DI,
    RI
};

enum ListAuditsMode
{
    FinishedAuditsOnly,
    UnFinishedAuditsOnly,
    AllAudits
};


class PCx_AuditModel : public QObject
{
    Q_OBJECT

public:

    explicit PCx_AuditModel(unsigned int auditId,QObject *parent = 0);
    virtual ~PCx_AuditModel();

    static bool addNewAudit(const QString &name, QSet<unsigned int> years, unsigned int attachedTreeId);
    static bool deleteAudit(unsigned int auditId);

    unsigned int getAuditId() const{return auditId;}
    bool isFinished() const{return finished;}
    const QString &getName() const{return name;}
    QDateTime getCreationTime() const;
    const QSet<unsigned int> &getYears()const {return years;}

    bool finishAudit();
    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    QSqlTableModel *getTableModel(const QString &mode) const;
    QSqlTableModel *getTableModel(DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    static QList<QPair<unsigned int, QString> > getListOfAudits(ListAuditsMode mode);
    static bool finishAudit(unsigned int id);

    //TODO : T10, T11, T12
    QString getTabJoursAct(unsigned int node, DFRFDIRI mode) const;
    QString getTabBase100(unsigned int node, DFRFDIRI mode) const;
    QString getTabEvolutionCumul(unsigned int node, DFRFDIRI mode) const;
    QString getTabEvolution(unsigned int node, DFRFDIRI mode) const;
    QString getTabRecap(unsigned int node, DFRFDIRI mode) const;
    QString getTabResults(unsigned int node) const;
    QString getCSS() const;

    QString modetoTableString(DFRFDIRI mode) const;
    QString modeToCompleteString(DFRFDIRI mode) const;

    //T1,T2... are named as in the original PCA version
    //Content of tab "Recap"
    QString getT1(unsigned int node, DFRFDIRI mode) const;
    QString getT4(unsigned int node, DFRFDIRI mode) const;
    QString getT8(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Evolution"
    QString getT2bis(unsigned int node, DFRFDIRI mode) const;
    QString getT3bis(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "EvolutionCumul"
    QString getT2(unsigned int node, DFRFDIRI mode) const;
    QString getT3(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Base100"
    QString getT5(unsigned int node, DFRFDIRI mode) const;
    QString getT6(unsigned int node, DFRFDIRI mode) const;

    //Content of tab "Jours act."
    QString getT7(unsigned int node, DFRFDIRI mode) const;
    QString getT9(unsigned int node, DFRFDIRI mode) const;

    //Content of "Resultats" mode (without tabs)
    QString getT10(unsigned int node) const;
    QString getT11(unsigned int node) const;
    QString getT12(unsigned int node) const;

signals:

public slots:

private:
    unsigned int auditId;

    //TODO : fill finished
    bool finished;

    QString name;

    //TODO : fill creationTime
    QString creationTime;
   //TODO : fill years
    QSet<unsigned int> years;
    PCx_TreeModel *attachedTree;
    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    bool loadFromDb(unsigned int auditId);
    bool propagateToAncestors(const QModelIndex &node);
    bool updateParent(const QString &tableName, unsigned int annee, unsigned int nodeId);




private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // PCX_AUDITMODEL_H
