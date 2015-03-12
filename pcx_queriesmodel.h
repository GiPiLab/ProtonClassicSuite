#ifndef PCX_QUERIESMODEL_H
#define PCX_QUERIESMODEL_H

#include <QSqlQueryModel>
#include "pcx_audit.h"

class PCx_QueriesModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit PCx_QueriesModel(QObject *parent = 0);
    explicit PCx_QueriesModel(PCx_Audit *auditModel, QObject *parent);


    QVariant data(const QModelIndex &item, int role) const;

    void update(void);





signals:

public slots:

private:
    PCx_Audit *auditModel;
    unsigned int auditId;
    PCx_QueriesModel(const PCx_QueriesModel &c);
    PCx_QueriesModel &operator=(const PCx_QueriesModel &);

};

#endif // PCX_QUERIESMODEL_H
