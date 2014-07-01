#ifndef PCX_QUERIESMODEL_H
#define PCX_QUERIESMODEL_H

#include <QSqlQueryModel>

class PCx_QueriesModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit PCx_QueriesModel(QObject *parent = 0);
    explicit PCx_QueriesModel(unsigned int auditId, QObject *parent);


    QVariant data(const QModelIndex &item, int role) const;

    void update(void);





signals:

public slots:

private:
    unsigned int auditId;

};

#endif // PCX_QUERIESMODEL_H
