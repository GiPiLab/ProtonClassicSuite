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
#include "pcx_graphics.h"
#include "pcx_report.h"
#include "pcx_tables.h"
#include "tst_protonclassicsuiteunittests.h"
#include "utils.h"
#include <QCryptographicHash>
#include <QCustomPlot/qcustomplot.h>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

void ProtonClassicSuiteUnitTests::fillTestAudit(PCx_Audit *audit) {
  int firstYear = audit->getYears().constFirst();
  int lastYear = audit->getYears().constLast();
  QList<double> values = {65678.3, 66154.3, 394754,  68857.1, 218632,  369859,  82991.4, 363930,  129854,  292819,
                          118571,  319187,  38998.5, 243909,  189486,  343779,  22629.2, 351890,  31705.7, 113993,
                          423967,  88494.2, 392966,  341903,  55329.6, 295115,  6070.43, 187548,  320275,  161991,
                          18543.9, 238280,  250449,  322051,  205258,  77598.7, 105687,  411057,  253461,  374204,
                          145066,  283387,  356531,  80491.4, 147056,  308305,  414776,  275012,  75619.3, 177614,
                          182726,  341583,  1785.75, 81796.9, 146423,  278263,  190203,  36165.6, 202805,  45837.1,
                          398973,  97304.4, 282173,  424908,  247909,  363903,  83313.5, 422401,  214320,  42290,
                          409648,  271988,  334211,  370082,  143964,  413732,  417126,  416604,  249822,  46883.6,
                          98482.3, 405190,  35378.7, 276694,  5857.75, 325860,  237603,  191510,  138822,  196374,
                          13648.5, 83803,   171284,  207096,  82751.2, 12422.7, 234194,  77392.1, 394402,  86692.1};

  int count = 0;
  QList<unsigned int> leaves = audit->getAttachedTree()->getLeavesId();
  for (unsigned int mode = static_cast<unsigned int>(MODES::DFRFDIRI::DF); mode < sizeof(MODES::DFRFDIRI); mode++) {
    for (int year = firstYear; year <= lastYear; year++) {
      foreach (unsigned int leaf, leaves) {

        audit->setLeafValues(leaf, (MODES::DFRFDIRI)mode, year,
                             QMap<PCx_Audit::ORED, double>{{PCx_Audit::ORED::OUVERTS, values.at(count)},
                                                           {PCx_Audit::ORED::REALISES, values.at(count + 1)},
                                                           {PCx_Audit::ORED::ENGAGES, values.at(count + 2)}});

        if (count == values.count() - 4)
          count = 0;
        else
          count++;
      }
    }
  }
}

void ProtonClassicSuiteUnitTests::testCaseForPCATables() {
  unsigned int treeId = PCx_Tree::addTree("TREEFORTEST");
  PCx_Tree tree(treeId);
  unsigned int nodeA = tree.addNode(1, 1, "nodeA");
  unsigned int nodeB = tree.addNode(nodeA, 1, "nodeB");
  tree.addNode(nodeA, 1, "nodeC");
  tree.addNode(1, 1, "nodeD");
  tree.addNode(nodeB, 1, "nodeE");
  tree.finishTree();

  unsigned int auditId = PCx_Audit::addNewAudit("AUDITFORTEST", QList<int>{2010, 2011, 2012}, treeId);

  PCx_Audit audit(auditId);

  fillTestAudit(&audit);

  PCx_Tables tables(&audit);

  auto expectedT1 = QStringLiteral(
      "\n<table align='center' width='98%' class='t1' cellpadding='5'><tr class='t1entete'><td align='center' "
      "colspan=8><b>1. Racine (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr "
      "class='t1entete'><th>Exercice</th><th>Crédits ouverts</th><th>R&eacute;alis&eacute;</th><th>%/crédits "
      "ouverts</th><th>Engag&eacute;</th><th>%/crédits ouverts</th><th>Disponible</th><th>%/crédits "
      "ouverts</th></tr><tr><td class='t1annee'>2010</td><td align='right' class='t1valeur'>526 586,60</td><td "
      "align='right' class='t1valeur'>529 765,40</td><td align='right' class='t1pourcent'>100,60%</td><td "
      "align='right' class='t1valeur'>682 243,10</td><td align='right' class='t1pourcent'>129,56%</td><td "
      "align='right' class='t1valeur'>-685 421,90</td><td align='right' "
      "class='t1pourcent'>-130,16%</td></tr><tr><td class='t1annee'>2011</td><td align='right' "
      "class='t1valeur'>657 348,10</td><td align='right' class='t1valeur'>671 482,40</td><td align='right' "
      "class='t1pourcent'>102,15%</td><td align='right' class='t1valeur'>816 780,40</td><td align='right' "
      "class='t1pourcent'>124,25%</td><td align='right' class='t1valeur'>-830 914,70</td><td align='right' "
      "class='t1pourcent'>-126,40%</td></tr><tr><td class='t1annee'>2012</td><td align='right' "
      "class='t1valeur'>576 775,40</td><td align='right' class='t1valeur'>786 603,00</td><td align='right' "
      "class='t1pourcent'>136,38%</td><td align='right' class='t1valeur'>541 244,00</td><td align='right' "
      "class='t1pourcent'>93,84%</td><td align='right' class='t1valeur'>-751 071,60</td><td align='right' "
      "class='t1pourcent'>-130,22%</td></tr></table>");

  QCOMPARE(tables.getPCAT1(1, MODES::DFRFDIRI::DF), expectedT1);

  auto expectedT2 =
      QStringLiteral("\n<table class='t2' width='70%' align='center' cellpadding='5'><tr class='t2entete'><td "
                     "colspan=3 align='center'><b>&Eacute;volution cumul&eacute;e du compte administratif de [ 1. "
                     "Racine ] <u>hors celui de [ 1. Racine ]</u> (<span style='color:#7c0000'>Dépenses de "
                     "fonctionnement</span>)</b></td></tr><tr class='t2entete'><th>Exercice</th><th>Pour les crédits "
                     "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr><tr><td class='t2annee'>2010</td><td "
                     "class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr><tr><td "
                     "class='t2annee'>2011</td><td align='right' class='t2pourcent'>0,00%</td><td align='right' "
                     "class='t2pourcent'>0,00%</td></tr><tr><td class='t2annee'>2012</td><td align='right' "
                     "class='t2pourcent'>0,00%</td><td align='right' class='t2pourcent'>0,00%</td></tr></table>");

  QCOMPARE(tables.getPCAT2(1, MODES::DFRFDIRI::DF), expectedT2);

  auto expectedT2bis =
      QStringLiteral("\n<table class='t2bis' width='70%' align='center' cellpadding='5'><tr class='t3entete'><td "
                     "colspan=3 align='center'><b>&Eacute;volution du compte administratif de [ 1. Racine ] <u>hors "
                     "celui de<br>[ 1. Racine ]</u> (<span style='color:#7c0000'>Dépenses de "
                     "fonctionnement</span>)</b></td></tr><tr class='t3entete'><th>Exercice</th><th>Pour les crédits "
                     "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr><tr><td class='t3annee'>2010</td><td "
                     "class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr><tr><td "
                     "class='t3annee'>2011</td><td align='right' class='t3pourcent'>0,00%</td><td align='right' "
                     "class='t3pourcent'>0,00%</td></tr><tr><td class='t3annee'>2012</td><td align='right' "
                     "class='t3pourcent'>0,00%</td><td align='right' class='t3pourcent'>0,00%</td></tr></table>");

  QCOMPARE(tables.getPCAT2bis(1, MODES::DFRFDIRI::DF), expectedT2bis);

  auto expectedT3 =
      QStringLiteral("\n<table class='t3' width='70%' align='center' cellpadding='5'><tr class='t2entete'><td "
                     "colspan=3 align='center'><b>&Eacute;volution cumul&eacute;e du compte administratif de<br>[ 1. "
                     "Racine ] (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr "
                     "class='t2entete'><th>Exercice</th><th>Pour les crédits ouverts</th><th>Pour le "
                     "r&eacute;alis&eacute;</th></tr><tr><td class='t2annee'>2010</td><td "
                     "class='t2pourcent'>&nbsp;</td><td class='t2pourcent'>&nbsp;</td></tr><tr><td "
                     "class='t2annee'>2011</td><td align='right' class='t2pourcent'>24,83%</td><td align='right' "
                     "class='t2pourcent'>26,75%</td></tr><tr><td class='t2annee'>2012</td><td align='right' "
                     "class='t2pourcent'>9,53%</td><td align='right' class='t2pourcent'>48,48%</td></tr></table>");

  QCOMPARE(tables.getPCAT3(1, MODES::DFRFDIRI::DF), expectedT3);

  auto expectedT3bis =
      QStringLiteral("\n<table class='t3bis' width='70%' align='center' cellpadding='5'><tr class='t3entete'><td "
                     "colspan=3 align='center'><b>&Eacute;volution du compte administratif de<br>[ 1. Racine ] (<span "
                     "style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr "
                     "class='t3entete'><th>Exercice</th><th>Pour les crédits ouverts</th><th>Pour le "
                     "r&eacute;alis&eacute;</th></tr><tr><td class='t3annee'>2010</td><td "
                     "class='t3pourcent'>&nbsp;</td><td class='t3pourcent'>&nbsp;</td></tr><tr><td "
                     "class='t3annee'>2011</td><td align='right' class='t3pourcent'>24,83%</td><td align='right' "
                     "class='t3pourcent'>26,75%</td></tr><tr><td class='t3annee'>2012</td><td align='right' "
                     "class='t3pourcent'>-12,26%</td><td align='right' class='t3pourcent'>17,14%</td></tr></table>");

  QCOMPARE(tables.getPCAT3bis(1, MODES::DFRFDIRI::DF), expectedT3bis);

  auto expectedT4 =
      QStringLiteral("\n<table class='t4' align='center' width='65%' cellpadding='5'><tr class='t4entete'><td "
                     "colspan=3 align='center'><b>Poids relatif de [ 1. Racine ]<br>par rapport à [ 1. Racine ] (<span "
                     "style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr "
                     "class='t4entete'><th>Exercice</th><th>Pour les crédits ouverts</th><th>Pour le "
                     "r&eacute;alis&eacute;</th></tr><tr><td class='t4annee'>2010</td><td align='right' "
                     "class='t4pourcent'>100,00%</td><td align='right' class='t4pourcent'>100,00%</td></tr><tr><td "
                     "class='t4annee'>2011</td><td align='right' class='t4pourcent'>100,00%</td><td align='right' "
                     "class='t4pourcent'>100,00%</td></tr><tr><td class='t4annee'>2012</td><td align='right' "
                     "class='t4pourcent'>100,00%</td><td align='right' class='t4pourcent'>100,00%</td></tr></table>");

  QCOMPARE(tables.getPCAT4(1, MODES::DFRFDIRI::DF), expectedT4);

  auto expectedT5 = QStringLiteral(
      "\n<table class='t5' width='80%' align='center' cellpadding='5'><tr class='t5entete'><td colspan=6 "
      "align='center'><b>Analyse en base 100 du compte administratif de [ 1. Racine ] <u>hors celui de<br>[ 1. Racine "
      "]</u> (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr class='t5entete'><th "
      "valign='middle' rowspan=2>Exercice</th><th valign='middle' rowspan=2>Crédits ouverts</th><th valign='middle' "
      "rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr><tr "
      "class='t5entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th "
      "style='font-weight:normal'>dont<br>disponible</th></tr><tr><td class='t5annee'>2010</td><td align='right' "
      "class='t5pourcent'>100</td><td align='right' class='t5pourcent'>100</td><td align='right' "
      "class='t5pourcent'>100</td><td align='right' class='t5valeur'>100</td><td align='right' "
      "class='t5valeur'>100</td></tr><tr><td class='t5annee'>2011</td><td align='right' class='t5pourcent'>0</td><td "
      "align='right' class='t5pourcent'>0</td><td align='right' class='t5pourcent'>0</td><td align='right' "
      "class='t5valeur'>0</td><td align='right' class='t5valeur'>0</td></tr><tr><td class='t5annee'>2012</td><td "
      "align='right' class='t5pourcent'>0</td><td align='right' class='t5pourcent'>0</td><td align='right' "
      "class='t5pourcent'>0</td><td align='right' class='t5valeur'>0</td><td align='right' "
      "class='t5valeur'>0</td></tr></table>");

  QCOMPARE(tables.getPCAT5(1, MODES::DFRFDIRI::DF), expectedT5);

  auto expectedT6 = QStringLiteral(
      "\n<table class='t6' width='80%' align='center' cellpadding='5'><tr class='t6entete'><td colspan=6 "
      "align='center'><b>Analyse en base 100 du compte administratif de [ 1. Racine ]<br>(<span "
      "style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr class='t6entete'><th valign='middle' "
      "rowspan=2>Exercice</th><th valign='middle' rowspan=2>Crédits ouverts</th><th valign='middle' "
      "rowspan=2>R&eacute;alis&eacute;</th><th colspan=3 align='center'>Non consomm&eacute;</th></tr><tr "
      "class='t6entete'><th valign='middle'>Total</th><th style='font-weight:normal'>dont<br>engag&eacute;</th><th "
      "style='font-weight:normal'>dont<br>disponible</th></tr><tr><td class='t6annee'>2010</td><td align='right' "
      "class='t6pourcent'>100</td><td align='right' class='t6pourcent'>100</td><td align='right' "
      "class='t6pourcent'>100</td><td align='right' class='t6valeur'>100</td><td align='right' "
      "class='t6valeur'>100</td></tr><tr><td class='t6annee'>2011</td><td align='right' class='t6pourcent'>125</td><td "
      "align='right' class='t6pourcent'>127</td><td align='right' class='t6pourcent'>445</td><td align='right' "
      "class='t6valeur'>120</td><td align='right' class='t6valeur'>121</td></tr><tr><td class='t6annee'>2012</td><td "
      "align='right' class='t6pourcent'>110</td><td align='right' class='t6pourcent'>148</td><td align='right' "
      "class='t6pourcent'>6 601</td><td align='right' class='t6valeur'>79</td><td align='right' "
      "class='t6valeur'>110</td></tr></table>");

  QCOMPARE(tables.getPCAT6(1, MODES::DFRFDIRI::DF), expectedT6);

  auto expectedT7 =
      QStringLiteral("\n<table class='t7' width='80%' align='center' cellpadding='5'><tr class='t7entete'><td "
                     "colspan=5 align='center'><b>Transcription en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; "
                     "de<br>[ 1. Racine ] (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)<br><u>sur la "
                     "base d'une &eacute;quivalence de 365/365<sup>&egrave;me</sup> entre le r&eacute;alis&eacute; "
                     "budg&eacute;taire et l'ann&eacute;e</u></b><tr class='t7entete'><th>Exercice</th><th>Crédits "
                     "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th><th>Disponible</th></tr><tr><td "
                     "class='t7annee'>2010</td><td align='right' class='t7pourcent'>363</td><td align='right' "
                     "class='t7pourcent'>365</td><td align='right' class='t7pourcent'>470</td><td align='right' "
                     "class='t7pourcent'>-472</td></tr><tr><td class='t7annee'>2011</td><td align='right' "
                     "class='t7pourcent'>357</td><td align='right' class='t7pourcent'>365</td><td align='right' "
                     "class='t7pourcent'>444</td><td align='right' class='t7pourcent'>-452</td></tr><tr><td "
                     "class='t7annee'>2012</td><td align='right' class='t7pourcent'>268</td><td align='right' "
                     "class='t7pourcent'>365</td><td align='right' class='t7pourcent'>251</td><td align='right' "
                     "class='t7pourcent'>-349</td></tr></table>");

  QCOMPARE(tables.getPCAT7(1, MODES::DFRFDIRI::DF), expectedT7);

  auto expectedT8 = QStringLiteral(
      "\n<table class='t8' width='50%' align='center' cellpadding='5'><tr class='t8entete'><td colspan=2 "
      "align='center'><b>Moyennes budg&eacute;taires de<br>[ 1. Racine ] (<span style='color:#7c0000'>Dépenses de "
      "fonctionnement</span>)<br>constat&eacute;es pour la p&eacute;riode audit&eacute;e</b></td></tr><tr><td "
      "class='t8annee' align='center'>R&eacute;alis&eacute;</td><td align='right' "
      "class='t8pourcent'>112,90%</td></tr><tr><td class='t8annee' align='center' style='font-weight:normal'>Non "
      "utilis&eacute;</td><td align='right' class='t8valeur'>-12,90%</td></tr><tr><td class='t8annee' "
      "align='center'><i>dont engag&eacute;</i></td><td align='right' "
      "class='t8pourcent'><i>115,88%</i></td></tr><tr><td class='t8annee' align='center'><i>dont "
      "disponible</i></td><td align='right' class='t8pourcent'><i>-128,78%</i></td></tr></table>");

  QCOMPARE(tables.getPCAT8(1, MODES::DFRFDIRI::DF), expectedT8);

  auto expectedT9 = QStringLiteral(
      "\n<table class='t9' width='60%' align='center' cellpadding='5'><tr class='t9entete'><td colspan=2 "
      "align='center'><b>&Eacute;quivalences moyennes en &laquo;&nbsp;jours activit&eacute;&nbsp;&raquo; de<br>[ 1. "
      "Racine ] (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)<br>constat&eacute;es pour la "
      "p&eacute;riode audit&eacute;e</b></td></tr><tr><td colspan='2' align='center' class='t9annee'>Face aux "
      "cr&eacute;dits r&eacute;alis&eacute;s, les pr&eacute;visions correspondent &agrave; :<br><b>323 jours de "
      "travail par an</b></td></tr><tr><td class='t9annee' style='font-weight:normal'>D&eacute;passement de "
      "capacit&eacute; des pr&eacute;visions</td><td align='right' class='t9valeur' valign='middle'>-42 "
      "jours</td></tr><tr><td class='t9annee'><i>dont engag&eacute;</i></td><td class='t9pourcent' align='right' "
      "valign='middle'><i>375 jours</i></td></tr><tr><td class='t9annee'><i>dont disponible</i></td><td "
      "class='t9pourcent' align='right' valign='middle'><i>-416 jours</i></td></tr></table>");

  QCOMPARE(tables.getPCAT9(1, MODES::DFRFDIRI::DF), expectedT9);

  auto expectedT10 = QStringLiteral(
      "\n<table class='t10' width='80%' align='center' cellpadding='5'><tr class='t1entete'><td align='center' "
      "colspan=4><b>R&Eacute;SULTATS de FONCTIONNEMENT de [ 1. Racine ]<br>UCC <span "
      "style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
      "</span><span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr><tr "
      "class='t1entete'><th>Exercice</th><th>Crédits "
      "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr><tr><td class='t1annee'>2010</td><td "
      "align='right' class='t1valeur'><span style='color:#008000'>203 990,40</span></td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-53 008,90</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-80 148,60</span></td></tr><tr><td class='t1annee'>2011</td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-184 954,60</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>105 691,60</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-260 886,20</span></td></tr><tr><td class='t1annee'>2012</td><td align='right' "
      "class='t1valeur'><span style='color:#008000'>141 522,80</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-380 378,10</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-43 655,30</span></td></tr></table>");

  QCOMPARE(tables.getPCAT10(1), expectedT10);

  auto expectedT11 = QStringLiteral(
      "\n<table class='t11' width='80%' align='center' cellpadding='5'><tr class='t1entete'><td align='center' "
      "colspan=4><b>R&Eacute;SULTATS d'INVESTISSEMENT de [ 1. Racine ]<br>UCC <span "
      "style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
      "</span><span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr><tr "
      "class='t1entete'><th>Exercice</th><th>Crédits "
      "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr><tr><td class='t1annee'>2010</td><td "
      "align='right' class='t1valeur'><span style='color:#008000'>100 148,30</span></td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-125 644,30</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-486 612,30</span></td></tr><tr><td class='t1annee'>2011</td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-316 090,30</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>20 581,40</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>85 410,40</span></td></tr><tr><td class='t1annee'>2012</td><td align='right' "
      "class='t1valeur'><span style='color:#008000'>248 392,67</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-100 189,73</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>80 449,27</span></td></tr></table>");

  QCOMPARE(tables.getPCAT11(1), expectedT11);

  auto expectedT12 = QStringLiteral(
      "\n<table class='t12' width='80%' align='center' cellpadding='5'><tr class='t1entete'><td align='center' "
      "colspan=4><b>R&Eacute;SULTATS BUDG&Eacute;TAIRES de [ 1. Racine ]<br>UCC <span "
      "style='color:black'>&eacute;quilibr&eacute;e (0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
      "</span><span style='color:#7c0000'>d&eacute;ficitaire (-)</span></b></td></tr><tr "
      "class='t1entete'><th>Exercice</th><th>Crédits "
      "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th></tr><tr><td class='t1annee'>2010</td><td "
      "align='right' class='t1valeur'><span style='color:#008000'>304 138,70</span></td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-178 653,20</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-566 760,90</span></td></tr><tr><td class='t1annee'>2011</td><td align='right' "
      "class='t1valeur'><span style='color:#7c0000'>-501 044,90</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>126 273,00</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-175 475,80</span></td></tr><tr><td class='t1annee'>2012</td><td align='right' "
      "class='t1valeur'><span style='color:#008000'>389 915,47</span></td><td align='right' class='t1valeur'><span "
      "style='color:#7c0000'>-480 567,83</span></td><td align='right' class='t1valeur'><span "
      "style='color:#008000'>36 793,97</span></td></tr></table>");

  QCOMPARE(tables.getPCAT12(1), expectedT12);

  auto expectedTRaw = QStringLiteral(
      "\n<table class='tPCARAW' align='center' cellpadding='5'><tr class='t1entete'><td align='center' colspan=5><b>1. "
      "Racine (<span style='color:#7c0000'>Dépenses de fonctionnement</span>)</b></td></tr><tr "
      "class='t1entete'><th>Exercice</th><th>Crédits "
      "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</th><th>Disponible</th></tr><tr><td "
      "class='t1annee'>2010</td><td align='right' class='t1valeur'>526 586,60</td><td align='right' "
      "class='t1valeur'>529 765,40</td><td align='right' class='t1valeur'>682 243,10</td><td align='right' "
      "class='t1valeur'>-685 421,90</td></tr><tr><td class='t1annee'>2011</td><td align='right' "
      "class='t1valeur'>657 348,10</td><td align='right' class='t1valeur'>671 482,40</td><td align='right' "
      "class='t1valeur'>816 780,40</td><td align='right' class='t1valeur'>-830 914,70</td></tr><tr><td "
      "class='t1annee'>2012</td><td align='right' class='t1valeur'>576 775,40</td><td align='right' "
      "class='t1valeur'>786 603,00</td><td align='right' class='t1valeur'>541 244,00</td><td align='right' "
      "class='t1valeur'>-751 071,60</td></tr></table>");

  QCOMPARE(tables.getPCARawData(1, MODES::DFRFDIRI::DF), expectedTRaw);

  /*  qDebug() << tables.getPCAT1(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT2(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT3(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT3bis(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT4(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT5(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT6(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT7(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT8(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT9(1, MODES::DFRFDIRI::DF);
    qDebug() << tables.getPCAT10(1);
    qDebug() << tables.getPCAT11(1);
    qDebug() << tables.getPCAT12(1);
    qDebug() << tables.getPCARawData(1, MODES::DFRFDIRI::DF);*/

  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::benchmarkReport() {
  unsigned int treeId = PCx_Tree::createRandomTree("RANDOM", 50);
  PCx_Tree tree(treeId);
  tree.finishTree();
  QTextDocument doc;

  unsigned int auditId = PCx_Audit::addNewAudit("RANDOMAUDIT", QList<int>{2000, 2001, 2002, 2003, 2004, 2005}, treeId);
  PCx_Audit audit(auditId);

  audit.fillWithRandomData(MODES::DFRFDIRI::DF, false);
  audit.fillWithRandomData(MODES::DFRFDIRI::RF, false);
  audit.fillWithRandomData(MODES::DFRFDIRI::DI, false);
  audit.fillWithRandomData(MODES::DFRFDIRI::RI, false);
  audit.finishAudit();

  PCx_Report report(&audit);

  QBENCHMARK(report.generateHTMLAuditReportForNode(
      QList<PCx_Tables::PCAPRESETS>{},
      QList<PCx_Tables::PCATABLES>{
          PCx_Tables::PCATABLES::PCAT1, PCx_Tables::PCATABLES::PCAT2, PCx_Tables::PCATABLES::PCAT3,
          PCx_Tables::PCATABLES::PCAT4, PCx_Tables::PCATABLES::PCAT5, PCx_Tables::PCATABLES::PCAT6,
          PCx_Tables::PCATABLES::PCAT7, PCx_Tables::PCATABLES::PCAT8, PCx_Tables::PCATABLES::PCAT9,
          PCx_Tables::PCATABLES::PCAT10, PCx_Tables::PCATABLES::PCAT11, PCx_Tables::PCATABLES::PCAT12},
      QList<PCx_Graphics::PCAGRAPHICS>{PCx_Graphics::PCAGRAPHICS::PCAG1, PCx_Graphics::PCAGRAPHICS::PCAG2,
                                       PCx_Graphics::PCAGRAPHICS::PCAG3, PCx_Graphics::PCAGRAPHICS::PCAG4,
                                       PCx_Graphics::PCAGRAPHICS::PCAG5, PCx_Graphics::PCAGRAPHICS::PCAG6,
                                       PCx_Graphics::PCAGRAPHICS::PCAG7, PCx_Graphics::PCAGRAPHICS::PCAG8,
                                       PCx_Graphics::PCAGRAPHICS::PCAG9, PCx_Graphics::PCAGRAPHICS::PCAHISTORY},
      4, MODES::DFRFDIRI::DF, 1, &doc, nullptr, nullptr, nullptr, nullptr));

  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForPCAGraphics() {
  unsigned int treeId = PCx_Tree::addTree("TREEFORTEST");
  PCx_Tree tree(treeId);
  unsigned int nodeA = tree.addNode(1, 1, "nodeA");
  unsigned int nodeB = tree.addNode(nodeA, 1, "nodeB");
  tree.addNode(nodeA, 1, "nodeC");
  tree.addNode(1, 1, "nodeD");
  tree.addNode(nodeB, 1, "nodeE");
  tree.finishTree();

  unsigned int auditId = PCx_Audit::addNewAudit("AUDITFORTEST", QList<int>{2010, 2011, 2012, 2013}, treeId);

  PCx_Audit audit(auditId);

  fillTestAudit(&audit);

  QCustomPlot plot;
  QByteArray ba, hash;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  PCx_Graphics graphics(&audit, &plot);

  QByteArray expectedG1 = "0f8a89463874c3cf8a708b6b10f4e3a1852ce193fbde2fa497da4242cd0290a0";
  QByteArray expectedG2 = "1ff3b470c0c3d387da9405351dc889c3163bb09055dd8b7588f1ececcba5ae36";
  QByteArray expectedG3 = "fd020155c9c9c0e6c5cd0fa499c3ff02a07facfd9785f1d0d49a6ba2835aed0f";
  QByteArray expectedG4 = "87c3596abb89d3ce80610dc922981826fcaabac27d3c93cc24610926d550605c";
  QByteArray expectedG5 = "ce343ee0ccc5ad718190e113f7b3838e2ecea1e1735259d51e86079fe6a3e570";
  QByteArray expectedG6 = "533522b3a26aa324e7abe138a2bf32e02dec45fc13262626ac67fa1b0072f9e2";
  QByteArray expectedG7 = "0a11d97f065ef41bb5daaad8f5985e43fd1a3b90f804a540871393c04320cd07";
  QByteArray expectedG8 = "18c88ada9516d7794ce72f61120f476a1db25e6bded7b84b0aa6b6604e7a81d0";
  QByteArray expectedG9 = "9869af5f3566524f779241b726a1d596080c1ff81bcf8531158c334a56f4a8ba";
  QByteArray expectedGHist = "d30064d2490e767f93a76832b597319f7c9320c27f014c6237e00e4400d37e54";

  graphics.getPCAG1(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);

  qDebug() << "G1" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG1);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG2(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G2" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG2);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG3(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G3" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG3);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG4(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G4" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG4);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG5(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G5" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG5);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG6(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G6" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG6);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG7(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G7" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG7);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG8(nodeA, MODES::DFRFDIRI::DF);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G8" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG8);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAG9(nodeA);
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "G9" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedG9);

  ba.clear();
  buffer.open(QIODevice::WriteOnly);
  graphics.getPCAHistory(nodeA, MODES::DFRFDIRI::DF,
                         QList<PCx_Audit::ORED>{PCx_Audit::ORED::OUVERTS, PCx_Audit::ORED::REALISES,
                                                PCx_Audit::ORED::ENGAGES, PCx_Audit::ORED::DISPONIBLES});
  plot.toPixmap(PCx_Graphics::DEFAULTWIDTH, PCx_Graphics::DEFAULTHEIGHT).save(&buffer, "PNG");
  buffer.close();
  hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
  qDebug() << "GHist" << hash.toHex();
  QCOMPARE(hash.toHex(), expectedGHist);

  PCx_Audit::deleteAudit(auditId);
  PCx_Tree::deleteTree(treeId);
}
