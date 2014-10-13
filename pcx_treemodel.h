#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include <QStandardItemModel>

class PCx_TreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PCx_TreeModel(QObject *parent = 0);

signals:

public slots:

};

#endif // PCX_TREEMODEL_H
