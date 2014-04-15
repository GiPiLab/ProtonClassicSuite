#ifndef PCX_AUDITMODEL_H
#define PCX_AUDITMODEL_H

#include <QtGui>
#include <QtSql>

#include "pcx_treemodel.h"

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

    PCx_TreeModel *getAttachedTreeModel() const{return attachedTree;}
    QSqlTableModel *getModelDF() const {return modelDF;}
    QSqlTableModel *getModelDI() const {return modelDI;}
    QSqlTableModel *getModelRF() const {return modelRF;}
    QSqlTableModel *getModelRI() const {return modelRI;}

    static QHash<int, QString> getListOfAudits(bool finishedOnly);

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
};

#endif // PCX_AUDITMODEL_H
