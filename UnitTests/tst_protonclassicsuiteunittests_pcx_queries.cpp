/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
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
#include "pcx_queryminmax.h"
#include "pcx_queryrank.h"
#include "pcx_queryvariation.h"
#include "tst_protonclassicsuiteunittests.h"
#include "utils.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

void ProtonClassicSuiteUnitTests::testCaseForPCAQueries() {
  unsigned int treeId = PCx_Tree::addTree("TESTTREE");
  PCx_Tree tree(treeId);
  unsigned int nodeA = tree.addNode(1, 1, "NODEA");
  unsigned int nodeB = tree.addNode(1, 1, "NODEB");
  tree.addNode(nodeA, 2, "NODEC");
  tree.addNode(nodeB, 2, "NODED");

  tree.finishTree();
  unsigned int auditId = PCx_Audit::addNewAudit("TESTAUDIT", QList<int>{2010, 2011, 2012}, treeId);
  PCx_Audit audit(auditId);
  fillTestAudit(&audit);

  PCx_QueryRank qr(&audit, 2, PCx_Audit::ORED::OUVERTS, MODES::DFRFDIRI::DF, PCx_QueryRank::GREATERSMALLER::GREATER, 2,
                   2010, 2012, "REQ1");

  auto expectedReqResultString = QStringLiteral(
      "<h4>Requ&ecirc;te REQ1</h4><p><i>Noeuds du type [Conseiller] dont les crédits ouverts des dépenses de "
      "fonctionnement sont parmi les [2] plus grands entre 2010 et 2012</i></p><table class='req2' cellpadding='5' "
      "align='center' style='margin-left:auto;margin-right:auto;'><tr><th>&nbsp;</th><th>année</th><th>crédits "
      "ouverts</th></tr><tr><td>4. Conseiller NODEC</td><td align='right'>2011</td><td "
      "align='right'>394 754,00</td></tr><tr><td>5. Conseiller NODED</td><td align='right'>2012</td><td "
      "align='right'>369 859,00</td></tr></table>");

  QCOMPARE(qr.exec(), expectedReqResultString);
  QVERIFY(qr.canSave("TITI"));
  unsigned int queryId = qr.save("TITI");
  QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
  QVERIFY(!qr.canSave("TITI"));
  PCx_QueryRank qrLoad(&audit, queryId);
  QCOMPARE(qrLoad.getName(), QString("TITI"));
  QCOMPARE(qrLoad.getDFRFDIRI(), MODES::DFRFDIRI::DF);
  QCOMPARE(qrLoad.getORED(), PCx_Audit::ORED::OUVERTS);
  QCOMPARE(qrLoad.getGreaterOrSmaller(), PCx_QueryRank::GREATERSMALLER::GREATER);
  QCOMPARE(qrLoad.getNumber(), (unsigned)2);
  QPair<int, int> years = {2010, 2012};
  QCOMPARE(qrLoad.getYears(), years);

  PCx_Query::deleteQuery(auditId, queryId);
  QVERIFY(qr.canSave("TITI"));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));

  PCx_QueryVariation qv(&audit, 2, PCx_Audit::ORED::OUVERTS, MODES::DFRFDIRI::DF,
                        PCx_QueryVariation::INCREASEDECREASE::VARIATION, PCx_QueryVariation::PERCENTORPOINTS::PERCENT,
                        PCx_QueryVariation::OPERATORS::NOTEQUAL, 0, 2010, 2012, "TOTI");

  auto expectedReqVarResult = QStringLiteral(
      "<h4>Requ&ecirc;te TOTI</h4><p><i>Noeuds du type [Conseiller] dont les crédits ouverts des dépenses de "
      "fonctionnement ont connu une variation différente de 0,00% entre 2010 et 2012</i></p><table class='req1' "
      "align='center' cellpadding='5' "
      "style='margin-left:auto;margin-right:auto'><tr><th>&nbsp;</th><th>2010</th><th>2012</th><th>variation</"
      "th><tr><td>4. Conseiller NODEC</td><td align='right'>65 678,30</td><td align='right'>218 632,00</td><td "
      "align='right'>232,88 %</td></tr><tr><td>5. Conseiller NODED</td><td align='right'>66 154,30</td><td "
      "align='right'>369 859,00</td><td align='right'>459,08 %</td></tr></table>");
  QCOMPARE(qv.exec(), expectedReqVarResult);
  QVERIFY(qv.canSave("TOTO"));
  queryId = qv.save("TOTO");
  QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
  PCx_QueryVariation qvLoad(&audit, queryId);
  QCOMPARE(qvLoad.getName(), QString("TOTO"));
  QCOMPARE(qvLoad.getDFRFDIRI(), MODES::DFRFDIRI::DF);
  QCOMPARE(qvLoad.getIncDec(), PCx_QueryVariation::INCREASEDECREASE::VARIATION);
  QCOMPARE(qvLoad.getOperator(), PCx_QueryVariation::OPERATORS::NOTEQUAL);
  QCOMPARE(qvLoad.getORED(), PCx_Audit::ORED::OUVERTS);
  QCOMPARE(qvLoad.getYears(), years);
  QCOMPARE(qvLoad.getPercentOrPoints(), PCx_QueryVariation::PERCENTORPOINTS::PERCENT);
  QCOMPARE(qvLoad.getValue(), (qint64)0);

  QVERIFY(!qv.canSave("TOTO"));
  PCx_Query::deleteQuery(auditId, queryId);
  QVERIFY(qv.canSave("TOTO"));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));

  PCx_QueryMinMax qm(&audit, 2, PCx_Audit::ORED::OUVERTS, MODES::DFRFDIRI::DF, 0,
                     NUMBERSFORMAT::doubleToFixedPoint(300000.0), 2010, 2012, "TUTU");

  auto expectedReqMMResult =
      QStringLiteral("<h4>Requ&ecirc;te TUTU</h4><p><i>Noeuds du type [Conseiller] dont les crédits ouverts des "
                     "dépenses de fonctionnement sont compris entre 0,00€ et 300 000,00€ entre 2010 et "
                     "2012</i></p><table class='req3' cellpadding='5' align='center' "
                     "style='margin-left:auto;margin-right:auto;'><tr><th>&nbsp;</th><th>année</th><th>crédits "
                     "ouverts</th></tr><tr><td>4. Conseiller NODEC</td><td align='right'>2010</td><td "
                     "align='right'>65 678,30</td></tr><tr><td>4. Conseiller NODEC</td><td align='right'>2012</td><td "
                     "align='right'>218 632,00</td></tr><tr><td>5. Conseiller NODED</td><td align='right'>2010</td><td "
                     "align='right'>66 154,30</td></tr><tr><td>5. Conseiller NODED</td><td align='right'>2011</td><td "
                     "align='right'>68 857,10</td></tr></table>");
  QCOMPARE(qm.exec(), expectedReqMMResult);
  QVERIFY(qm.canSave("TUTU"));
  queryId = qm.save("TUTU");
  QVERIFY(PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));
  QVERIFY(!qm.canSave("TUTU"));
  PCx_QueryMinMax qmLoad(&audit, queryId);
  QCOMPARE(qmLoad.getName(), QString("TUTU"));
  QCOMPARE(qmLoad.getDFRFDIRI(), MODES::DFRFDIRI::DF);
  QCOMPARE(qmLoad.getORED(), PCx_Audit::ORED::OUVERTS);
  QCOMPARE(qmLoad.getYears(), years);
  QPair<qint64, qint64> expectedVals = {0, NUMBERSFORMAT::doubleToFixedPoint(300000.0)};

  QCOMPARE(qmLoad.getVals(), expectedVals);

  PCx_Query::deleteQuery(auditId, queryId);
  QVERIFY(qm.canSave("TUTU"));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::RANK).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::VARIATION).contains(queryId));
  QVERIFY(!PCx_Query::getListOfQueriesId(auditId, PCx_Query::QUERIESTYPES::MINMAX).contains(queryId));

  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}
