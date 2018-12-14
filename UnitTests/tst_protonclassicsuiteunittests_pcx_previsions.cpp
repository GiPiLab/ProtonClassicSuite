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
#include "pcx_prevision.h"
#include "pcx_previsionitem.h"
#include "pcx_previsionitemcriteria.h"
#include "tst_protonclassicsuiteunittests.h"
#include "utils.h"

void ProtonClassicSuiteUnitTests::testCaseForPCBPrevision() {
  QVERIFY(PCx_Prevision::getListOfPrevisions().isEmpty());
  unsigned int treeId = PCx_Tree::addTree("TESTTREE");
  PCx_Tree tree(treeId);
  tree.addNode(1, 1, "NODE1");
  tree.finishTree();
  unsigned int auditId = PCx_Audit::addNewAudit("AUDITTEST", QList<int>{2010, 2011, 2012, 2013}, treeId);
  PCx_Audit audit(auditId);
  audit.finishAudit();
  unsigned int prevId = PCx_Prevision::addNewPrevision(auditId, "PREVTEST");
  QVERIFY(!PCx_Prevision::getListOfPrevisions().isEmpty());
  PCx_Prevision prev(prevId);
  QCOMPARE(prev.getAttachedAuditId(), auditId);
  QCOMPARE(prev.getAttachedTreeId(), treeId);
  QCOMPARE(prev.getPrevisionId(), prevId);
  QCOMPARE(prev.getPrevisionName(), QString("PREVTEST"));
  QVERIFY(prev.isPrevisionEmpty());
  QVERIFY(PCx_Prevision::previsionNameExists("PREVTEST"));
  PCx_Prevision::deletePrevision(prevId);
  QVERIFY(!PCx_Prevision::previsionNameExists("PREVTEST"));
  QVERIFY(PCx_Prevision::getListOfPrevisions().isEmpty());
  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForPCBPrevisionItemCriteria() {
  unsigned int treeId = PCx_Tree::addTree("TESTTREE");
  PCx_Tree tree(treeId);
  unsigned int node1 = tree.addNode(1, 1, "NODE1");
  tree.finishTree();
  unsigned int auditId = PCx_Audit::addNewAudit("AUDITTEST", QList<int>{2010, 2011, 2012, 2013}, treeId);

  PCx_Audit audit(auditId);
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::DF, 2010,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 10.0}, {PCx_Audit::ORED::REALISES, 8.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::DF, 2011,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 15.0}, {PCx_Audit::ORED::REALISES, 14.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::DF, 2012,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 16.0}, {PCx_Audit::ORED::REALISES, 17.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::DF, 2013,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 20.0}, {PCx_Audit::ORED::REALISES, 19.0}});

  qint64 val;
  QString serialized;
  PCx_PrevisionItemCriteria criteria2;

  // MINIMUM
  PCx_PrevisionItemCriteria criteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(10.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(10.0));
  criteria2.unserialize(serialized);
  val = criteria2.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(10.0));

  // MAXIMUM
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(20.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(20.0));

  // LASTVALUE
  criteria =
      PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::LASTVALUE, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(20.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(20.0));

  // FIXEDVALUE
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUE,
                                       PCx_Audit::ORED::OUVERTS, NUMBERSFORMAT::doubleToFixedPoint(13.5));
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(13.5));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);

  // FIXEDVALUE2
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(13.5));
  criteria =
      PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION,
                                PCx_Audit::ORED::OUVERTS, NUMBERSFORMAT::doubleToFixedPoint(13.5));
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(13.5));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(13.5));

  // AVERAGE
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::AVERAGE, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(15.25));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(15.25));

  // PERCENT
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT, PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(10.0));
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(2.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(2.0));

  // REGLIN
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::REGLIN, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(23.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::DF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(23.0));

  // The same with large values
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::RF, 2010,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, -MAX_NUM}, {PCx_Audit::ORED::REALISES, 8.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::RF, 2011,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 0.0}, {PCx_Audit::ORED::REALISES, 14.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::RF, 2012,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, 10.0}, {PCx_Audit::ORED::REALISES, 17.0}});
  audit.setLeafValues(
      node1, MODES::DFRFDIRI::RF, 2013,
      QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, MAX_NUM}, {PCx_Audit::ORED::REALISES, 19.0}});

  // MINIMUM
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));
  criteria2.unserialize(serialized);
  val = criteria2.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(-MAX_NUM));

  // MAXIMUM
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM));

  // AVERAGE
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::AVERAGE, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(2.5));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(2.5));

  // PERCENT
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT, PCx_Audit::ORED::OUVERTS,
                                       NUMBERSFORMAT::doubleToFixedPoint(10.0));
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM / 10.0));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(MAX_NUM / 10.0));

  // REGLIN
  criteria = PCx_PrevisionItemCriteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::REGLIN, PCx_Audit::ORED::OUVERTS);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(6442450947.5));
  serialized = criteria.serialize();
  criteria = PCx_PrevisionItemCriteria(serialized);
  val = criteria.compute(auditId, MODES::DFRFDIRI::RF, node1);
  QCOMPARE(val, NUMBERSFORMAT::doubleToFixedPoint(6442450947.5));

  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForPCBPrevisionItemDispatch() {
  unsigned int treeId = PCx_Tree::addTree("TESTTREE");
  PCx_Tree tree(treeId);
  /*
   * ROOT
   * |_NODE1
   * |  |__LEAF1
   * |  |__LEAF1B
   * |_NODE2
   * |  |__LEAF2
   */
  unsigned int node1 = tree.addNode(1, 1, "NODE1");
  unsigned int leaf1 = tree.addNode(node1, 2, "LEAF1");
  unsigned int leaf1b = tree.addNode(node1, 2, "LEAF1B");
  unsigned int node2 = tree.addNode(1, 1, "NODE2");
  unsigned int leaf2 = tree.addNode(node2, 2, "LEAF2");
  tree.finishTree();
  unsigned int auditId = PCx_Audit::addNewAudit("AUDITTEST", QList<int>{2010, 2011, 2012, 2013}, treeId);

  PCx_Audit audit(auditId);
  fillTestAudit(&audit);
  audit.finishAudit();
  unsigned int prevId = PCx_Prevision::addNewPrevision(auditId, "PREVTEST");
  PCx_Prevision prevision(prevId);

  PCx_PrevisionItem prevItemNode1(&prevision, MODES::DFRFDIRI::DF, node1, 2014);
  PCx_PrevisionItem prevItemLeaf1(&prevision, MODES::DFRFDIRI::DF, leaf1, 2014);
  PCx_PrevisionItem prevItemLeaf1b(&prevision, MODES::DFRFDIRI::DF, leaf1b, 2014);
  PCx_PrevisionItem prevItemNode2(&prevision, MODES::DFRFDIRI::DF, node2, 2014);
  PCx_PrevisionItem prevItemLeaf2(&prevision, MODES::DFRFDIRI::DF, leaf2, 2014);
  PCx_PrevisionItem prevRoot(&prevision, MODES::DFRFDIRI::DF, 1, 2014);

  PCx_PrevisionItemCriteria criteriaMIN(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM,
                                        PCx_Audit::ORED::OUVERTS);
  PCx_PrevisionItemCriteria criteriaMAX(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM,
                                        PCx_Audit::ORED::OUVERTS);

  prevItemNode1.insertCriteriaToAdd(criteriaMIN);
  prevItemNode1.insertCriteriaToAdd(criteriaMAX);

  QCOMPARE(prevItemLeaf1.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevRoot.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemLeaf1.getSummedPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemLeaf1b.getSummedPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemLeaf1b.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevRoot.getSummedPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemLeaf2.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode2.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemLeaf2.getSummedPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode2.getSummedPrevisionItemValue(), (qint64)0);

  prevRoot.insertCriteriaToAdd(criteriaMIN);
  prevRoot.saveDataToDb();
  prevRoot.dispatchComputedValueToChildrenLeaves();
  prevItemNode1.loadFromDb();
  QVERIFY(prevItemNode1.getItemsToAdd().isEmpty());
  QVERIFY(prevItemNode1.getItemsToSubstract().isEmpty());
  prevItemLeaf1.loadFromDb();
  prevItemLeaf1b.loadFromDb();
  prevItemLeaf2.loadFromDb();
  QCOMPARE(prevItemLeaf1.getItemsToAdd().count(), 1);
  QCOMPARE(prevItemLeaf1b.getItemsToAdd().count(), 1);
  QCOMPARE(prevItemLeaf2.getItemsToAdd().count(), 1);
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)483512017);

  prevItemNode1.insertCriteriaToAdd(criteriaMIN);
  prevItemNode1.insertCriteriaToAdd(criteriaMAX);
  prevItemNode1.saveDataToDb();
  prevItemNode1.dispatchComputedValueToChildrenLeaves();
  prevItemLeaf1.loadFromDb();
  QCOMPARE(prevItemLeaf1.getItemsToAdd().count(), 1);
  QCOMPARE(prevItemLeaf1.getItemsToSubstract().count(), 0);
  PCx_PrevisionItemCriteria criteria = prevItemLeaf1.getItemsToAdd().at(0);
  QCOMPARE(criteria.getOperator(), PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION);
  QCOMPARE(prevItemLeaf1.getPrevisionItemValue(), (qint64)156761134);
  QCOMPARE(prevItemLeaf1.getSummedPrevisionItemValue(), (qint64)156761134);
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)578754000);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)578754000);
  prevRoot.loadFromDb();
  QVERIFY(prevRoot.getItemsToAdd().isEmpty());
  QVERIFY(prevRoot.getItemsToSubstract().isEmpty());

  QVERIFY(!prevItemNode1.getItemsToAdd().isEmpty());
  QVERIFY(prevItemNode1.getItemsToSubstract().isEmpty());
  prevItemNode1.dispatchCriteriaItemsToChildrenLeaves();
  QVERIFY(prevItemNode1.getItemsToAdd().isEmpty());
  QVERIFY(prevItemNode1.getItemsToSubstract().isEmpty());
  prevItemLeaf1.loadFromDb();
  prevItemLeaf1b.loadFromDb();
  QCOMPARE(prevItemLeaf1.getItemsToAdd().count(), 2);
  criteria = prevItemLeaf1.getItemsToAdd().at(0);
  QCOMPARE(criteria.getOperator(), PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM);
  QCOMPARE(prevItemLeaf1b.getItemsToAdd().count(), 2);
  criteria = prevItemLeaf1b.getItemsToAdd().at(1);
  QCOMPARE(criteria.getOperator(), PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM);
  QCOMPARE(prevItemLeaf1.getItemsToSubstract().count(), 0);
  QCOMPARE(prevItemLeaf1.getPrevisionItemValue(), prevItemLeaf1.getSummedPrevisionItemValue());
  prevRoot.loadFromDb();
  QVERIFY(prevRoot.getItemsToAdd().isEmpty());
  QVERIFY(prevRoot.getItemsToSubstract().isEmpty());
  QCOMPARE(prevRoot.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevRoot.getSummedPrevisionItemValue(), (qint64)831656183);
  prevItemLeaf2.loadFromDb();
  QCOMPARE(prevItemLeaf2.getItemsToAdd().count(), 1);
  PCx_Prevision::deletePrevision(prevId);
  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForPCBPrevisionItem() {
  unsigned int treeId = PCx_Tree::addTree("TESTTREE");
  PCx_Tree tree(treeId);
  /*
   * ROOT
   * |_NODE1
   * |  |__LEAF1
   * |  |__LEAF1B
   * |_NODE2
   * |  |__LEAF2
   */
  unsigned int node1 = tree.addNode(1, 1, "NODE1");
  unsigned int leaf1 = tree.addNode(node1, 2, "LEAF1");
  unsigned int leaf1b = tree.addNode(node1, 2, "LEAF1B");
  unsigned int node2 = tree.addNode(1, 1, "NODE2");
  unsigned int leaf2 = tree.addNode(node2, 2, "LEAF2");
  tree.finishTree();
  unsigned int auditId = PCx_Audit::addNewAudit("AUDITTEST", QList<int>{2010, 2011, 2012, 2013}, treeId);

  PCx_Audit audit(auditId);
  fillTestAudit(&audit);
  audit.finishAudit();
  unsigned int prevId = PCx_Prevision::addNewPrevision(auditId, "PREVTEST");
  PCx_Prevision prevision(prevId);

  PCx_PrevisionItem prevItemNode1(&prevision, MODES::DFRFDIRI::DF, node1, 2014);
  PCx_PrevisionItem prevItemLeaf1(&prevision, MODES::DFRFDIRI::DF, leaf1, 2014);
  PCx_PrevisionItem prevItemLeaf1b(&prevision, MODES::DFRFDIRI::DF, leaf1b, 2014);
  PCx_PrevisionItem prevItemNode2(&prevision, MODES::DFRFDIRI::DF, node2, 2014);
  PCx_PrevisionItem prevItemLeaf2(&prevision, MODES::DFRFDIRI::DF, leaf2, 2014);
  PCx_PrevisionItem prevRoot(&prevision, MODES::DFRFDIRI::DF, 1, 2014);

  QCOMPARE(prevItemNode1.getMode(), MODES::DFRFDIRI::DF);
  QCOMPARE(prevItemNode1.getNodeId(), node1);
  QCOMPARE(prevItemNode1.getYear(), 2014);

  QVERIFY(prevItemNode1.getItemsToAdd().isEmpty());
  QVERIFY(prevItemNode1.getItemsToSubstract().isEmpty());
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)0);
  PCx_PrevisionItemCriteria criteriaMIN(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MINIMUM,
                                        PCx_Audit::ORED::OUVERTS);
  PCx_PrevisionItemCriteria criteriaMAX(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::MAXIMUM,
                                        PCx_Audit::ORED::OUVERTS);
  prevItemNode1.insertCriteriaToAdd(criteriaMIN);
  prevItemNode1.insertCriteriaToAdd(criteriaMAX);
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)578754000);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)0);
  prevItemNode1.insertCriteriaToSub(criteriaMIN);
  prevItemNode1.insertCriteriaToSub(criteriaMAX);
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)0);
  prevItemNode1.deleteAllCriteria();
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)0);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)0);
  prevItemNode1.insertCriteriaToAdd(criteriaMIN);
  prevItemNode1.insertCriteriaToAdd(criteriaMAX);
  prevItemNode1.saveDataToDb();
  prevItemNode1.deleteAllCriteria();
  prevItemNode1.loadFromDb();
  QCOMPARE(prevItemNode1.getPrevisionItemValue(), (qint64)578754000);
  QCOMPARE(prevItemNode1.getSummedPrevisionItemValue(), (qint64)0);

  PCx_Prevision::deletePrevision(prevId);
  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}
