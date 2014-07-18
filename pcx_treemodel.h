#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include "pcx_typemodel.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QSqlTableModel>
#include <QMimeData>

//TODO : Refactor this class

class PCx_TreeModel:public QStandardItemModel
{

public:
    explicit PCx_TreeModel(unsigned int treeId, bool typesReadOnly=true, QObject *parent=0);
    virtual ~PCx_TreeModel();

    unsigned int getTreeId() const {return treeId;}
    bool isFinished() const {return finished;}
    const QString & getName() const {return treeName;}
    QDateTime getCreationTime() const;
    PCx_TypeModel* getTypes() const {return types;}

    void setName(const QString & name){treeName=name;}

    unsigned int addNode(unsigned int pid, unsigned int type, const QString &name, const QModelIndex &pidNodeIndex);
    bool updateNode(const QModelIndex &nodeIndex ,const QString &newName, unsigned int newType);

    bool deleteNode(const QModelIndex &nodeIndex);

    QList<unsigned int> getNodesId() const;
    QList<unsigned int> getLeavesId() const;
    static QList<unsigned int> getNodesId(unsigned int treeId);
    QList<unsigned int> getNonLeavesId() const;
    bool isLeaf(unsigned int nodeId) const;

    unsigned int getParentId(unsigned int nodeId) const;
    QList<unsigned int> getChildren(unsigned int nodeId=1) const;

    QModelIndexList getIndexesOfNodesWithThisType(unsigned int typeId) const;

    QList<unsigned int> sortNodesDFS(QList<unsigned int> &nodes, unsigned int currentNode=1) const;
    QList<unsigned int> sortNodesBFS(QList<unsigned int> &nodes) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void updateNodePosition(unsigned int nodeId, unsigned int newPid);
    bool finishTree();

    bool updateTree();

    //Export tree to dot format
    QString toDot() const;

    int duplicateTree(const QString &newName) const;

    QString getNodeName(unsigned int node) const;
    static int addNewTree(const QString &name);
    static int deleteTree(unsigned int treeId);
    static QString idTreeToName(unsigned int treeId);
    static QList<QPair<unsigned int,QString> > getListOfTrees(bool finishedOnly=false);


private:
    PCx_TypeModel *types;

    unsigned int treeId;
    bool finished;
    QString treeName;
    QString creationTime;

    static int addTree(const QString &name, bool createRoot=true);

    bool loadFromDatabase(unsigned int treeId);
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    bool deleteNodeAndChildren(unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName,const QString &nodeName,unsigned int typeId,unsigned int nodeId);
};

#endif // PCX_TREEMODEL_H
