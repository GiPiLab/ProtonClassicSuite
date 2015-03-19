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


    //Finish a tree
    PCx_Tree tree(treeId);
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
