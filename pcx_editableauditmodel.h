#ifndef PCX_EDITABLEAUDITMODEL_H
#define PCX_EDITABLEAUDITMODEL_H

#include "pcx_audit.h"
#include <QObject>

class PCx_EditableAuditModel : public QObject, public PCx_Audit
{
    Q_OBJECT
public:

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

    explicit PCx_EditableAuditModel(unsigned int auditId,QObject *parent = 0);
    virtual ~PCx_EditableAuditModel();

    QSqlTableModel *getTableModel(const QString &mode) const;
    QSqlTableModel *getTableModel(DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    bool setLeafValues(unsigned int leafId, DFRFDIRI mode, unsigned int year, QHash<ORED, double> vals);
    bool clearAllData(DFRFDIRI mode);


signals:

public slots:


private:
    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    bool propagateToAncestors(const QModelIndex &node);


private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

};

#endif // PCX_EDITABLEAUDITMODEL_H
