/*
* ProtonClassicSuite
* 
* Copyright Thibault Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/
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

