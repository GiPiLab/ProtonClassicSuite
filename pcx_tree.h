#ifndef PCX_TREE_H
#define PCX_TREE_H


#include <QStringList>
#include <QPair>
#include <QHash>
#include <QDebug>


/**
 * @brief The PCx_Tree class represents an existing tree in the database
 *
 * This class provides methods to deal with a tree stored in the database.
 * The table is "arbre_[treeId]".
 *
 * Each tree has a name, a creation time and is finished or not. A finished tree
 * cannot be modified in the user interface. Only a finished tree is allowed to build an audit.
 *
 * Each node in a tree has a name, an unsigned identifier, a parent identifier and a type identifier.
 * The type identifier is a mapping between an unsigned int and a string stored in table "types_[treeId]".
 * Duplicates are not allowed (same name and same type identifier)
 *
 *
 */
class PCx_Tree
{

public:

    /**
     * @brief MAXNODES is the maximum allowed number of nodes for a tree
     */
    static const unsigned int MAXNODES=1700;


    /**
     * @brief PCx_Tree is a Tree
     * @param treeId the identifier of the tree in the database
     */
    explicit PCx_Tree(unsigned int treeId);

    virtual ~PCx_Tree(){}

    /**
     * @brief getTreeId gets the identifier of the tree in the database
     * @return the tree ID in the DB
     */
    unsigned int getTreeId() const {return treeId;}

    /**
     * @brief isFinished
     * @return true if a tree is finished (a finished tree is read-only in the UI)
     */
    bool isFinished() const {return finished;}

    /**
     * @brief getName
     * @return the name of the tree
     */
    const QString & getName() const {return treeName;}

    /**
     * @brief getCreationTime
     * @return the creation time of the tree, as an SQLITE timestamp
     */
    const QString & getCreationTime() const{return creationTime;}


    /**
     * @brief PCx_Tree::addNode inserts a node in the tree
     *
     * Add a node in the tree. Check for the maximum number of nodes and if a node with the same name and type exists
     * @param pid the ID of the parent node (PID)
     * @param typeId the ID of the type for the node to insert
     * @param name the name of the node to insert
     * @return the ID of the inserted node, or 0 in case of failure
     */
    unsigned int addNode(unsigned int pid, unsigned int type, const QString &name);


    /**
     * @brief updateNode changes the name and the type of a node
     * @param nodeId the identifier of the node to update
     * @param newName the new name of the node
     * @param newType the identifier of the new type
     * @return true in case of success, false if a node with the same name and type exists, or if the newType is invalid
     */
    bool updateNode(unsigned int nodeId, const QString &newName, unsigned int newType);

    /**
     * @brief PCx_Tree::deleteNode deletes a node and all its children from the database
     * @param nodeId the node to delete
     * @return true on success, false on failure
     */
    bool deleteNode(unsigned int nodeId);

    /**
     * @brief getNumberOfNodes gets the number of nodes in the tree
     * @return the number of nodes in the tree
     */
    unsigned int getNumberOfNodes() const;


    /**
     * @brief getLeavesId gets the identifier of the nodes that are leaves
     * @return a list of leaves ID
     */
    QList<unsigned int> getLeavesId() const;


    /**
     * @brief PCx_Tree::getNodesId gets ID of all the nodes
     * @return The list of all nodes ID
     */
    QList<unsigned int> getNodesId() const;

    /**
     * @brief PCx_Tree::getListOfCompleteNodeNames gets the list of full node names (typeName + " " + nodeName)
     * @return The string list of full node names
     */
    QStringList getListOfCompleteNodeNames() const;

    /**
     * @brief PCx_Tree::getListOfNodeNames gets the list of simple node names (without the name of the type)
     *
     * For example, if a tree contains three nodes "T1 N1", "T1 N2" and "T2 N3"
     * getListOfNodeNames will return ("N1","N2","N3)
     * @return The QStringList of simple node names
     */
    QStringList getListOfNodeNames() const;

    /**
     * @brief getNonLeavesId gets non-leaves
     * @return the list of nodes identifier that are not leaves
     */
    QList<unsigned int> getNonLeavesId() const;

    /**
     * @brief getNodesWithSharedName gets nodes that have the same name but not the same type
     * @return a set of node ids
     */
    QSet<unsigned int> getNodesWithSharedName() const;

    /**
     * @brief isLeaf is the node a leaf ?
     * @param nodeId the node identifier
     * @return true if the node is a leaf, false otherwise
     */
    bool isLeaf(unsigned int nodeId) const;

    /**
     * @brief getTreeDepth computes the depth of the tree
     * @return the maximum depth of the tree (0 for a tree with only a root)
     */
    unsigned int getTreeDepth() const;

    /**
     * @brief getParentId gets the PID of a node
     * @param nodeId the node identifier
     * @return the parent identifier of the node, 0 if the node has no PID
     */
    unsigned int getParentId(unsigned int nodeId) const;

    /**
     * @brief getChildren gets the list of children of a node
     * @param nodeId the node identifier
     * @return the list of its children identifier, sorted by node simple name
     */
    QList<unsigned int> getChildren(unsigned int nodeId=1) const;

    /**
     * @brief getNodeName gets the complete name of a node
     * @param node the node identifier
     * @return the full name of the node, including its type name, joined with a space
     */
    QString getNodeName(unsigned int node) const;

    /**
     * @brief getTypeNameAndNodeName gets the complete name of a node, type name splitted
     * @param node the node identifier
     * @return a pair of QString with the name of the type and the name of the node
     */
    QPair<QString, QString> getTypeNameAndNodeName(unsigned int node) const;


    /**
     * @brief getIdsOfNodesWithThisType gets all nodes that have a specific type
     * @param typeId the type identifier
     * @return a list of node identifier that have the specified type
     */
    QList<unsigned int> getIdsOfNodesWithThisType(unsigned int typeId) const;

    /**
     * @brief getNumberOfNodesWithThisType gets the number of nodes that have a specific type
     * @param typeId the type identifier
     * @return the number of nodes that have the specified type
     */
    unsigned int getNumberOfNodesWithThisType(unsigned int typeId) const;


    /**
     * @brief sortNodesDFS sorts nodes of the tree with a Depth-first Search
     * @param nodes the list of nodes to sort
     * @param currentNode the starting node
     * @return the list of nodes identifier sorted after a Depth-first search
     */
    QList<unsigned int> sortNodesDFS(QList<unsigned int> &nodes, unsigned int currentNode=1) const;

    /**
     * @brief sortNodesBFS sorts nodes of the tree with a Breadth-first Search
     * @param nodes the list of nodes to sort
     * @return the list of nodes identifier sorted after a Breadth-first search
     */
    QList<unsigned int> sortNodesBFS(QList<unsigned int> &nodes) const;


    /**
     * @brief finishTree marks a tree as finished
     *
     * A finished tree is read-only in the UI and is needed to create audits
     * @return true on success
     */
    bool finishTree();


    /**
     * @brief PCx_Tree::nodeExists checks if a node with given name and type already exists
     * @param name : the name of the node
     * @param typeId : the type identifier of the node
     * @return true if the node exists, false otherwise
     */
    bool nodeExists(const QString &name, unsigned int typeId) const;


    /**
     * @brief getNodeIdFromTypeAndNodeName finds the node identifier from its type name and node name
     *
     * The node name is simplified through QString::simplified() before querying the database
     *
     * @param typeAndNodeName a pair of QString with the name of the type and the name of the node
     * @return the node identifier, -1 if not found
     */
    int getNodeIdFromTypeAndNodeName(const QPair<QString, QString> &typeAndNodeName ) const;


    /**
     * @brief toDot draws a tree in DOT format suitable for GraphViz
     * @return the tree in DOT format
     */
    QString toDot() const;

    /**
     * @brief toXLSX saves a tree in XLSX
     *
     * Export a tree to XLSX. The format is four columns, each line is a node
     * Column 1: the name of the type
     * Column 2: the name of the node (without its type name)
     * Column 3: the name of the type of the parent node
     * Column 4: the name of the parent node (without its type name)
     *
     * Column 3 and column 4 are empty for first level nodes.
     *
     * @param fileName the name of the file to save
     * @return true on success
     */
    bool toXLSX(const QString &fileName) const;

    /**
     * @brief getTypeNames returns the list of all types in this tree
     * @return all type names that can be used in this tree.
     */
    const QStringList &getTypeNames() const {return listOfTypeNames;}

    /**
     * @brief idTypeToName gets the type name that corresponds to this identifier
     * @param id the type identifier
     * @return the type name, or an empty QString if the type identifier does not exists in the DB
     */
    QString idTypeToName(unsigned int id) const{if(!isTypeIdValid(id)){qDebug()<<"Missing type";return QString();}else return idTypesToNames[id];}

    /**
     * @brief isTypeIdValid checks if the type identifier exists in the database
     * @param id the type identifier
     * @return true if the type identifier exists, false otherwise
     */
    bool isTypeIdValid(unsigned int id) const{return idTypesToNames.contains(id);}

    /**
     * @brief getAllTypes gets a list of all the types name and identifier
     * @return a list of QPair<typeId,typeName>
     */
    QList<QPair<unsigned int,QString> > getAllTypes() const;

    /**
     * @brief nameToIdType gets the type identifier for the specified type name
     * @param typeName the type to find, will be simplified with QString::simplified()
     * @return the type identifier, -1 if it cannot be found
     */
    int nameToIdType(const QString &typeName) const;

    /**
     * @brief addType adds a new type in the database
     * @param typeName the type to add
     * @return the new type identifier, 0 on error (duplicate or empty type), with a warning displayed
     */
    virtual unsigned int addType(const QString &typeName);

    /**
     * @brief deleteType removes a type from the database
     * @param typeId the type identifier to remove
     * @return true on success, false if the type does not exists, or if it is used by nodes in the tree
     */
    virtual bool deleteType(unsigned int typeId);

protected:

    /**
     * @brief treeId the tree identifier in the database, the table is "arbre_treeId"
     */
    unsigned int treeId;

    /**
     * @brief finished is the tree finished ? A finished tree must be read-only in the UI
     */
    bool finished;

    /**
     * @brief treeName the name of the tree
     */
    QString treeName;

    /**
     * @brief creationTime the SQL timestamp of the creation time of the tree
     */
    QString creationTime;

    /**
     * @brief validateType checks if a type with this name can be added in the database
     * @param newType the type name to check
     * @return true if the type name can be added, false if it already exists or is blank. A warning is displayed in the UI.
     */
    bool validateType(const QString &newType);

    /**
     * @brief initTreeFromDb initializes the tree from the database
     * @return true on success, false if the tree does not exists
     */
    bool initTreeFromDb();

    /**
     * @brief updateNodePid change the parent identifier of a node. Used for D&D
     * @param nodeId the node to change
     * @param newPid the new parent identifier
     */
    void updateNodePid(unsigned int nodeId, unsigned int newPid);


    /**
     * @brief listOfTypeNames the list of all type names that can be used in this tree
     */
    QStringList listOfTypeNames;

    /**
     * @brief idTypeToName a mapping between type identifier and type name
     */
    QHash<unsigned int,QString> idTypesToNames;

    /**
     * @brief initTypesFromDb initializes idTypeToName of listOfTypeNames from the database
     * @return true on success, false on DB error
     */
    bool initTypesFromDb();
};

#endif // PCX_TREE_H
