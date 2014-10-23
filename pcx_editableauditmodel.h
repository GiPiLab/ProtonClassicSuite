#ifndef PCX_EDITABLEAUDITMODEL_H
#define PCX_EDITABLEAUDITMODEL_H

#include "pcx_auditwithtreemodel.h"
#include <QObject>

class PCx_EditableAuditModel : public QObject, public PCx_AuditWithTreeModel
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
    QSqlTableModel *getTableModel(PCx_Audit::DFRFDIRI mode) const;
    QSqlTableModel *getTableModelDF() const {return modelDF;}
    QSqlTableModel *getTableModelDI() const {return modelDI;}
    QSqlTableModel *getTableModelRF() const {return modelRF;}
    QSqlTableModel *getTableModelRI() const {return modelRI;}

    //Fast mode does not update the table model
    bool setLeafValues(unsigned int leafId, PCx_Audit::DFRFDIRI mode, unsigned int year, QHash<PCx_Audit::ORED, double> vals, bool fastMode=false);

    void clearAllData(PCx_Audit::DFRFDIRI mode);


signals:

public slots:


private:
    QSqlTableModel *modelDF,*modelRF,*modelDI,*modelRI;
    void propagateToAncestors(const QModelIndex &node);

    PCx_EditableAuditModel(const PCx_EditableAuditModel &c);
    PCx_EditableAuditModel &operator=(const PCx_EditableAuditModel &);


private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

};

#endif // PCX_EDITABLEAUDITMODEL_H
