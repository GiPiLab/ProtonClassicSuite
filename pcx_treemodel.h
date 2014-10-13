#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include <QStandardItemModel>
#include <QMimeData>
#include "pcx_tree.h"

class PCx_TreeModel : public QStandardItemModel,public PCx_Tree
{
    Q_OBJECT
public:
    explicit PCx_TreeModel(unsigned int treeId,bool readOnlyTypes=true,QObject *parent = 0);
    virtual ~PCx_TreeModel();

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    bool updateTree();

    unsigned int addNode(unsigned int pid, unsigned int typeId, const QString &name, const QModelIndex &pidNodeIndex);

    bool updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType);
    bool deleteNode(const QModelIndex &nodeIndex);


    QModelIndexList getIndexesOfNodesWithThisType(unsigned int typeId) const;


private:
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName, const QString &nodeName, unsigned int typeId, unsigned int nodeId);

signals:

public slots:

};

#endif // PCX_TREEMODEL_H
