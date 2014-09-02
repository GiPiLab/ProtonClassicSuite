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

//TODO for V2 : Support lazy loading for big trees

class PCx_TreeModel:public QStandardItemModel
{

public:

    static const unsigned int MAXNODES=1500;

    explicit PCx_TreeModel(unsigned int treeId, bool typesReadOnly=true, bool noLoadModel=false, QObject *parent=0);
    virtual ~PCx_TreeModel();

    unsigned int getTreeId() const {return treeId;}
    bool isFinished() const {return finished;}
    const QString & getName() const {return treeName;}
    const QString & getCreationTime() const{return creationTime;}
    PCx_TypeModel* getTypes() const {return types;}

    unsigned int addNode(unsigned int pid, unsigned int type, const QString &name, const QModelIndex &pidNodeIndex);
    bool updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType);

    bool deleteNode(const QModelIndex &nodeIndex);

    unsigned int getNumberOfNodes() const;
    static unsigned int getNumberOfNodes(unsigned int treeId);
    QList<unsigned int> getLeavesId() const;
    QList<unsigned int> getNodesId() const;
    static QList<unsigned int> getNodesId(unsigned int treeId);
    QList<unsigned int> getNonLeavesId() const;

    bool isLeaf(unsigned int nodeId) const;
    unsigned int getTreeDepth() const;

    unsigned int getParentId(unsigned int nodeId) const;
    QList<unsigned int> getChildren(unsigned int nodeId=1) const;

    QModelIndexList getIndexesOfNodesWithThisType(unsigned int typeId) const;
    QList<unsigned int> getIdsOfNodesWithThisType(unsigned int typeId) const;
    unsigned int getNumberOfNodesWithThisType(unsigned int typeId) const;

    QList<unsigned int> sortNodesDFS(QList<unsigned int> &nodes, unsigned int currentNode=1) const;
    QList<unsigned int> sortNodesBFS(QList<unsigned int> &nodes) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    bool finishTree();
    bool updateTree();

    //Export tree to dot format
    QString toDot() const;
    QString toTSV() const;

    int duplicateTree(const QString &newName) const;

    QString getNodeName(unsigned int node) const;
    QPair<QString, QString> getTypeNameAndNodeName(unsigned int node) const;

    static int addNewTree(const QString &name);
    static int deleteTree(unsigned int treeId);
    static int importTreeFromTSV(const QString &fileName,const QString &treeName);

    static bool treeNameExists(const QString &name);


    static QString idTreeToName(unsigned int treeId);
    static QList<QPair<unsigned int,QString> > getListOfTrees(bool finishedOnly=false);

    static int createRandomTree(const QString &name, unsigned int nbNodes);


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
    void updateNodePosition(unsigned int nodeId, unsigned int newPid);
    bool noLoadModel;
};

#endif // PCX_TREEMODEL_H
