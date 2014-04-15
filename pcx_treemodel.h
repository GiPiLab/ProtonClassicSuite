#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include "pcx_typemodel.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QtGlobal>
#include <QSqlTableModel>
#include <QMimeData>


class PCx_TreeModel:public QStandardItemModel
{

public:
    explicit PCx_TreeModel(unsigned int treeId, QObject *parent=0);
    virtual ~PCx_TreeModel();

    int getTreeId() const {return treeId;}
    bool isFinished() const {return finished;}
    const QString & getName() const {return treeName;}
    QDateTime getCreationTime() const;
    PCx_TypeModel* getTypes() const {return types;}

    void setName(const QString & name){treeName=name;}

    unsigned int addNode(int pid, int type, const QString &name, const QModelIndex &pidNodeIndex);
    bool updateNode(const QModelIndex &nodeIndex ,const QString &newName, unsigned int newType);

    bool deleteNode(const QModelIndex &nodeIndex);

    QList<unsigned int> getNodesId() const;
    QList<unsigned int> getLeavesId() const;
    static QList<unsigned int> getNodesId(unsigned int treeId);
    QList<unsigned int> getNonLeavesId() const;
    bool isLeaf(unsigned int nodeId) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void updateNodePosition(unsigned int nodeId, unsigned int newPid);
    bool finishTree();

    bool updateTree();




    static bool addNewTree(const QString &name);
    static int deleteTree(unsigned int treeId);
    static QString idTreeToName(unsigned int treeId);
    static QHash<int,QString> getListOfTrees(bool finishedOnly=false);



private:
    QStandardItem *root;
    PCx_TypeModel *types;

    int treeId;
    bool finished;
    QString treeName;
    QString creationTime;

    bool loadFromDatabase(unsigned int treeId);
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    bool deleteNodeAndChildren(unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName,const QString &nodeName,unsigned int typeId,unsigned int nodeId);
};

#endif // PCX_TREEMODEL_H
