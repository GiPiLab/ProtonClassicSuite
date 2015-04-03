#include "utils.h"
#include "pcx_queryminmax.h"
#include "pcx_queryrank.h"
#include "pcx_queryvariation.h"
#include "tst_protonclassicsuiteunittests.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>



void ProtonClassicSuiteUnitTests::testCaseForPCAQueries()
{
    unsigned int treeId=PCx_Tree::addTree("TESTTREE");
    PCx_Tree tree(treeId);
    unsigned int nodeA=tree.addNode(1,1,"NODEA");
    unsigned int nodeB=tree.addNode(1,1,"NODEB");
    tree.addNode(nodeA,2,"NODEC");
    tree.addNode(nodeB,2,"NODED");

    tree.finishTree();
    unsigned int auditId=PCx_Audit::addNewAudit("TESTAUDIT",QList<unsigned int>{2010,2011,2012},treeId);
    PCx_Audit audit(auditId);
    fillTestAudit(&audit);


    PCx_QueryRank qr(&audit,2,PCx_Audit::ORED::OUVERTS,MODES::DFRFDIRI::DF,
                     PCx_QueryRank::GREATERSMALLER::GREATER,2,2010,2012,"REQ1");
    //qDebug()<<qr.exec();
    //qDebug()<<hashString(qr.exec()).toHex();
    QByteArray expectedReqResult="c723ef05ba68ee87249489d9b4eb8104c58c606f6ecd1443fa12363aa515135e";
    QCOMPARE(hashString(qr.exec()).toHex(),expectedReqResult);
    QVERIFY(qr.canSave("TOTO"));
    unsigned int queryId=qr.save("TOTO");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    QVERIFY(!qr.canSave("TOTO"));
    PCx_Query::deleteQuery(auditId,queryId);
    QVERIFY(qr.canSave("TOTO"));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));


    PCx_QueryVariation qv(&audit,2,PCx_Audit::ORED::OUVERTS,MODES::DFRFDIRI::DF,
                          PCx_QueryVariation::INCREASEDECREASE::VARIATION,PCx_QueryVariation::PERCENTORPOINTS::PERCENT,
                          PCx_QueryVariation::OPERATORS::NOTEQUAL,0,2010,2012,"TOTI");
    //qDebug()<<qv.exec();
    //qDebug()<<hashString(qv.exec()).toHex();
    QByteArray expectedReqVarResult="ea21c5d78ff2354bf3cbbfa43b647a417856d7e1dd67726400ed79959daf1d7e";
    QCOMPARE(hashString(qv.exec()).toHex(),expectedReqVarResult);
    QVERIFY(qv.canSave("TOTO"));
    queryId=qv.save("TOTO");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    QVERIFY(!qv.canSave("TOTO"));
    PCx_Query::deleteQuery(auditId,queryId);
    QVERIFY(qv.canSave("TOTO"));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));


    PCx_QueryMinMax qm(&audit,2,PCx_Audit::ORED::OUVERTS,MODES::DFRFDIRI::DF,
                       0,NUMBERSFORMAT::doubleToFixedPoint(300000.0),2010,2012,"TUTU");

    //qDebug()<<qm.exec();
    //qDebug()<<hashString(qm.exec()).toHex();
    QByteArray expectedReqMMResult="8cd6c607e6ac2562960913c132600b302092a80bbdb69ee1a807ea97211abde1";
    QCOMPARE(hashString(qm.exec()).toHex(),expectedReqMMResult);
    QVERIFY(qm.canSave("TUTU"));
    queryId=qm.save("TUTU");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    QVERIFY(!qm.canSave("TUTU"));
    PCx_Query::deleteQuery(auditId,queryId);
    QVERIFY(qm.canSave("TUTU"));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));

    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}

