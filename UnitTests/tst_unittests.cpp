#include <QString>
#include <QtTest>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utils.h"
#include "pcx_tree.h"


class UnitTests : public QObject
{
    Q_OBJECT

public:
    UnitTests();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCaseForTreeManagement();
    void testCaseForTreeNodes();
    void testCaseForTreeNodesOrder();
    void testCaseForTreeImportExport();
    void testCaseForTreeToDot();
};

UnitTests::UnitTests()
{
}

void UnitTests::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();
    QSqlQuery("PRAGMA foreign_keys=ON");
    initializeNewDb();
}

void UnitTests::cleanupTestCase()
{
    QSqlDatabase::database().close();
}

void UnitTests::testCaseForTreeNodes()
{
    unsigned int treeId=PCx_Tree::addTree("TESTTREENODE");
    PCx_Tree tree(treeId);

    //The root as typeId 0
    QCOMPARE(tree.getTypeId(1),(unsigned)0);
    //Only the root in the tree
    QCOMPARE(tree.getNodesId().size(),1);
    QCOMPARE(tree.getNumberOfNodes(),(unsigned)1);
    //No node with type other than root
    for(int i=1;i<=tree.getTypeNames().size();i++)
    {
        QCOMPARE(tree.getNumberOfNodesWithThisType(i),(unsigned)0);
    }

    //Add/delete type
    unsigned int typeId=tree.addType("TESTTYPE");
    QVERIFY(tree.getTypeNames().contains("TESTTYPE"));
    QVERIFY(tree.getAllTypes().contains(QPair<unsigned int,QString>{typeId,"TESTTYPE"}));
    QCOMPARE(tree.nameToIdType("TESTTYPE"),(int)typeId);

    tree.deleteType(typeId);
    QVERIFY(!tree.getTypeNames().contains("TESTTYPE"));
    QVERIFY(!tree.getAllTypes().contains(QPair<unsigned int,QString>{typeId,"TESTTYPE"}));

    //Add a node
    unsigned int nodeId=tree.addNode(1,1,"TESTNODE");
    QCOMPARE(tree.getNodesId().size(),2);
    QCOMPARE(tree.getNumberOfNodes(),(unsigned)2);
    QCOMPARE(tree.getNumberOfNodesWithThisType(1),(unsigned)1);
    //Check the correct type
    QCOMPARE(tree.getTypeId(nodeId),(unsigned)1);
    //Check the correct node name (the type name + TESTNODE)
    QCOMPARE(tree.getNodeName(nodeId),QString(tree.getTypeNameAndNodeName(nodeId).first+" "+"TESTNODE"));
    QVERIFY(tree.getListOfCompleteNodeNames().contains(tree.getTypeNameAndNodeName(nodeId).first+" "+"TESTNODE"));

    //Check if we can retrieve the id from the type and node names
    QCOMPARE(tree.getNodeIdFromTypeAndNodeName(tree.getTypeNameAndNodeName(nodeId)),(int)nodeId);

    //Update a node
    tree.updateNode(nodeId,"TESTNODENEW",2);
    QCOMPARE(tree.getNodeName(nodeId),QString(tree.getTypeNameAndNodeName(nodeId).first+" "+"TESTNODENEW"));

    //Nodes with shared name
    unsigned int newNodeId=tree.addNode(1,1,"TESTNODENEW");
    QVERIFY(tree.getNodesWithSharedName().contains(nodeId));
    QVERIFY(tree.getNodesWithSharedName().contains(newNodeId));
    tree.deleteNode(newNodeId);

    //Delete a node
    tree.deleteNode(nodeId);
    QCOMPARE(tree.getNumberOfNodes(),(unsigned)1);
    QCOMPARE(tree.getNumberOfNodesWithThisType(1),(unsigned)0);
    PCx_Tree::deleteTree(treeId);
}

void UnitTests::testCaseForTreeImportExport()
{
    unsigned int randomTreeId=PCx_Tree::createRandomTree("RANDOMTREEIMPORTEXPORT",100);
    PCx_Tree randomTree(randomTreeId);
    QList<unsigned int> nodeIds=randomTree.getNodesId();
    QList<unsigned int> randomNodesSortedDFS=randomTree.sortNodesDFS(nodeIds);
    QStringList randomNodesNamesSortedDFS;
    foreach(unsigned int node,randomNodesSortedDFS)
    {
        randomNodesNamesSortedDFS.append(randomTree.getNodeName(node));
    }

    QString tmpFileName=QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/"+generateUniqueFileName(".xlsx");

    randomTree.toXLSX(tmpFileName);
    QFileInfo tmpFileInfo(tmpFileName);
    QVERIFY(tmpFileInfo.size()>0);

    unsigned int importedTreeId=PCx_Tree::importTreeFromXLSX(tmpFileName,"RANDOMTREEIMPORTED");
    PCx_Tree importedTree(importedTreeId);
    QCOMPARE(randomTree.getNumberOfNodes(),importedTree.getNumberOfNodes());
    QStringList randomCompleteNodeNames=randomTree.getListOfCompleteNodeNames();
    QStringList importedCompleteNodeNames=importedTree.getListOfCompleteNodeNames();
    randomCompleteNodeNames.sort();
    importedCompleteNodeNames.sort();
    QCOMPARE(randomCompleteNodeNames,importedCompleteNodeNames);
    QList<unsigned int>importedNodeIds=importedTree.getNodesId();
    QList<unsigned int> importedNodesSortedDFS=importedTree.sortNodesDFS(importedNodeIds);
    QStringList importedNodesNamesSortedDFS;
    foreach(unsigned int node,importedNodesSortedDFS)
    {
        importedNodesNamesSortedDFS.append(importedTree.getNodeName(node));
    }

    //We must compare on complete node names because node ID differs during import
    QCOMPARE(randomNodesNamesSortedDFS,importedNodesNamesSortedDFS);
    PCx_Tree::deleteTree(randomTreeId);
    PCx_Tree::deleteTree(importedTreeId);
    QFile fi(tmpFileName);
    fi.remove();
}

void UnitTests::testCaseForTreeToDot()
{
    unsigned int treeId=PCx_Tree::addTree("TESTTODOT");
    PCx_Tree tree(treeId);

    /*
     * ROOT
     * |_NODE1
     *      |_NODE3
     *          |_NODE5
     *      |_NODE4
     *
     * |_NODE2
     *      |_NODE6
     *          |_NODE7
     * */

    unsigned int node1=tree.addNode(1,1,"1_NODE1");
    unsigned int node2=tree.addNode(1,1,"2_NODE2");
    unsigned int node3=tree.addNode(node1,1,"10_NODE3");
    unsigned int node4leaf=tree.addNode(node1,1,"11_NODE4");
    unsigned int node5leaf=tree.addNode(node3,1,"100_NODE5");
    unsigned int node6=tree.addNode(node2,1,"20_NODE6");
    unsigned int node7leaf=tree.addNode(node6,1,"200_NODE7");

    //Verify tree structure
    QVERIFY(tree.isLeaf(node4leaf));
    QVERIFY(tree.isLeaf(node5leaf));
    QVERIFY(tree.isLeaf(node7leaf));
    QVERIFY(!tree.isLeaf(node1));
    QVERIFY(!tree.isLeaf(node2));
    QVERIFY(!tree.isLeaf(node3));
    QVERIFY(!tree.isLeaf(node6));
    QCOMPARE(tree.getParentId(node1),(unsigned)1);
    QCOMPARE(tree.getParentId(node2),(unsigned)1);
    QCOMPARE(tree.getParentId(node3),node1);
    QCOMPARE(tree.getParentId(node4leaf),node1);
    QCOMPARE(tree.getParentId(node5leaf),node3);
    QCOMPARE(tree.getParentId(node6),node2);
    QCOMPARE(tree.getParentId(node7leaf),node6);

    QString dot=tree.toDot();

    QString attendedDot="graph g{\n"
                        "rankdir=LR;\n"
            "1 [label=\"Racine\"];\n"
    "2 [label=\"Maire adjoint 1_NODE1\"];\n"
    "3 [label=\"Maire adjoint 2_NODE2\"];\n"
    "4 [label=\"Maire adjoint 10_NODE3\"];\n"
    "5 [label=\"Maire adjoint 11_NODE4\"];\n"
    "7 [label=\"Maire adjoint 20_NODE6\"];\n"
    "6 [label=\"Maire adjoint 100_NODE5\"];\n"
    "8 [label=\"Maire adjoint 200_NODE7\"];\n"
        "\t1--2;\n"
        "\t1--3;\n"
        "\t2--4;\n"
        "\t2--5;\n"
        "\t3--7;\n"
        "\t4--6;\n"
        "\t7--8;\n}\n";

    QCOMPARE(dot,attendedDot);
    PCx_Tree::deleteTree(treeId);
}

void UnitTests::testCaseForTreeNodesOrder()
{
    unsigned int treeId=PCx_Tree::addTree("TESTTREENODE");
    PCx_Tree tree(treeId);
    //Check tree structure
    //Will test both guessHierarchy and sortDFS/BFS
    /*
     * ROOT
     * |_NODE1
     *      |_NODE3
     *          |_NODE5
     *      |_NODE4
     *
     * |_NODE2
     *      |_NODE6
     *          |_NODE7
     * */

    //A flat set of nodes
    unsigned int node1=tree.addNode(1,1,"1_NODE1CHILDOFROOT");
    unsigned int node2=tree.addNode(1,1,"2_NODE2CHILDOFROOT");
    unsigned int node3=tree.addNode(1,1,"10_NODE3CHILDOFNODE1");
    unsigned int node4leaf=tree.addNode(1,1,"11_NODE4CHILDOFNODE1");
    unsigned int node5leaf=tree.addNode(1,1,"100_NODE5CHILDOFNODE3");
    unsigned int node6=tree.addNode(1,1,"20_NODE6CHILDOFNODE2");
    unsigned int node7leaf=tree.addNode(1,1,"200_NODE7CHILDOFNODE6");

    //Ensure it is flat
    QCOMPARE(tree.getChildren(1).size(),7);
    QCOMPARE(tree.getLeavesId().size(),7);
    QCOMPARE(tree.getDescendantsId(1).size(),7);

    //Check that "guessHierarchy" produces the correct result
    tree.guessHierarchy();
    QVERIFY(tree.getChildren(1).contains(node1));
    QVERIFY(tree.getChildren(1).contains(node2));
    QVERIFY(!tree.getChildren(1).contains(node3));
    QVERIFY(!tree.getChildren(1).contains(node4leaf));
    QVERIFY(!tree.getChildren(1).contains(node5leaf));
    QVERIFY(!tree.getChildren(1).contains(node6));
    QVERIFY(!tree.getChildren(1).contains(node7leaf));
    QCOMPARE(tree.getParentId(node4leaf),node1);
    QCOMPARE(tree.getParentId(node7leaf),node6);
    QCOMPARE(tree.getParentId(node6),node2);
    QCOMPARE(tree.getParentId(node2),(unsigned)1);
    QCOMPARE(tree.getParentId(node3),node1);
    QCOMPARE(tree.getParentId(node1),(unsigned)1);
    QCOMPARE(tree.getChildren(1).size(),2);
    QCOMPARE(tree.getDescendantsId(1).size(),7);
    QCOMPARE(tree.getChildren(node2).size(),1);
    QCOMPARE(tree.getDescendantsId(node1).size(),3);
    QCOMPARE(tree.getChildren(node1).size(),2);
    QVERIFY(tree.isLeaf(node4leaf));
    QVERIFY(tree.isLeaf(node5leaf));
    QVERIFY(tree.isLeaf(node7leaf));
    QVERIFY(tree.getAncestorsId(node5leaf).contains(node1));
    QVERIFY(tree.getAncestorsId(node5leaf).contains(node3));
    QVERIFY(tree.getLeavesId(node1).contains(node5leaf));
    QVERIFY(tree.getLeavesId(node1).contains(node4leaf));
    QVERIFY(!tree.getLeavesId(node1).contains(node7leaf));
    QVERIFY(!tree.getLeavesId(1).contains(1));
    QVERIFY(!tree.getLeavesId(1).contains(node1));
    QVERIFY(tree.getLeavesId().contains(node5leaf));
    QVERIFY(tree.getLeavesId().contains(node4leaf));
    QVERIFY(tree.getLeavesId().contains(node7leaf));
    QVERIFY(!tree.getLeavesId().contains(1));
    QVERIFY(!tree.getLeavesId().contains(node1));

    //Update node PID
    tree.updateNodePid(node7leaf,1);
    QVERIFY(tree.getChildren(1).contains(node7leaf));
    tree.updateNodePid(node7leaf,node6);

    //Tree traversal
    QList<unsigned int> nodeList=tree.getNodesId();
    QList<unsigned int> sortedBFS=tree.sortNodesBFS(nodeList);
    QList<unsigned int> sortedDFS=tree.sortNodesDFS(nodeList,1);
    QList<unsigned int> requiredBFS={1,node1,node2,node3,node4leaf,node6,node5leaf,node7leaf};
    QList<unsigned int> requiredDFS={1,node1,node3,node5leaf,node4leaf,node2,node6,node7leaf};
    QCOMPARE(sortedBFS,requiredBFS);
    QCOMPARE(sortedDFS,requiredDFS);


    PCx_Tree::deleteTree(treeId);
}





/**
 * @brief UnitTests::testCaseForTreeManagement tests for tree management without taking nodes into account
 */
void UnitTests::testCaseForTreeManagement()
{
    //Add a tree
    unsigned int treeId=PCx_Tree::addTree("TESTTREE");
    QVERIFY(PCx_Tree::treeNameExists("TESTTREE"));

    QVERIFY(PCx_Tree::getListOfTreesId().contains(treeId));
    QList<QPair<unsigned int,QString> > trees=PCx_Tree::getListOfTrees();

    QPair<unsigned int,QString> aTree;
    bool found=false;
    foreach(aTree,trees)
    {
        if(aTree.first==treeId && aTree.second.contains("TESTTREE"))
            found=true;
    }
    QVERIFY(found==true);
    PCx_Tree tree(treeId);
    QCOMPARE(tree.getName(),QString("TESTTREE"));
    QCOMPARE(tree.getTreeId(),treeId);

    //Random tree
    unsigned int randomTreeId=PCx_Tree::createRandomTree("RANDOMTREE",20);
    QVERIFY(PCx_Tree::getListOfTreesId().contains(randomTreeId));
    trees.clear();
    trees=PCx_Tree::getListOfTrees();

    found=false;
    foreach(aTree,trees)
    {
        if(aTree.first==randomTreeId && aTree.second.contains("RANDOMTREE"))
            found=true;
    }
    QVERIFY(found==true);
    PCx_Tree randomTree(randomTreeId);
    QCOMPARE(randomTree.getNodesId().size(),20);

    PCx_Tree::deleteTree(randomTreeId);

    //Finish a tree
    QCOMPARE(tree.getName(),QString("TESTTREE"));
    QVERIFY(!tree.isFinished());
    QVERIFY(!PCx_Tree::getListOfTreesId(true).contains(treeId));
    tree.finishTree();
    QVERIFY(tree.isFinished());
    QVERIFY(PCx_Tree::getListOfTreesId(true).contains(treeId));

    //Duplicate a tree
    unsigned int dupTreeId=tree.duplicateTree("TESTTREEDUP");
    QVERIFY(PCx_Tree::getListOfTreesId().contains(dupTreeId));
    QVERIFY(PCx_Tree::treeNameExists("TESTTREEDUP"));
    trees.clear();
    trees=PCx_Tree::getListOfTrees();

    found=false;
    foreach(aTree,trees)
    {
        if(aTree.first==dupTreeId && aTree.second.contains("TESTTREEDUP"))
            found=true;
    }
    QVERIFY(!PCx_Tree::getListOfTreesId(true).contains(dupTreeId));

    //Delete a tree
    PCx_Tree::deleteTree(treeId);
    QVERIFY(!PCx_Tree::treeNameExists("TESTTREE"));
    QVERIFY(!PCx_Tree::getListOfTreesId().contains(treeId));
    QCOMPARE(PCx_Tree::getListOfTreesId().size(),1);
    PCx_Tree::deleteTree(dupTreeId);
    QCOMPARE(PCx_Tree::getListOfTreesId().size(),0);
}

QTEST_APPLESS_MAIN(UnitTests)

#include "tst_unittests.moc"
