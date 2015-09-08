#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include <QStandardItemModel>
#include <QMimeData>
#include <QSqlTableModel>
#include "pcx_tree.h"


/**
 * @brief The PCx_TreeModel class provides an editable tree model and type model
 *
 * This class extends PCx_Tree to provides a model suitable for a TreeView. It supports
 * internal drag and drop and node name/type modification. An editable SQLTableModel is provided
 * for modifying type names.
 *
 *
 */
class PCx_TreeModel : public QStandardItemModel,public PCx_Tree
{
    Q_OBJECT

public:

    /**
     * @brief NodeIdUserRole the identifier of the role used to store the node identifier in QStandardItems
     */
    static const int NodeIdUserRole=Qt::UserRole+1;

    /**
     * @brief TypeIdUserRole the identifier of the role used to store the type identifier in QStandardItems
     */
    static const int TypeIdUserRole=Qt::UserRole+2;

    explicit PCx_TreeModel(unsigned int treeId, QObject *parent = 0);
    virtual ~PCx_TreeModel();

    /**
     * @brief dropMimeData supports internal drag and drop in TreeView
     *
     * Supports extended selection
     * Checks validity (no dd outside of the tree), uses an intermediate QStandardItemModel to decode MimeData
     * Updates database
     * Lets QStandardItemModel::dropMimeData do the job with items
     *
     * @return false in case of invalid DD, true on success
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    /**
     * @brief updateTree clears and refreshes the tree model
     * @return true on success
     */
    bool updateTree();

    /**
     * @brief getTypesTableModel returns the editable table model for types
     * @return a pointer to the QSqlTableModel of types
     */
    QSqlTableModel *getTypesTableModel(){return typesTableModel;}

    /**
     * @brief addNode adds a node to the tree, with the parent node identified by a model index
     * @param typeId the type identifier
     * @param name the node name
     * @param pidNodeIndex the model index of the parent
     * @return the ID of the inserted node, or 0 in case of failure
     */
    unsigned int addNode(unsigned int typeId, const QString &name, const QModelIndex &pidNodeIndex);

    /**
     * @brief updateNode changes the name and the type of a node identified by a model index
     * @param nodeIndex the model index of the node to update
     * @param newName the new name of the node
     * @param newType the identifier of the new type
     * @return true in case of success, false if a node with the same name and type exists, or if the newType is invalid
     */
    bool updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType);

    /**
     * @brief deleteNode removes a node identified by a model index and all its children
     * @param nodeIndex the model index of the node to delete
     * @return true on success, false on failure
     */
    bool deleteNode(const QModelIndex &nodeIndex);



    /**
     * @brief getIndexesOfNodesWithThisType finds the indexes of nodes that have a specific type
     * @param typeId the type identifier to find
     * @return a list of model indexes of nodes
     */
    QModelIndexList getIndexesOfNodesWithThisType(unsigned int typeId) const;

    /**
     * @brief addType adds a new type to this tree, and updates the QSqlTableModel
     * @param typeName the type to add
     * @return the new type identifier, 0 on error (duplicate or empty type), with a warning displayed
     */
    unsigned int addType(const QString &typeName);

    /**
     * @brief deleteType removes a type from the database, and updates the QSqlTableModel
     * @param typeId the type identifier to remove
     * @return true on success, false if the type does not exists, or if it is used by nodes in the tree
     */
    bool deleteType(unsigned int typeId);


    /**
     * @brief getIndexOfNodeId gets the QModelIndex of the nodeID
     * @param nodeId the node ID
     * @return the model index of this node in the treeModel
     */
    QModelIndexList getIndexOfNodeId(unsigned int nodeId) const;


    int guessHierarchy();

private:

    /**
     * @brief createChildrenItems populates the QStandardItemModel with tree items
     * @param item the QStandardItem to add children
     * @param nodeId the node identifier of the item
     * @return true on success, false otherwise
     */
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId=0);

    /**
     * @brief createItem creates a tree item in the model
     * @param typeId the type identifier, stored in TypeIdUserRole role
     * @param nodeId the node identifier, stored in NodeIdUserRole role
     * @return the newly created item
     */
    QStandardItem *createItem(unsigned int typeId, unsigned int nodeId);

    /**
     * @brief typesTableModel the QSqlTableModel associated with the table types_[treeId]
     */
    QSqlTableModel *typesTableModel;

    PCx_TreeModel(const PCx_TreeModel &c);
    PCx_TreeModel &operator=(const PCx_TreeModel &);

signals:

    /**
     * @brief typesUpdated signal emitted when a type is modified
     */
    void typesUpdated(void);


private slots:
    /**
     * @brief onTypesModelDataChanged slot called when a type is modified through the user interface
     *
     * Validates the new type name, assumes that only one row is modified at once and updates the DB
     *
     * @param topLeft the model index that have changed
     * @param bottomRight not used
     * @return true on success, false if the new type name is not valid
     */
    bool onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);


};

#endif // PCX_TREEMODEL_H
