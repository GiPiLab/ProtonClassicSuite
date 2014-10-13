#ifndef PCX_TREE_H
#define PCX_TREE_H

#include "pcx_typemodel.h"

#include <QList>
#include <QString>

class PCx_Tree
{

public:

    /**
     * @brief MAXNODES is the maximum allowed number of nodes for a tree
     */
    static const unsigned int MAXNODES=1700;

    /**
     * @brief PCx_Tree::PCx_Tree represent an existing tree in the database
     *
     * This class provides methods to deal with a tree stored in the database.
     * The table is ARBRE_[treeId]. Each node in a tree has a name and a
     * type identifier. The type identifier has a name which is obtained through PCx_TypeModel methods
     *
     * @param treeId the identifier of the tree in the database
     * @param typesReadOnly if the PCx_TypeModel should be a read-only model or a read-write model
     */
    explicit PCx_Tree(unsigned int treeId, bool typesReadOnly=true);
    virtual ~PCx_Tree();


    unsigned int getTreeId() const {return treeId;}
    bool isFinished() const {return finished;}
    const QString & getName() const {return treeName;}
    const QString & getCreationTime() const{return creationTime;}
    PCx_TypeModel* getTypes() const {return types;}

    /**
     * @brief PCx_Tree::addNode insert a node in the tree
     *
     * Add a node in the tree. Check for the maximum number of nodes and if a node with the same name and type exists
     * @param pid the ID of the parent node (PID)
     * @param typeId the ID of the type for the node to insert
     * @param name the name of the node to insert
     * @param pidNodeIndex the index of the parent node, as selected in the view (if not specified, only insert the node in the DB)
     * @return the ID of the inserted node, or 0 in case of failure
     */
    unsigned int addNode(unsigned int pid, unsigned int type, const QString &name);


    bool updateNode(unsigned int nodeId, const QString &newName, unsigned int newType);

    /**
     * @brief PCx_Tree::deleteNode remove a node and all its children
     *
     * This method deletes a node and all its children
     * @param nodeId the node to delete
     * @return true on success, false on failure
     */
    bool deleteNode(unsigned int nodeId);

    unsigned int getNumberOfNodes() const;
    static unsigned int getNumberOfNodes(unsigned int treeId);
    QList<unsigned int> getLeavesId() const;


    /**
     * @brief PCx_Tree::getNodesId gets ID of all nodes
     * @return The list of all nodes ID
     */
    QList<unsigned int> getNodesId() const;

    /**
     * @brief PCx_Tree::getListOfCompleteNodeNames get all full node names (typeName + " "+nodeName)
     * @return The string list of full node names
     */
    QStringList getListOfCompleteNodeNames() const;

    /**
     * @brief PCx_Tree::getListOfNodeNames Get the list of simple node names (without the name of the type)
     *
     * For example, if a tree contains three nodes "T1 N1", "T1 N2" and "T2 N3"
     * getListOfNodeNames will return ("N1","N2","N3)
     * @return The QStringList of simple node names
     */
    QStringList getListOfNodeNames() const;
    static QList<unsigned int> getNodesId(unsigned int treeId);
    QList<unsigned int> getNonLeavesId() const;
    QSet<unsigned int> getNodesWithSharedName() const;

    bool isLeaf(unsigned int nodeId) const;
    unsigned int getTreeDepth() const;

    unsigned int getParentId(unsigned int nodeId) const;
    QList<unsigned int> getChildren(unsigned int nodeId=1) const;

    QString getNodeName(unsigned int node) const;
    QPair<QString, QString> getTypeNameAndNodeName(unsigned int node) const;


    QList<unsigned int> getIdsOfNodesWithThisType(unsigned int typeId) const;
    unsigned int getNumberOfNodesWithThisType(unsigned int typeId) const;

    QList<unsigned int> sortNodesDFS(QList<unsigned int> &nodes, unsigned int currentNode=1) const;
    QList<unsigned int> sortNodesBFS(QList<unsigned int> &nodes) const;


    bool finishTree();


    /**
     * @brief PCx_Tree::nodeExists check if a node with given name and type already exists
     *
     * If PID is not 0, check if this node belongs to the same PID, in order to only disallow duplicates under the same pid
     * @param name : the name of the node
     * @param typeId : the type of the node
     * @param pid : the pid of the node, if 0 check under the whole tree for duplicates
     * @return true if the node exists, false otherwise, die in case of DB error
     */
    bool nodeExists(const QString &name, unsigned int typeId, unsigned int pid=0) const;


    int getNodeIdFromTypeAndNodeName(const QPair<QString, QString> &typeAndNodeName ) const;



    QString toDot() const;
    bool toXLSX(const QString &fileName) const;

protected:
    PCx_TypeModel *types;

    unsigned int treeId;
    bool finished;
    QString treeName;
    QString creationTime;
    bool loadFromDatabase(unsigned int treeId);

    void updateNodePid(unsigned int nodeId, unsigned int newPid);
};

#endif // PCX_TREE_H
