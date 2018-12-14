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
#include "pcx_audit.h"
#include "pcx_prevision.h"
#include "pcx_tree.h"
#include "tst_protonclassicsuiteunittests.h"
#include "utils.h"
#include <QDataStream>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <iostream>

ProtonClassicSuiteUnitTests::ProtonClassicSuiteUnitTests() {}

void ProtonClassicSuiteUnitTests::cleanup() {
  QList<QPair<unsigned int, QString>> listOfPrev = PCx_Prevision::getListOfPrevisions();
  QPair<unsigned int, QString> prevision;
  foreach (prevision, listOfPrev) { PCx_Prevision::deletePrevision(prevision.first); }

  QList<QPair<unsigned int, QString>> listOfAudits = PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::AllAudits);
  QPair<unsigned int, QString> audit;
  foreach (audit, listOfAudits) { PCx_Audit::deleteAudit(audit.first); }

  QList<unsigned int> listOfTrees = PCx_Tree::getListOfTreesId();
  foreach (unsigned int tree, listOfTrees) { PCx_Tree::deleteTree(tree); }
}

QByteArray ProtonClassicSuiteUnitTests::hashString(const QString &str) {
  QByteArray hash = QCryptographicHash::hash(QByteArray::fromRawData((const char *)str.utf16(), str.length() * 2),
                                             QCryptographicHash::Sha256);

  /* QDataStream stream(hash);
     quint64 a, b;
     stream >> a >> b;
     return a ^ b;*/
  return hash;
}

void ProtonClassicSuiteUnitTests::initTestCase() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(":memory:");
  db.open();
  QSqlQuery("PRAGMA foreign_keys=ON");
  initializeNewDb();
}

void ProtonClassicSuiteUnitTests::cleanupTestCase() { QSqlDatabase::database().close(); }

QTEST_MAIN(ProtonClassicSuiteUnitTests)

//#include "tst_protonclassicsuiteunittests_main.moc"
