#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include <QStandardItemModel>
#include <QMimeData>
#include <QSqlTableModel>
#include "pcx_tree.h"

class PCx_TreeModel : public QStandardItemModel,public PCx_Tree
{
    Q_OBJECT

public:
    explicit PCx_TreeModel(unsigned int treeId, QObject *parent = 0);
    virtual ~PCx_TreeModel();

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    bool updateTree();

    QSqlTableModel *getTypesTableModel(){return typesTableModel;}

    unsigned int addNode(unsigned int pid, unsigned int typeId, const QString &name, const QModelIndex &pidNodeIndex);

    bool updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType);
    bool deleteNode(const QModelIndex &nodeIndex);

    QModelIndexList getIndexesOfNodesWithThisType(unsigned int typeId) const;

    unsigned int addType(const QString &typeName);
    bool deleteType(unsigned int typeId);



private:
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName, const QString &nodeName, unsigned int typeId, unsigned int nodeId);

    bool loadTypesTableModel();

    QSqlTableModel *typesTableModel;




signals:
    void typesUpdated(void);


public slots:

private slots:
    bool onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);


};

#endif // PCX_TREEMODEL_H
