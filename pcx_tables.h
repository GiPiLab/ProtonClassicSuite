#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include <QObject>
#include "pcx_auditmodel.h"

class PCx_tables : public QObject
{
    Q_OBJECT
public:
    explicit PCx_tables(unsigned int auditId,QObject *parent = 0);
    virtual ~PCx_tables();
    QString getT1(unsigned int node, QSqlTableModel *model) const;
    PCx_AuditModel *getModel(){return currentAudit;}

signals:

public slots:

private:
    PCx_AuditModel *currentAudit;


};

#endif // PCX_TABLES_H
