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
    unsigned int treeId=PCx_Tree::addTree("TESTTREE");
    QVERIFY(PCx_Tree::treeNameExists("TESTTREE"));
    QVERIFY(PCx_Tree::getListOfTreesId().contains(treeId));
    PCx_Tree tree(treeId);
    QCOMPARE(tree.getName(),QString("TESTTREE"));
    QVERIFY(!tree.isFinished());
    PCx_Tree::deleteTree(treeId);
    QVERIFY(!PCx_Tree::treeNameExists("TESTTREE"));

}

QTEST_APPLESS_MAIN(UnitTests)

#include "tst_unittests.moc"
