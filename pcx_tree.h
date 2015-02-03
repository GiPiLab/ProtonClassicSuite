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
     * @param type the ID of the type for the node to insert
     * @param name the name of the node to insert
     * @return the ID of the inserted node, or 0 if the node exists
     */
    unsigned int addNode(unsigned int pid, unsigned int type, const QString &name);

    /**
     * @brief getTypeId gets the type identifier of the node
     * @param nodeId the node identifier
     * @return the type identifier
     */
    unsigned int getTypeId(unsigned int nodeId) const;

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
     * @brief getLeavesId gets the identifier of the nodes that are both leaves and descendants of parentNode
     * @param parentNode the node identifier
     * @return a list of leaves ID
     */
    QList<unsigned int> getLeavesId(unsigned int parentNode) const;


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
    QString idTypeToName(unsigned int id) const{return idTypesToNames[id];}

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


    /**
     * @brief duplicateTree duplicates a tree
     * @param newName the name of the new tree
     * @return the identifier of the new tree, -1 if the tree name exists
     */
    int duplicateTree(const QString &newName);


    /**
     * @brief importTreeFromXLSX import a tree from an XLSX file
     * The file must contain node description in four column :
     * COL1 is the name of the type, COL2 is the name of the node
     * COL3 is the name of the type for the parent of the node
     * COL4 is the name of the parent of the node.
     * If both COL3 and COL4 are empty, the node described in COL1 and COL2 is a first level node
     *
     * Check for duplicates, orphan nodes, multiple ancestors and empty root and print a warning. Stop after the first warning
     *
     * @param fileName the name of the file to import
     * @param treeName the name of the newly imported tree
     * @return the identifier of the newly created tree, or -1 in case of error
     */
    static int importTreeFromXLSX(const QString &fileName,const QString &treeName);

    /**
     * @brief createRandomTree creates a tree with random node names
     * @param name the name of the newly created tree
     * @param nbNodes the number of nodes
     * @return the identifier of the newly created tree
     */
    static int createRandomTree(const QString &name, unsigned int nbNodes);

    /**
     * @brief addTree creates a new tree
     * @param name the name of the tree
     * @return the identifier of the new tree, -1 if the tree name exists
     */
    static int addTree(const QString &name);

    /**
     * @brief deleteTree removes a tree from database
     * @param treeId the tree identifier
     * @return 1 on success, 0 if an audit is attached to this tree, -1 if the tree does not exists
     */
    static int deleteTree(unsigned int treeId);

    /**
     * @brief getListOfTrees gets a list of trees to fill QComboBox
     * @param finishedOnly indicates if the list should contains only finished trees
     * @return a list of QPair<treeId,treeName>
     */
    static QList<QPair<unsigned int,QString> > getListOfTrees(bool finishedOnly=false);


    /**
     * @brief getListOfTreesId gets a list of tree identifiers
     * @param finishedOnly indicates if the list should contains only finished trees
     * @return a list of tree identifiers
     */
    static QList<unsigned int> getListOfTreesId(bool finishedOnly=false);


    /**
     * @brief treeNameExists checks if the tree name exists
     * @param name the name of the tree
     * @return true if the name exists, false otherwise
     */
    static bool treeNameExists(const QString &name);


    /**
     * @brief getListOfDefaultTypes returns a list of default types for new audits
     * @return a QStringList with default types
     */
    static QStringList getListOfDefaultTypes();




    /**
     * @brief getAncestorsId returns the list of ancestors id
     * @param node the current node
     * @return the list of ancestors
     */
    QList<unsigned int> getAncestorsId(unsigned int node) const;
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
     * @brief loadTypesFromDb initializes idTypeToName of listOfTypeNames from the database
     */
    void loadTypesFromDb();

    /**
     * @brief _internalAddTree creates a new without associated types
     *
     * Protected method to creates a tree without sql transaction. Used in addTree and duplicateTree
     * @param name the name of the tree
     * @param createRoot if true creates a root node in the tree
     * @return the identifier of the new tree, call die on database error
     */
    static int _internalAddTree(const QString &name,bool createRoot=true);

    PCx_Tree(const PCx_Tree &c);
    PCx_Tree &operator=(const PCx_Tree &);


};

#endif // PCX_TREE_H
