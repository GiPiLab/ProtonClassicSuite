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

signals:

public slots:

private:
    unsigned int auditId;
    bool finished;
    QString name;
    QString creationTime;
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
