#ifndef PCX_TREEMANAGE_H
#define PCX_TREEMANAGE_H

#include <QString>
#include <QPair>


/**
 * @brief The PCx_TreeManage namespace contains functions to manage trees.
 */
namespace PCx_TreeManage{

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
    int importTreeFromXLSX(const QString &fileName,const QString &treeName);

    /**
     * @brief createRandomTree creates a tree with random node names
     * @param name the name of the newly created tree
     * @param nbNodes the number of nodes
     * @return the identifier of the newly created tree
     */
    int createRandomTree(const QString &name, unsigned int nbNodes);

    /**
     * @brief duplicateTree duplicates a tree
     * @param treeId the identifier of the tree to duplicate
     * @param newName the name of the new tree
     * @return the identifier of the new tree, -1 on error or if the tree name exists
     */
    int duplicateTree(unsigned int treeId,const QString &newName);

    /**
     * @brief addTree creates a new tree
     * @param name the name of the tree
     * @param createRoot indicates if the new tree will contains a root, "false" to duplicate a tree
     * @param addTypes indicates if the new tree will contains a list of default types, "false" to duplicate a tree
     * @return the identifier of the new tree, -1 on error or if the tree name exists
     */
    int addTree(const QString &name, bool createRoot=true, bool addTypes=true);

    /**
     * @brief deleteTree removes a tree from database
     * @param treeId the tree identifier
     * @return 1 on success, 0 if an audit is attached to this tree, -1 on error or if the tree does not exists
     */
    int deleteTree(unsigned int treeId);

    /**
     * @brief getListOfTrees gets a list of trees to fill QComboBox
     * @param finishedOnly indicates if the list should contains only finished trees
     * @return a list of QPair<treeId,treeName>
     */
    QList<QPair<unsigned int,QString> > getListOfTrees(bool finishedOnly=false);

    /**
     * @brief treeNameExists checks if the tree name exists
     * @param name the name of the tree
     * @return true if the name exists, false otherwise
     */
    bool treeNameExists(const QString &name);

}

#endif // PCX_TREEMANAGE_H
