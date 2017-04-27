#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utils.h"
#include "pcx_reporting.h"
#include "tst_protonclassicsuiteunittests.h"


void ProtonClassicSuiteUnitTests::testCaseForReportingManagement()
{
    unsigned int treeId=PCx_Tree::createRandomTree("RANDOMTREE",20);
    PCx_Tree tree(treeId);
    tree.finishTree();

    QVERIFY(!PCx_Reporting::reportingNameExists("Toto"));
    QVERIFY(PCx_Reporting::getListOfReportings().isEmpty());
    unsigned int reportingId=PCx_Reporting::addNewReporting("Toto",treeId);
    QVERIFY(PCx_Reporting::reportingNameExists("Toto"));
    QList<QPair<unsigned int,QString>> listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> pair;
    bool contains=false;
    foreach(pair,listOfReportings)
    {
        if(pair.second.contains("Toto"))
            contains=true;
    }
    QVERIFY(contains);

    PCx_Reporting reporting(reportingId);
    QCOMPARE(reporting.getAttachedTreeId(),treeId);
    QCOMPARE(reporting.getReportingName(),QString("Toto"));

    PCx_Reporting::deleteReporting(reportingId);
    QVERIFY(!PCx_Reporting::reportingNameExists("Toto"));
    QVERIFY(PCx_Reporting::getListOfReportings().isEmpty());
    PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForReportingNodes()
{
    unsigned int treeId=PCx_Tree::createRandomTree("RANDOMTREE",20);
    PCx_Tree tree(treeId);
    tree.finishTree();
    unsigned int reportingId=PCx_Reporting::addNewReporting("Toto",treeId);
    PCx_Reporting reporting(reportingId);

    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::DF),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::RF),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::DI),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::RI),QDate());

    unsigned int firstLeafId=tree.getLeavesId().constFirst();
    unsigned int lastLeafId=tree.getLeavesId().constLast();


    for(int i=0;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
    {
        if(i==(int)PCx_Reporting::OREDPCR::DISPONIBLES)
        {
            continue;
        }
        reporting.setLeafValues(firstLeafId,MODES::DFRFDIRI::DF,QDate(2015,01,01),QMap<PCx_Reporting::OREDPCR,double>{{(PCx_Reporting::OREDPCR)i,10.0}});
        reporting.setLeafValues(lastLeafId,MODES::DFRFDIRI::DF,QDate(2015,01,01),QMap<PCx_Reporting::OREDPCR,double>{{(PCx_Reporting::OREDPCR)i,20.0}});
        QCOMPARE(reporting.getNodeValue(firstLeafId,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(10.0));
        QCOMPARE(reporting.getNodeValue(lastLeafId,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(20.0));
    }

    for(int i=0;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
    {
        if(i==(int)PCx_Reporting::OREDPCR::DISPONIBLES)
        {
            continue;
        }
        QCOMPARE(reporting.getNodeValue(1,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(30.0));
    }

    PCx_Reporting::deleteReporting(reportingId);
    PCx_Tree::deleteTree(treeId);
}
