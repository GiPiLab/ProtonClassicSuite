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
    QByteArray expectedReqResult="359e289380e8ffafd14272646d1e35b02fbafdd53093a76c4af29d9a764a3e01";
    QCOMPARE(hashString(qr.exec()).toHex(),expectedReqResult);
    QVERIFY(qr.canSave("TITI"));
    unsigned int queryId=qr.save("TITI");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    QVERIFY(!qr.canSave("TITI"));
    PCx_QueryRank qrLoad(&audit,queryId);
    QCOMPARE(qrLoad.getName(),QString("TITI"));
    QCOMPARE(qrLoad.getDFRFDIRI(),MODES::DFRFDIRI::DF);
    QCOMPARE(qrLoad.getORED(),PCx_Audit::ORED::OUVERTS);
    QCOMPARE(qrLoad.getGreaterOrSmaller(),PCx_QueryRank::GREATERSMALLER::GREATER);
    QCOMPARE(qrLoad.getNumber(),(unsigned)2);
    QPair<unsigned int,unsigned int> years={2010,2012};
    QCOMPARE(qrLoad.getYears(),years);

    PCx_Query::deleteQuery(auditId,queryId);
    QVERIFY(qr.canSave("TITI"));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));


    PCx_QueryVariation qv(&audit,2,PCx_Audit::ORED::OUVERTS,MODES::DFRFDIRI::DF,
                          PCx_QueryVariation::INCREASEDECREASE::VARIATION,PCx_QueryVariation::PERCENTORPOINTS::PERCENT,
                          PCx_QueryVariation::OPERATORS::NOTEQUAL,0,2010,2012,"TOTI");
    //qDebug()<<qv.exec();
    //qDebug()<<hashString(qv.exec()).toHex();
    QByteArray expectedReqVarResult="b8bd8ca5da32d68c33591e807fd08020fe926d3dbe0e9eb9215741d68819ab4e";
    QCOMPARE(hashString(qv.exec()).toHex(),expectedReqVarResult);
    QVERIFY(qv.canSave("TOTO"));
    queryId=qv.save("TOTO");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    PCx_QueryVariation qvLoad(&audit,queryId);
    QCOMPARE(qvLoad.getName(),QString("TOTO"));
    QCOMPARE(qvLoad.getDFRFDIRI(),MODES::DFRFDIRI::DF);
    QCOMPARE(qvLoad.getIncDec(),PCx_QueryVariation::INCREASEDECREASE::VARIATION);
    QCOMPARE(qvLoad.getOperator(),PCx_QueryVariation::OPERATORS::NOTEQUAL);
    QCOMPARE(qvLoad.getORED(),PCx_Audit::ORED::OUVERTS);
    QCOMPARE(qvLoad.getYears(),years);
    QCOMPARE(qvLoad.getPercentOrPoints(),PCx_QueryVariation::PERCENTORPOINTS::PERCENT);
    QCOMPARE(qvLoad.getValue(),(qint64)0);

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
    QByteArray expectedReqMMResult="c70aa134da68d82362b0ffc47b5cf56331d7ba84303a4bdcbf2ddd25e417c713";
    QCOMPARE(hashString(qm.exec()).toHex(),expectedReqMMResult);
    QVERIFY(qm.canSave("TUTU"));
    queryId=qm.save("TUTU");
    QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
    QVERIFY(!qm.canSave("TUTU"));
    PCx_QueryMinMax qmLoad(&audit,queryId);
    QCOMPARE(qmLoad.getName(),QString("TUTU"));
    QCOMPARE(qmLoad.getDFRFDIRI(),MODES::DFRFDIRI::DF);
    QCOMPARE(qmLoad.getORED(),PCx_Audit::ORED::OUVERTS);
    QCOMPARE(qmLoad.getYears(),years);
    QPair<qint64,qint64> expectedVals={0,NUMBERSFORMAT::doubleToFixedPoint(300000.0)};

    QCOMPARE(qmLoad.getVals(),expectedVals);


    PCx_Query::deleteQuery(auditId,queryId);
    QVERIFY(qm.canSave("TUTU"));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::RANK).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
    QVERIFY(!PCx_Query::getListOfQueriesId(auditId,PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));

    PCx_Audit::deleteAudit(auditId);
    PCx_Tree::deleteTree(treeId);
}

