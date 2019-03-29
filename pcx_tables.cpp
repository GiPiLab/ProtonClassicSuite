/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le
 * Développement Local (2006--)
 *
 * labo@gipilab.org
 *
 * Ce logiciel est un programme informatique servant à cerner l'ensemble des
 * données budgétaires de la collectivité territoriale (audit, reporting
 * infra-annuel, prévision des dépenses à venir)
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

#include "pcx_tables.h"
#include "pcx_reportingtableoverviewmodel.h"
#include "pcx_treemodel.h"
#include "utils.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableView>

using namespace NUMBERSFORMAT;

PCx_Tables::PCx_Tables(PCx_Audit *model) : auditModel(model) { reportingModel = nullptr; }

PCx_Tables::PCx_Tables(PCx_Reporting *reportingModel) : reportingModel(reportingModel) { auditModel = nullptr; }

QString PCx_Tables::getPCAT1(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table align='center' width='98%' class='t1' cellpadding='5'>"
              "<tr class='t1entete'><td align='center' colspan=8><b>%1 (<span "
              "style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t1entete'><th>Exercice</th><th>Crédits ouverts</th>"
              "<th>R&eacute;alis&eacute;</th><th>%/crédits ouverts</th>"
              "<th>Engag&eacute;</th><th>%/crédits "
              "ouverts</th><th>Disponible</th><th>%/crédits ouverts</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();

    double percentRealisesOuverts = 0.0, percentEngagesOuverts = 0.0, percentDisponiblesOuverts = 0.0;

    if (ouverts != 0) {
      percentRealisesOuverts = realises * 100.0 / ouverts;
      percentEngagesOuverts = engages * 100.0 / ouverts;
      percentDisponiblesOuverts = disponibles * 100.0 / ouverts;
    }
    output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>%2</td><td align='right' class='t1valeur'>%3</td>"
                          "<td align='right' class='t1pourcent'>%4%</td><td align='right' "
                          "class='t1valeur'>%5</td><td align='right' "
                          "class='t1pourcent'>%6%</td>"
                          "<td align='right' class='t1valeur'>%7</td><td align='right' "
                          "class='t1pourcent'>%8%</td></tr>")
                      .arg(q.value("annee").toUInt())
                      .arg(formatFixedPoint(ouverts), formatFixedPoint(realises),
                           formatDouble(percentRealisesOuverts, -1, true), formatFixedPoint(engages),
                           formatDouble(percentEngagesOuverts, -1, true), formatFixedPoint(disponibles),
                           formatDouble(percentDisponiblesOuverts, -1, true)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT2(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  if (node == 0 || auditModel == nullptr || referenceNode == 0) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());
  QString output =
      QString("\n<table class='t2' width='70%' align='center' cellpadding='5'>"
              "<tr class='t2entete'><td colspan=3 align='center'>"
              "<b>&Eacute;volution cumul&eacute;e du compte administratif de [ "
              "%3 ] "
              "<u>hors celui de [ %1 ]</u> (<span "
              "style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t2entete'><th>Exercice</th><th>Pour les crédits "
              "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode),
               auditModel->getAttachedTree()->getNodeName(referenceNode).toHtmlEscaped());

  QMap<unsigned int, qint64> ouvertsRoot, realisesRoot;
  qint64 firstYearOuvertsRoot = 0, firstYearRealisesRoot = 0;

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:refnode order by annee").arg(tableName));
  q.bindValue(":refnode", referenceNode);
  q.exec();
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForRoot = false;
  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    ouvertsRoot.insert(q.value(QStringLiteral("annee")).toUInt(), ouverts);
    realisesRoot.insert(q.value(QStringLiteral("annee")).toUInt(), realises);
    if (!doneFirstForRoot) {
      firstYearOuvertsRoot = ouverts;
      firstYearRealisesRoot = realises;
      doneFirstForRoot = true;
    }
  }

  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0;
  qint64 diffFirstYearRootNodeOuverts = 0, diffFirstYearRootNodeRealises = 0;

  while (q.next()) {
    qint64 diffRootNodeOuverts = 0, diffRootNodeRealises = 0;
    qint64 diffCurrentYearFirstYearOuverts = 0, diffCurrentYearFirstYearRealises = 0;

    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();

    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      diffFirstYearRootNodeOuverts = firstYearOuvertsRoot - firstYearOuvertsNode;
      diffFirstYearRootNodeRealises = firstYearRealisesRoot - firstYearRealisesNode;
      output.append(QString("<tr><td class='t2annee'>%1</td><td "
                            "class='t2pourcent'>&nbsp;</td><td "
                            "class='t2pourcent'>&nbsp;</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      double percentOuverts = 0.0, percentRealises = 0.0;
      diffRootNodeOuverts = ouvertsRoot[annee] - ouverts;
      diffRootNodeRealises = realisesRoot[annee] - realises;
      diffCurrentYearFirstYearOuverts = diffRootNodeOuverts - diffFirstYearRootNodeOuverts;
      diffCurrentYearFirstYearRealises = diffRootNodeRealises - diffFirstYearRootNodeRealises;
      if (diffFirstYearRootNodeOuverts != 0) {
        percentOuverts = diffCurrentYearFirstYearOuverts * 100.0 / diffFirstYearRootNodeOuverts;
      }
      if (diffFirstYearRootNodeRealises != 0) {
        percentRealises = diffCurrentYearFirstYearRealises * 100.0 / diffFirstYearRootNodeRealises;
      }
      output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' "
                            "class='t2pourcent'>%2%</td><td align='right' "
                            "class='t2pourcent'>%3%</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, -1, true), formatDouble(percentRealises, -1, true)));
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT2bis(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  if (node == 0 || auditModel == nullptr || referenceNode == 0) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  // The classes "t3xxx" are not a mistake
  QString output =
      QString("\n<table class='t2bis' width='70%' align='center' cellpadding='5'>"
              "<tr class='t3entete'><td colspan=3 align='center'>"
              "<b>&Eacute;volution du compte administratif de [ %3 ] "
              "<u>hors celui de<br>[ %1 ]</u> (<span "
              "style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t3entete'><th>Exercice</th><th>Pour les crédits "
              "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode),
               auditModel->getAttachedTree()->getNodeName(referenceNode).toHtmlEscaped());

  QMap<unsigned int, qint64> ouvertsRoot, realisesRoot;
  qint64 firstYearOuvertsRoot = 0, firstYearRealisesRoot = 0;

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:refnode order by annee").arg(tableName));
  q.bindValue(":refnode", referenceNode);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForRoot = false;
  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    ouvertsRoot.insert(q.value(QStringLiteral("annee")).toUInt(), ouverts);
    realisesRoot.insert(q.value(QStringLiteral("annee")).toUInt(), realises);
    if (!doneFirstForRoot) {
      firstYearOuvertsRoot = ouverts;
      firstYearRealisesRoot = realises;
      doneFirstForRoot = true;
    }
  }

  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0;
  qint64 diffFirstYearRootNodeOuverts = 0, diffFirstYearRootNodeRealises = 0;

  while (q.next()) {
    qint64 diffRootNodeOuverts = 0, diffRootNodeRealises = 0;
    qint64 diffCurrentYearFirstYearOuverts = 0, diffCurrentYearFirstYearRealises = 0;

    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      // ouverts_1 in PCA
      diffFirstYearRootNodeOuverts = firstYearOuvertsRoot - firstYearOuvertsNode;
      diffFirstYearRootNodeRealises = firstYearRealisesRoot - firstYearRealisesNode;
      output.append(QString("<tr><td class='t3annee'>%1</td><td "
                            "class='t3pourcent'>&nbsp;</td><td "
                            "class='t3pourcent'>&nbsp;</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      // ouverts_2
      double percentOuverts = 0.0, percentRealises = 0.0;
      diffRootNodeOuverts = ouvertsRoot[annee] - ouverts;
      diffRootNodeRealises = realisesRoot[annee] - realises;
      diffCurrentYearFirstYearOuverts = diffRootNodeOuverts - diffFirstYearRootNodeOuverts;
      diffCurrentYearFirstYearRealises = diffRootNodeRealises - diffFirstYearRootNodeRealises;
      if (diffFirstYearRootNodeOuverts != 0) {
        percentOuverts = diffCurrentYearFirstYearOuverts * 100.0 / diffFirstYearRootNodeOuverts;
      }
      if (diffFirstYearRootNodeRealises != 0) {
        percentRealises = diffCurrentYearFirstYearRealises * 100.0 / diffFirstYearRootNodeRealises;
      }
      output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' "
                            "class='t3pourcent'>%2%</td><td align='right' "
                            "class='t3pourcent'>%3%</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, -1, true), formatDouble(percentRealises, -1, true)));

      // Here is the trick between T2 and T2bis, change the reference each year
      // (the css changes also)
      diffFirstYearRootNodeOuverts = diffRootNodeOuverts;
      diffFirstYearRootNodeRealises = diffRootNodeRealises;
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT3(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  // The classes "t2xxx" are not a mistake
  QString output =
      QString("\n<table class='t3' width='70%' align='center' cellpadding='5'>"
              "<tr class='t2entete'><td colspan=3 align='center'>"
              "<b>&Eacute;volution cumul&eacute;e du compte administratif "
              "de<br>[ %1 ] "
              "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t2entete'><th>Exercice</th><th>Pour les crédits "
              "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0;

  while (q.next()) {
    qint64 diffCurrentYearFirstYearOuverts = 0, diffCurrentYearFirstYearRealises = 0;

    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      output.append(QString("<tr><td class='t2annee'>%1</td><td "
                            "class='t2pourcent'>&nbsp;</td><td "
                            "class='t2pourcent'>&nbsp;</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      double percentOuverts = 0.0, percentRealises = 0.0;
      diffCurrentYearFirstYearOuverts = ouverts - firstYearOuvertsNode;
      diffCurrentYearFirstYearRealises = realises - firstYearRealisesNode;
      if (firstYearOuvertsNode != 0) {
        percentOuverts = diffCurrentYearFirstYearOuverts * 100.0 / firstYearOuvertsNode;
      }
      if (firstYearRealisesNode != 0) {
        percentRealises = diffCurrentYearFirstYearRealises * 100.0 / firstYearRealisesNode;
      }
      output.append(QString("<tr><td class='t2annee'>%1</td><td align='right' "
                            "class='t2pourcent'>%2%</td><td align='right' "
                            "class='t2pourcent'>%3%</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, -1, true), formatDouble(percentRealises, -1, true)));
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT3bis(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());
  QString output =
      QString("\n<table class='t3bis' width='70%' align='center' cellpadding='5'>"
              "<tr class='t3entete'><td colspan=3 align='center'>"
              "<b>&Eacute;volution du compte administratif de<br>[ %1 ] "
              "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t3entete'><th>Exercice</th><th>Pour les crédits "
              "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0;

  while (q.next()) {
    qint64 diffCurrentYearFirstYearOuverts = 0, diffCurrentYearFirstYearRealises = 0;

    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      output.append(QString("<tr><td class='t3annee'>%1</td><td "
                            "class='t3pourcent'>&nbsp;</td><td "
                            "class='t3pourcent'>&nbsp;</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      double percentOuverts = 0.0, percentRealises = 0.0;
      diffCurrentYearFirstYearOuverts = ouverts - firstYearOuvertsNode;
      diffCurrentYearFirstYearRealises = realises - firstYearRealisesNode;
      if (firstYearOuvertsNode != 0) {
        percentOuverts = diffCurrentYearFirstYearOuverts * 100.0 / firstYearOuvertsNode;
      }
      if (firstYearRealisesNode != 0) {
        percentRealises = diffCurrentYearFirstYearRealises * 100.0 / firstYearRealisesNode;
      }
      output.append(QString("<tr><td class='t3annee'>%1</td><td align='right' "
                            "class='t3pourcent'>%2%</td><td align='right' "
                            "class='t3pourcent'>%3%</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, -1, true), formatDouble(percentRealises, -1, true)));

      // Here is the only difference between T3 and T3bis (appart the styling of
      // tables)
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT4(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  if (node == 0 || auditModel == nullptr || referenceNode == 0) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());
  QString output =
      QString("\n<table class='t4' align='center' width='65%' cellpadding='5'>"
              "<tr class='t4entete'><td colspan=3 align='center'><b>Poids "
              "relatif de [ %1 ]<br>par rapport à [ %3 ] "
              "(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t4entete'><th>Exercice</th><th>Pour les crédits "
              "ouverts</th><th>Pour le r&eacute;alis&eacute;</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode),
               auditModel->getAttachedTree()->getNodeName(referenceNode).toHtmlEscaped());

  QHash<unsigned int, qint64> ouvertsRoot, realisesRoot;

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:refnode order by annee").arg(tableName));
  q.bindValue(":refnode", referenceNode);
  q.exec();
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    ouvertsRoot.insert(q.value(QStringLiteral("annee")).toUInt(), q.value(QStringLiteral("ouverts")).toLongLong());
    realisesRoot.insert(q.value(QStringLiteral("annee")).toUInt(), q.value(QStringLiteral("realises")).toLongLong());
  }

  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    if (annee == 0) {
      qFatal("Assertion failed");
    }

    double percentOuvertsRoot = 0.0, percentRealisesRoot = 0.0;

    if (ouvertsRoot[annee] != 0) {
      percentOuvertsRoot = ouverts * 100.0 / ouvertsRoot[annee];
    }
    if (realisesRoot[annee] != 0) {
      percentRealisesRoot = realises * 100.0 / realisesRoot[annee];
    }

    output.append(QString("<tr><td class='t4annee'>%1</td><td align='right' "
                          "class='t4pourcent'>%2%</td><td align='right' class='t4pourcent'>"
                          "%3%</td></tr>")
                      .arg(annee)
                      .arg(formatDouble(percentOuvertsRoot, -1, true), formatDouble(percentRealisesRoot, -1, true)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT5(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  if (node == 0 || auditModel == nullptr || referenceNode == 0) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table class='t5' width='80%' align='center' cellpadding='5'>"
              "<tr class='t5entete'><td colspan=6 align='center'>"
              "<b>Analyse en base 100 du compte administratif de [ %3 ] "
              "<u>hors celui de<br>[ %1 ]</u> (<span "
              "style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t5entete'><th valign='middle' rowspan=2>Exercice</th><th "
              "valign='middle' rowspan=2>Crédits ouverts</th>"
              "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th "
              "colspan=3 align='center'>Non consomm&eacute;</th></tr>"
              "<tr class='t5entete'><th valign='middle'>Total</th><th "
              "style='font-weight:normal'>dont<br>engag&eacute;</th><th "
              "style='font-weight:normal'>dont<br>disponible</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode),
               auditModel->getAttachedTree()->getNodeName(referenceNode).toHtmlEscaped());

  QMap<unsigned int, qint64> ouvertsRoot, realisesRoot, engagesRoot, disponiblesRoot;
  qint64 firstYearOuvertsRoot = 0, firstYearRealisesRoot = 0, firstYearEngagesRoot = 0, firstYearDisponiblesRoot = 0;

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:refnode order by annee").arg(tableName));
  q.bindValue(":refnode", referenceNode);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForRoot = false;
  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    ouvertsRoot.insert(annee, ouverts);
    realisesRoot.insert(annee, realises);
    engagesRoot.insert(annee, engages);
    disponiblesRoot.insert(annee, disponibles);
    if (!doneFirstForRoot) {
      firstYearOuvertsRoot = ouverts;
      firstYearRealisesRoot = realises;
      firstYearEngagesRoot = engages;
      firstYearDisponiblesRoot = disponibles;
      doneFirstForRoot = true;
    }
  }

  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0, firstYearEngagesNode = 0, firstYearDisponiblesNode = 0;
  qint64 diffFirstYearRootNodeOuverts = 0, diffFirstYearRootNodeRealises = 0, diffFirstYearRootNodeEngages = 0,
         diffFirstYearRootNodeDisponibles = 0, diffFirstYearRootNodeNC = 0;

  while (q.next()) {
    qint64 diffRootNodeOuverts = 0, diffRootNodeRealises = 0, diffRootNodeEngages = 0, diffRootNodeDisponibles = 0,
           diffRootNodeNC = 0;

    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();
    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      firstYearEngagesNode = engages;
      firstYearDisponiblesNode = disponibles;

      // ouverts_1
      diffFirstYearRootNodeOuverts = firstYearOuvertsRoot - firstYearOuvertsNode;
      diffFirstYearRootNodeRealises = firstYearRealisesRoot - firstYearRealisesNode;
      diffFirstYearRootNodeEngages = firstYearEngagesRoot - firstYearEngagesNode;
      diffFirstYearRootNodeDisponibles = firstYearDisponiblesRoot - firstYearDisponiblesNode;
      diffFirstYearRootNodeNC = diffFirstYearRootNodeEngages + diffFirstYearRootNodeDisponibles;

      output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' "
                            "class='t5pourcent'>100</td>"
                            "<td align='right' class='t5pourcent'>100</td><td "
                            "align='right' class='t5pourcent'>100</td>"
                            "<td align='right' class='t5valeur'>100</td><td "
                            "align='right' class='t5valeur'>100</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      // ouverts_2
      double percentOuverts = 0.0, percentRealises = 0.0, percentEngages = 0.0, percentDisponibles = 0.0,
             percentNC = 0.0;
      diffRootNodeOuverts = ouvertsRoot[annee] - ouverts;
      diffRootNodeRealises = realisesRoot[annee] - realises;
      diffRootNodeEngages = engagesRoot[annee] - engages;
      diffRootNodeDisponibles = disponiblesRoot[annee] - disponibles;
      diffRootNodeNC = diffRootNodeEngages + diffRootNodeDisponibles;

      if (diffFirstYearRootNodeOuverts * diffRootNodeOuverts != 0) {
        percentOuverts = 100.0 / diffFirstYearRootNodeOuverts * diffRootNodeOuverts;
      }

      if (diffFirstYearRootNodeRealises * diffRootNodeRealises != 0) {
        percentRealises = 100.0 / diffFirstYearRootNodeRealises * diffRootNodeRealises;
      }

      if (diffFirstYearRootNodeEngages * diffRootNodeEngages != 0) {
        percentEngages = 100.0 / diffFirstYearRootNodeEngages * diffRootNodeEngages;
      }

      if (diffFirstYearRootNodeDisponibles * diffRootNodeDisponibles != 0) {
        percentDisponibles = 100.0 / diffFirstYearRootNodeDisponibles * diffRootNodeDisponibles;
      }

      if (diffRootNodeNC + diffFirstYearRootNodeNC != 0) {
        percentNC = 100.0 / diffFirstYearRootNodeNC * diffRootNodeNC;
      }

      output.append(QString("<tr><td class='t5annee'>%1</td><td align='right' "
                            "class='t5pourcent'>%2</td>"
                            "<td align='right' class='t5pourcent'>%3</td><td "
                            "align='right' class='t5pourcent'>%4</td>"
                            "<td align='right' class='t5valeur'>%5</td><td "
                            "align='right' class='t5valeur'>%6</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, 0, true), formatDouble(percentRealises, 0, true),
                             formatDouble(percentNC, 0, true), formatDouble(percentEngages, 0, true),
                             formatDouble(percentDisponibles, 0, true)));
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT6(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table class='t6' width='80%' align='center' cellpadding='5'>"
              "<tr class='t6entete'><td colspan=6 align='center'>"
              "<b>Analyse en base 100 du compte administratif de [ %1 ]"
              "<br>(<span style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t6entete'><th valign='middle' rowspan=2>Exercice</th><th "
              "valign='middle' rowspan=2>Crédits ouverts</th>"
              "<th valign='middle' rowspan=2>R&eacute;alis&eacute;</th><th "
              "colspan=3 align='center'>Non consomm&eacute;</th></tr>"
              "<tr class='t6entete'><th valign='middle'>Total</th><th "
              "style='font-weight:normal'>dont<br>engag&eacute;</th><th "
              "style='font-weight:normal'>dont<br>disponible</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  bool doneFirstForNode = false;
  qint64 firstYearOuvertsNode = 0, firstYearRealisesNode = 0, firstYearEngagesNode = 0, firstYearDisponiblesNode = 0,
         firstYearNodeNC = 0;

  while (q.next()) {
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();
    qint64 nc = engages + disponibles;
    if (!doneFirstForNode) {
      firstYearOuvertsNode = ouverts;
      firstYearRealisesNode = realises;
      firstYearEngagesNode = engages;
      firstYearDisponiblesNode = disponibles;
      firstYearNodeNC = nc;

      output.append(QString("<tr><td class='t6annee'>%1</td><td align='right' "
                            "class='t6pourcent'>100</td>"
                            "<td align='right' class='t6pourcent'>100</td><td "
                            "align='right' class='t6pourcent'>100</td>"
                            "<td align='right' class='t6valeur'>100</td><td "
                            "align='right' class='t6valeur'>100</td></tr>")
                        .arg(annee));
      doneFirstForNode = true;
    } else {
      double percentOuverts = 0.0, percentRealises = 0.0, percentEngages = 0.0, percentDisponibles = 0.0,
             percentNC = 0.0;
      if (ouverts * firstYearOuvertsNode != 0) {
        percentOuverts = 100.0 / firstYearOuvertsNode * ouverts;
      }

      if (realises * firstYearRealisesNode != 0) {
        percentRealises = 100.0 / firstYearRealisesNode * realises;
      }

      if (engages * firstYearEngagesNode != 0) {
        percentEngages = 100.0 / firstYearEngagesNode * engages;
      }

      if (disponibles * firstYearDisponiblesNode != 0) {
        percentDisponibles = 100.0 / firstYearDisponiblesNode * disponibles;
      }

      if (nc * firstYearNodeNC != 0) {
        percentNC = 100.0 / firstYearNodeNC * nc;
      }

      output.append(QString("<tr><td class='t6annee'>%1</td><td align='right' "
                            "class='t6pourcent'>%2</td>"
                            "<td align='right' class='t6pourcent'>%3</td><td "
                            "align='right' class='t6pourcent'>%4</td>"
                            "<td align='right' class='t6valeur'>%5</td><td "
                            "align='right' class='t6valeur'>%6</td></tr>")
                        .arg(annee)
                        .arg(formatDouble(percentOuverts, 0, true), formatDouble(percentRealises, 0, true),
                             formatDouble(percentNC, 0, true), formatDouble(percentEngages, 0, true),
                             formatDouble(percentDisponibles, 0, true)));
    }
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT7(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table class='t7' width='80%' align='center' cellpadding='5'>"
              "<tr class='t7entete'><td colspan=5 align='center'>"
              "<b>Transcription en &laquo;&nbsp;jours "
              "activit&eacute;&nbsp;&raquo; de<br>"
              "[ %1 ] (<span style='color:#7c0000'>%2</span>)<br><u>sur la "
              "base d'une &eacute;quivalence de"
              " 365/365<sup>&egrave;me</sup> entre le r&eacute;alis&eacute; "
              "budg&eacute;taire et l'ann&eacute;e</u></b>"
              "<tr class='t7entete'><th>Exercice</th><th>Crédits "
              "ouverts</th><th>R&eacute;alis&eacute;</th>"
              "<th>Engag&eacute;</th><th>Disponible</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();
    double percentOuverts = 0.0, percentRealises = 0.0, percentEngages = 0.0, percentDisponibles = 0.0;

    if (realises != 0) {
      percentOuverts = 365.0 * ouverts / realises;
      percentRealises = 365.0;
      percentEngages = 365.0 * engages / realises;
      percentDisponibles = 365.0 * disponibles / realises;
    }
    output.append(QString("<tr><td class='t7annee'>%1</td><td align='right' "
                          "class='t7pourcent'>%2</td>"
                          "<td align='right' class='t7pourcent'>%3</td><td "
                          "align='right' class='t7pourcent'>%4</td>"
                          "<td align='right' class='t7pourcent'>%5</td></tr>")
                      .arg(annee)
                      .arg(formatDouble(percentOuverts, 0, true), formatDouble(percentRealises, 0, true),
                           formatDouble(percentEngages, 0, true), formatDouble(percentDisponibles, 0, true)));
  }
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT8(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());
  QString output =
      QString("\n<table class='t8' width='50%' align='center' cellpadding='5'>"
              "<tr class='t8entete'><td colspan=2 align='center'><b>Moyennes "
              "budg&eacute;taires de<br>[ %1 ] "
              "(<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es "
              "pour la p&eacute;riode audit&eacute;e</b></td></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  qint64 sumOuverts = 0, sumRealises = 0, sumEngages = 0, sumDisponibles = 0;

  while (q.next()) {
    sumOuverts += q.value(QStringLiteral("ouverts")).toLongLong();
    sumRealises += q.value(QStringLiteral("realises")).toLongLong();
    sumEngages += q.value(QStringLiteral("engages")).toLongLong();
    sumDisponibles += q.value(QStringLiteral("disponibles")).toLongLong();
  }

  double percentRealisesOuverts = 0.0, percentEngagesOuverts = 0.0, percentDisponiblesOuverts = 0.0;

  if (sumOuverts != 0) {
    percentRealisesOuverts = sumRealises * 100.0 / sumOuverts;
    percentEngagesOuverts = sumEngages * 100.0 / sumOuverts;
    percentDisponiblesOuverts = sumDisponibles * 100.0 / sumOuverts;
  }

  output.append(QString("<tr><td class='t8annee' "
                        "align='center'>R&eacute;alis&eacute;</td><td align='right' "
                        "class='t8pourcent'>%1%</td></tr>"
                        "<tr><td class='t8annee' align='center' "
                        "style='font-weight:normal'>Non utilis&eacute;</td><td "
                        "align='right' class='t8valeur'>%2%</td></tr>"
                        "<tr><td class='t8annee' align='center'><i>dont "
                        "engag&eacute;</i></td><td align='right' "
                        "class='t8pourcent'><i>%3%</i></td></tr>"
                        "<tr><td class='t8annee' align='center'><i>dont "
                        "disponible</i></td><td align='right' "
                        "class='t8pourcent'><i>%4%</i></td></tr>")
                    .arg(formatDouble(percentRealisesOuverts, -1, true),
                         formatDouble(percentDisponiblesOuverts + percentEngagesOuverts, -1, true),
                         formatDouble(percentEngagesOuverts, -1, true),
                         formatDouble(percentDisponiblesOuverts, -1, true)));

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT9(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table class='t9' width='60%' align='center' cellpadding='5'>"
              "<tr class='t9entete'><td colspan=2 align='center'>"
              "<b>&Eacute;quivalences moyennes en &laquo;&nbsp;jours "
              "activit&eacute;&nbsp;&raquo; de<br>[ %1 ]"
              " (<span style='color:#7c0000'>%2</span>)<br>constat&eacute;es "
              "pour la p&eacute;riode audit&eacute;e</b>"
              "</td></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  qint64 sumOuverts = 0, sumRealises = 0, sumEngages = 0, sumDisponibles = 0;
  double percentOuverts = 0.0, percentNC = 0.0, percentEngages = 0.0, percentDisponibles = 0.0;

  while (q.next()) {
    sumOuverts += q.value(QStringLiteral("ouverts")).toLongLong();
    sumRealises += q.value(QStringLiteral("realises")).toLongLong();
    sumEngages += q.value(QStringLiteral("engages")).toLongLong();
    sumDisponibles += q.value(QStringLiteral("disponibles")).toLongLong();
  }
  if (sumRealises != 0) {
    percentOuverts = sumOuverts * 365.0 / sumRealises;
    percentEngages = sumEngages * 365.0 / sumRealises;
    percentDisponibles = sumDisponibles * 365.0 / sumRealises;
    percentNC = percentEngages + percentDisponibles;
  }
  output.append(QString("<tr><td colspan='2' align='center' class='t9annee'>Face aux "
                        "cr&eacute;dits r&eacute;alis&eacute;s, les pr&eacute;visions "
                        "correspondent"
                        " &agrave; :<br><b>%1 jours de travail par an</b></td></tr>"
                        "<tr><td class='t9annee' "
                        "style='font-weight:normal'>D&eacute;passement de "
                        "capacit&eacute; des pr&eacute;visions</td><td align='right' "
                        "class='t9valeur' valign='middle'>%2 jours</td></tr>"
                        "<tr><td class='t9annee'><i>dont engag&eacute;</i></td><td "
                        "class='t9pourcent' align='right' valign='middle'><i>%3 "
                        "jours</i></td></tr>"
                        "<tr><td class='t9annee'><i>dont disponible</i></td><td "
                        "class='t9pourcent' align='right' valign='middle'><i>%4 "
                        "jours</i></td></tr>")
                    .arg(formatDouble(percentOuverts, 0, true), formatDouble(percentNC, 0, true),
                         formatDouble(percentEngages, 0, true), formatDouble(percentDisponibles, 0, true)));
  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT10(unsigned int node) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }
  QString output = QString("\n<table class='t10' width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' "
                           "colspan=4><b>R&Eacute;SULTATS de FONCTIONNEMENT de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e "
                           "(0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
                           "</span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire "
                           "(-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Crédits "
                           "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</"
                           "th></tr>")
                       .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped());

  QSqlQuery q;
  q.prepare(QString("select a.annee,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as "
                    "diff_ouverts, "
                    "coalesce(a.realises,0)-coalesce(b.realises,0) as diff_realises, "
                    "coalesce(a.engages,0)-coalesce(b.engages,0) as diff_engages "
                    "from audit_RF_%1 as a, audit_DF_%1 as b where a.id_node=:id and "
                    "b.id_node=:id "
                    "and a.annee=b.annee order by a.annee")
                .arg(auditModel->getAuditId()));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 diff_ouverts = q.value(QStringLiteral("diff_ouverts")).toLongLong();
    qint64 diff_realises = q.value(QStringLiteral("diff_realises")).toLongLong();
    qint64 diff_engages = q.value(QStringLiteral("diff_engages")).toLongLong();

    output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>")
                      .arg(annee));
    if (diff_ouverts == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_ouverts > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_ouverts)));

    if (diff_realises == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_realises > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_realises)));

    if (diff_engages == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_engages > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td></tr>").arg(formatFixedPoint(diff_engages)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT11(unsigned int node) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString output = QString("\n<table class='t11' width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' "
                           "colspan=4><b>R&Eacute;SULTATS d'INVESTISSEMENT de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e "
                           "(0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
                           "</span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire "
                           "(-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Crédits "
                           "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</"
                           "th></tr>")
                       .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped());

  QSqlQuery q;
  q.prepare(QString("select a.annee,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as "
                    "diff_ouverts, "
                    "coalesce(a.realises,0)-coalesce(b.realises,0) as diff_realises, "
                    "coalesce(a.engages,0)-coalesce(b.engages,0) as diff_engages "
                    "from audit_RI_%1 as a, audit_DI_%1 as b where a.id_node=:id and "
                    "b.id_node=:id "
                    "and a.annee=b.annee order by a.annee")
                .arg(auditModel->getAuditId()));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 diff_ouverts = q.value(QStringLiteral("diff_ouverts")).toLongLong();
    qint64 diff_realises = q.value(QStringLiteral("diff_realises")).toLongLong();
    qint64 diff_engages = q.value(QStringLiteral("diff_engages")).toLongLong();

    output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>")
                      .arg(annee));
    if (diff_ouverts == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_ouverts > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_ouverts)));

    if (diff_realises == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_realises > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_realises)));

    if (diff_engages == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_engages > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td></tr>").arg(formatFixedPoint(diff_engages)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAT12(unsigned int node) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString output = QString("\n<table class='t12' width='80%' align='center' cellpadding='5'>"
                           "<tr class='t1entete'><td align='center' "
                           "colspan=4><b>R&Eacute;SULTATS BUDG&Eacute;TAIRES de [ %1 ]"
                           "<br>UCC <span style='color:black'>&eacute;quilibr&eacute;e "
                           "(0)</span> <span style='color:#008000'>exc&eacute;dentaire (+) "
                           "</span>"
                           "<span style='color:#7c0000'>d&eacute;ficitaire "
                           "(-)</span></b></td></tr>"
                           "<tr class='t1entete'><th>Exercice</th><th>Crédits "
                           "ouverts</th><th>R&eacute;alis&eacute;</th><th>Engag&eacute;</"
                           "th></tr>")
                       .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped());

  QSqlQuery q;
  q.prepare(QString("select a.annee, "
                    "(coalesce(a.ouverts,0)-coalesce(b.ouverts,0)) "
                    " + (coalesce(c.ouverts,0)-coalesce(d.ouverts,0)) as diff_ouverts, "
                    "(coalesce(a.realises,0)-coalesce(b.realises,0)) "
                    " + (coalesce(c.realises,0)-coalesce(d.realises,0)) as "
                    "diff_realises, "
                    "(coalesce(a.engages,0)-coalesce(b.engages,0)) "
                    " + (coalesce(c.engages,0)-coalesce(d.engages,0)) as diff_engages "
                    "from audit_RF_%1 as a, audit_DF_%1 as b, audit_RI_%1 as c, "
                    "audit_DI_%1 as d"
                    " where a.id_node=:id and b.id_node=:id and c.id_node=:id and "
                    "d.id_node=:id "
                    "and a.annee=b.annee and b.annee=c.annee and c.annee=d.annee order "
                    "by a.annee")
                .arg(auditModel->getAuditId()));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    unsigned int annee = q.value(QStringLiteral("annee")).toUInt();
    qint64 diff_ouverts = q.value(QStringLiteral("diff_ouverts")).toLongLong();
    qint64 diff_realises = q.value(QStringLiteral("diff_realises")).toLongLong();
    qint64 diff_engages = q.value(QStringLiteral("diff_engages")).toLongLong();

    output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>")
                      .arg(annee));
    if (diff_ouverts == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_ouverts > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_ouverts)));

    if (diff_realises == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_realises > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td><td align='right' class='t1valeur'>").arg(formatFixedPoint(diff_realises)));

    if (diff_engages == 0) {
      output.append(QStringLiteral("<span style='color:black'>"));
    } else if (diff_engages > 0) {
      output.append(QStringLiteral("<span style='color:#008000'>"));
    } else {
      output.append(QStringLiteral("<span style='color:#7c0000'>"));
    }

    output.append(QString("%1</span></td></tr>").arg(formatFixedPoint(diff_engages)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCRRatioParents(unsigned int node, MODES::DFRFDIRI mode) const {
  if (reportingModel == nullptr) {
    qWarning() << "Invalid model usage";
    return QString();
  }

  QDate lastDate = reportingModel->getLastReportingDate(mode, node);
  if (lastDate.isNull() || !lastDate.isValid()) {
    return QString();
  }

  qint64 nodeVal = reportingModel->getNodeValue(node, mode, PCx_Reporting::OREDPCR::OUVERTS, lastDate);

  QString out = QObject::tr("<p>Le %1, date de la dernière situation PCR, les crédits "
                            "ouverts pour <strong>%2</strong> s'élevaient à %3 € %4</p>")
                    .arg(lastDate.toString(Qt::DefaultLocaleShortDate),
                         reportingModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(),
                         NUMBERSFORMAT::formatFixedPoint(nodeVal), node != 1 ? "représentant : " : "");

  out.append(QStringLiteral("<ul>"));
  unsigned int pid;
  unsigned int tmpNode = node;

  while ((pid = reportingModel->getAttachedTree()->getParentId(tmpNode)) != 0) {
    qint64 pidVal = reportingModel->getNodeValue(pid, mode, PCx_Reporting::OREDPCR::OUVERTS, lastDate);
    if (pidVal != 0) {
      out.append(QString("<li>%1 % des crédits de <b>%2</b></li>")
                     .arg(formatDouble(static_cast<double>(nodeVal) / pidVal * 100.0, -1, true),
                          reportingModel->getAttachedTree()->getNodeName(pid).toHtmlEscaped()));
    }
    tmpNode = pid;
  }
  out.append(QStringLiteral("</ul>"));

  return out;
}

QString PCx_Tables::getPCRProvenance(unsigned int node, MODES::DFRFDIRI mode) const {
  QSqlQuery q;
  q.prepare(QString("select * from reporting_%1_%2 where id_node=:idnode order "
                    "by date desc limit 1")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":idnode", node);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  double percentBP = std::numeric_limits<double>::quiet_NaN();
  double percentReports = std::numeric_limits<double>::quiet_NaN();
  double percentOCDM = std::numeric_limits<double>::quiet_NaN();
  double percentVCDM = std::numeric_limits<double>::quiet_NaN();
  double percentVINT = std::numeric_limits<double>::quiet_NaN();
  double sumPercent = std::numeric_limits<double>::quiet_NaN();

  if (q.next()) {
    qint64 ouverts = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)).toLongLong();
    qint64 bp = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::BP)).toLongLong();
    qint64 reports = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REPORTS)).toLongLong();
    qint64 ocdm = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OCDM)).toLongLong();
    qint64 vcdm = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::VCDM)).toLongLong();
    qint64 vInt = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES)).toLongLong();
    if (ouverts != 0) {
      percentBP = static_cast<double>(bp) / ouverts * 100.0;
      percentReports = static_cast<double>(reports) / ouverts * 100.0;
      percentOCDM = static_cast<double>(ocdm) / ouverts * 100.0;
      percentVCDM = static_cast<double>(vcdm) / ouverts * 100.0;
      percentVINT = static_cast<double>(vInt) / ouverts * 100.0;
      sumPercent = percentBP + percentReports + percentOCDM + percentVCDM + percentVINT;
    }

    qint64 sum = bp + reports + ocdm + vcdm + vInt;

    QString out = QString("<table class='tPCRProvenance' align='center' cellpadding='5'><tr "
                          "class='t1entete'><th>PROVENANCE</th><th>MONTANT</th><th>Part des "
                          "cr&eacute;dits ouverts</th></tr>"
                          "<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>%2</td><td align='right' class='t1pourcent'>%3 "
                          "%</td></tr>")
                      .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP, true),
                           NUMBERSFORMAT::formatFixedPoint(bp), NUMBERSFORMAT::formatDouble(percentBP, -1, true));
    out.append(QString("<tr><td class='t1annee'>%1</td><td class='t1valeur' "
                       "align='right'>%2</td><td align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS, true),
                        NUMBERSFORMAT::formatFixedPoint(reports),
                        NUMBERSFORMAT::formatDouble(percentReports, -1, true)));
    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM, true),
                        NUMBERSFORMAT::formatFixedPoint(ocdm), NUMBERSFORMAT::formatDouble(percentOCDM, -1, true)));
    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM, true),
                        NUMBERSFORMAT::formatFixedPoint(vcdm), NUMBERSFORMAT::formatDouble(percentVCDM, -1, true)));
    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES, true),
                        NUMBERSFORMAT::formatFixedPoint(vInt), NUMBERSFORMAT::formatDouble(percentVINT, -1, true)));
    out.append(QString("<tr><td class='t1annee'><b>TOTAL</b></td><td align='right'  "
                       "class='t1valeur'><b>%1</b></td><td align='right'  "
                       "class='t1pourcent'><b>%2 %</b></td></tr></table>")
                   .arg(NUMBERSFORMAT::formatFixedPoint(sum), NUMBERSFORMAT::formatDouble(sumPercent, -1, true)));

    return out;
  }
  return QString();
}

QString PCx_Tables::getPCRVariation(unsigned int node, MODES::DFRFDIRI mode) const {
  QSqlQuery q;
  q.prepare(QString("select * from reporting_%1_%2 where id_node=:idnode order "
                    "by date desc limit 1")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":idnode", node);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  double percentOCDM = std::numeric_limits<double>::quiet_NaN();
  double percentVCDM = std::numeric_limits<double>::quiet_NaN();
  double percentVINT = std::numeric_limits<double>::quiet_NaN();
  double sumPercent = std::numeric_limits<double>::quiet_NaN();

  if (q.next()) {
    unsigned int dateTimeT = q.value("date").toUInt();
    QDate laDate = QDateTime::fromTime_t(dateTimeT).date();
    qint64 bp = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::BP)).toLongLong();
    qint64 ocdm = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OCDM)).toLongLong();
    qint64 vcdm = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::VCDM)).toLongLong();
    qint64 vInt = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES)).toLongLong();
    if (bp != 0) {
      percentOCDM = static_cast<double>(ocdm) / bp * 100.0;
      percentVCDM = static_cast<double>(vcdm) / bp * 100.0;
      percentVINT = static_cast<double>(vInt) / bp * 100.0;
      sumPercent = percentOCDM + percentVCDM + percentVINT;
    }

    qint64 sum = ocdm + vcdm + vInt;

    QString out = QStringLiteral("<table class='tPCRVARIATION' align='center' cellpadding='5'><tr "
                                 "class='t1entete'><th>PROVENANCE</th><th>MONTANT</th><th>Variation en "
                                 "% du BP</th></tr>");

    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM, true),
                        NUMBERSFORMAT::formatFixedPoint(ocdm), NUMBERSFORMAT::formatDouble(percentOCDM, -1, true)));
    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM, true),
                        NUMBERSFORMAT::formatFixedPoint(vcdm), NUMBERSFORMAT::formatDouble(percentVCDM, -1, true)));
    out.append(QString("<tr><td class='t1annee'>%1</td><td  align='right' "
                       "class='t1valeur'>%2</td><td  align='right' "
                       "class='t1pourcent'>%3 %</td></tr>")
                   .arg(PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES, true),
                        NUMBERSFORMAT::formatFixedPoint(vInt), NUMBERSFORMAT::formatDouble(percentVINT, -1, true)));
    out.append(QString("<tr><td class='t1annee'><b>TOTAL</b></td><td align='right'  "
                       "class='t1valeur'><b>%1</b></td><td align='right'  "
                       "class='t1pourcent'><b>%2 %</b></td></tr></table>")
                   .arg(NUMBERSFORMAT::formatFixedPoint(sum), NUMBERSFORMAT::formatDouble(sumPercent, -1, true)));

    out.prepend(QString("<p>Entre le 1er janvier et le %1, date de la dernière situation "
                        "PCR, les crédits alloués au budget primitif à <b>%2</b> ont "
                        "enregistré une variation de %3 €, correspondant à %4 % de "
                        "l'autorisation d'origine</p>")
                    .arg(laDate.toString(Qt::DefaultLocaleShortDate),
                         reportingModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(),
                         NUMBERSFORMAT::formatFixedPoint(sum), NUMBERSFORMAT::formatDouble(sumPercent, -1, true)));

    return out;
  }
  return QString();
}

QString PCx_Tables::getPCRUtilisation(unsigned int node, MODES::DFRFDIRI mode) const {
  QSqlQuery q;
  q.prepare(QString("select * from reporting_%1_%2 where id_node=:idnode order "
                    "by date desc limit 1")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":idnode", node);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  double percentRealises = std::numeric_limits<double>::quiet_NaN();
  double percentEngages = std::numeric_limits<double>::quiet_NaN();
  double percentDispo = std::numeric_limits<double>::quiet_NaN();
  double percentNonUtilise = std::numeric_limits<double>::quiet_NaN();
  double percentUtilise = std::numeric_limits<double>::quiet_NaN();

  if (q.next()) {
    // unsigned int dateTimeT=q.value("date").toUInt();
    // QDate laDate=QDateTime::fromTime_t(dateTimeT).date();
    qint64 ouverts = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)).toLongLong();
    qint64 realises = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)).toLongLong();
    qint64 engages = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::ENGAGES)).toLongLong();
    qint64 disponibles = q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::DISPONIBLES)).toLongLong();
    qint64 nonUtilise = engages + disponibles;
    qint64 utilise = realises + engages;
    if (ouverts != 0) {
      percentRealises = static_cast<double>(realises) / ouverts * 100.0;
      percentEngages = static_cast<double>(engages) / ouverts * 100.0;
      percentDispo = static_cast<double>(disponibles) / ouverts * 100.0;
      percentNonUtilise = static_cast<double>(nonUtilise) / ouverts * 100.0;
      percentUtilise = static_cast<double>(utilise) / ouverts * 100.0;
    }

    // First table
    QString outGlob(QLatin1String("<table class='tPCRUTILISATION' "
                                  "align='center' cellspacing='5'><tr><td>"));

    QString out("<table width='100%' cellpadding='5'><tr class='t1entete'><th "
                "colspan='3' "
                "align='right'>Pourcentage&nbsp;des&nbsp;crédits&nbsp;ouverts</"
                "th></tr>");

    out.append(QStringLiteral("<tr><td colspan='3' class='t1annee'>UTILISÉ</td></tr>"));

    out.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Réalisé</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr>")
            .arg(NUMBERSFORMAT::formatFixedPoint(realises), NUMBERSFORMAT::formatDouble(percentRealises, -1, true)));
    // out.append("<tr><td colspan='3'>&nbsp;</td></tr>");
    out.append(QString("<tr><td class='t1annee'>NON&nbsp;UTILISÉ</td><td "
                       "align='right' class='t1valeur'>%1</td><td  align='right' "
                       "class='t1pourcent'>%2 %</td></tr>")
                   .arg(NUMBERSFORMAT::formatFixedPoint(nonUtilise),
                        NUMBERSFORMAT::formatDouble(percentNonUtilise, -1, true)));
    out.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Engagé</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr>")
            .arg(NUMBERSFORMAT::formatFixedPoint(engages), NUMBERSFORMAT::formatDouble(percentEngages, -1, true)));
    out.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Disponible</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr></table>")
            .arg(NUMBERSFORMAT::formatFixedPoint(disponibles), NUMBERSFORMAT::formatDouble(percentDispo, -1, true)));

    outGlob.append(out);
    outGlob.append(QStringLiteral("</td><td>"));

    QString out2("<table width='100%' cellpadding='5'><tr class='t1entete'><th "
                 "colspan='3' "
                 "align='right'>Pourcentage&nbsp;des&nbsp;crédits&nbsp;ouverts<"
                 "/th></tr>");
    out2.append(
        QString("<tr><td class='t1annee'>UTILISÉ</td><td align='right' "
                "class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr>")
            .arg(NUMBERSFORMAT::formatFixedPoint(utilise), NUMBERSFORMAT::formatDouble(percentUtilise, -1, true)));
    out2.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Réalisé</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr>")
            .arg(NUMBERSFORMAT::formatFixedPoint(realises), NUMBERSFORMAT::formatDouble(percentRealises, -1, true)));
    out2.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Engagé</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr>")
            .arg(NUMBERSFORMAT::formatFixedPoint(engages), NUMBERSFORMAT::formatDouble(percentEngages, -1, true)));
    // out2.append("<tr><td colspan='3'>&nbsp;</td></tr>");

    out2.append(QStringLiteral("<tr><td colspan='3' class='t1annee'>NON&nbsp;UTILISÉ</td></tr>"));

    out2.append(
        QString("<tr><td class='t1annee' align='right'>=>&nbsp;Disponible</td><td "
                "align='right' class='t1valeur'>%1</td><td  align='right' "
                "class='t1pourcent'>%2 %</td></tr></table>")
            .arg(NUMBERSFORMAT::formatFixedPoint(disponibles), NUMBERSFORMAT::formatDouble(percentDispo, -1, true)));

    outGlob.append(out2);
    outGlob.append(QStringLiteral("</td></tr></table>"));

    return outGlob;
  }

  return QString();
}

QString PCx_Tables::getPCRCycles(unsigned int node, MODES::DFRFDIRI mode) const {
  QSqlQuery q;
  q.prepare(QString("select * from reporting_%1_%2 where id_node=:idnode order "
                    "by date desc limit 1")
                .arg(MODES::modeToTableString(mode))
                .arg(reportingModel->getReportingId()));
  q.bindValue(":idnode", node);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.next()) {
    unsigned int dateTimeT = q.value("date").toUInt();
    QDate laDate = QDateTime::fromTime_t(dateTimeT).date();
    double ouverts = NUMBERSFORMAT::fixedPointToDouble(
        q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::OUVERTS)).toLongLong());
    double realises = NUMBERSFORMAT::fixedPointToDouble(
        q.value(PCx_Reporting::OREDPCRtoTableString(PCx_Reporting::OREDPCR::REALISES)).toLongLong());

    double journ = ouverts / 365;
    double hebd = journ * 7;
    double mens = ouverts / 12;

    int nbJoursDepuisDebutAnnee = laDate.dayOfYear();
    double journReal = realises / nbJoursDepuisDebutAnnee;
    double hebdReal = journReal * 7;
    double mensReal = journReal * 365 / 12;

    QString out = QString("<ol><li>Utilisation <b>théorique</b> moyenne des crédits :<ul>"
                          "<li>journalière = <b>%1</b> €</li>"
                          "<li>hebdomadaire = <b>%2</b> €</li>"
                          "<li>mensuelle = <b>%3</b> €</li>"
                          "<li>trimestrielle = <b>%4</b> €</li>"
                          "<li>semestrielle = <b>%5</b> €</li>"
                          "<li>annuelle = <b>%6</b> €</li>"
                          "</ul></li>")
                      .arg(NUMBERSFORMAT::formatDouble(journ), NUMBERSFORMAT::formatDouble(hebd),
                           NUMBERSFORMAT::formatDouble(mens), NUMBERSFORMAT::formatDouble(mens * 3),
                           NUMBERSFORMAT::formatDouble(mens * 6), NUMBERSFORMAT::formatDouble(ouverts));

    out.append(QString("<li>Utilisation <b>effective</b> moyenne des crédits :<ul>"
                       "<li>journalière = <b>%1</b> €</li>")
                   .arg(NUMBERSFORMAT::formatDouble(journReal)));

    if (nbJoursDepuisDebutAnnee < 7) {
      out.append(QString("<li><em><span style='color:#aaa'>hebdomadaire = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(hebdReal)));
    } else {
      out.append(QString("<li>hebdomadaire = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(hebdReal)));
    }

    if (nbJoursDepuisDebutAnnee < 30) {
      out.append(QString("<li><em><span style='color:#aaa'>mensuelle = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(mensReal)));
    } else {
      out.append(QString("<li>mensuelle = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(mensReal)));
    }
    if (nbJoursDepuisDebutAnnee < 91) {
      out.append(QString("<li><em><span style='color:#aaa'>trimestrielle = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(mensReal * 3)));
    } else {
      out.append(QString("<li>trimestrielle = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(mensReal * 3)));
    }

    if (nbJoursDepuisDebutAnnee < 182) {
      out.append(QString("<li><em><span style='color:#aaa'>semestrielle = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(mensReal * 6)));
    } else {
      out.append(QString("<li>semestrielle = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(mensReal * 6)));
    }

    out.append(QString("<li><em><span style='color:#aaa'>annuelle = <b>%1</b> "
                       "€</span></em></li></ul></li>")
                   .arg(NUMBERSFORMAT::formatDouble(journReal * 365)));

    out.append(QString("<li>Écart entre les utilisations <b>effective</b> et "
                       "<b>théorique</b> moyennes des crédits :<ul>"
                       "<li>journalière = <b>%1</b> €</li>")
                   .arg(NUMBERSFORMAT::formatDouble(journReal - journ)));
    if (nbJoursDepuisDebutAnnee < 7) {
      out.append(QString("<li><em><span style='color:#aaa'>hebdomadaire = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(hebdReal - hebd)));
    } else {
      out.append(QString("<li>hebdomadaire = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(hebdReal - hebd)));
    }

    if (nbJoursDepuisDebutAnnee < 30) {
      out.append(QString("<li><em><span style='color:#aaa'>mensuelle = "
                         "<b>%1</b> €</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble(mensReal - mens)));
    } else {
      out.append(QString("<li>mensuelle = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble(mensReal - mens)));
    }
    if (nbJoursDepuisDebutAnnee < 91) {
      out.append(QString("<li><em><span style='color:#aaa'>trimestrielle = <b>%1</b> "
                         "€</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble((mensReal * 3) - (mens * 3))));
    } else {
      out.append(QString("<li>trimestrielle = <b>%1</b> €</li>")
                     .arg(NUMBERSFORMAT::formatDouble((mensReal * 3) - (mens * 3))));
    }

    if (nbJoursDepuisDebutAnnee < 182) {
      out.append(QString("<li><em><span style='color:#aaa'>semestrielle = <b>%1</b> "
                         "€</span></em></li>")
                     .arg(NUMBERSFORMAT::formatDouble((mensReal * 6) - (mens * 6))));
    } else {
      out.append(
          QString("<li>semestrielle = <b>%1</b> €</li>").arg(NUMBERSFORMAT::formatDouble((mensReal * 6) - (mens * 6))));
    }

    out.append(QString("<li><em><span style='color:#aaa'>annuelle = <b>%1</b> "
                       "€</span></em></li></ul></li>")
                   .arg(NUMBERSFORMAT::formatDouble((journReal * 365) - (journ * 365))));

    if (q.value("ouverts").toLongLong() != 0) {
      out.append(QString("<li>Taux d'écart constaté le %1, date de la dernière "
                         "situation PCR = <b>%2 %</b></li></ol>")
                     .arg(laDate.toString(Qt::DefaultLocaleShortDate),
                          NUMBERSFORMAT::formatDouble((journReal * 365 - ouverts) / ouverts * 100)));

      out.append(QStringLiteral("<p>Le taux d'écart signale, s'il est négatif, un cycle à venir, "
                                "s'il est positif, un cycle en cours ou passé."
                                " En l'absence de cycle, le taux d'écart anticipe, s'il est positif, "
                                "une insuffisance de crédits, s'il est négatif un excédent de "
                                "crédits.</p>"));
    }

    return out;
  }
  return QString();
}

QString PCx_Tables::getPCRRFDF(unsigned int node) const {
  PCx_ReportingTableOverviewModel tableModel(reportingModel, node, PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF);
  QTableView view;
  view.setModel(&tableModel);
  return qTableViewToHtml(&view);
}

QString PCx_Tables::getPCRRIDI(unsigned int node) const {
  PCx_ReportingTableOverviewModel tableModel(reportingModel, node, PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI);
  QTableView view;
  view.setModel(&tableModel);
  return qTableViewToHtml(&view);
}

QString PCx_Tables::getPCRRFDFRIDI(unsigned int node) const {
  PCx_ReportingTableOverviewModel tableModel(reportingModel, node,
                                             PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI);
  QTableView view;
  view.setModel(&tableModel);
  return qTableViewToHtml(&view);
}

QStandardItemModel *PCx_Tables::getTreeModelOfAvailableAuditTables(bool richTooltip) const {
  QStandardItemModel *model = new QStandardItemModel();
  QStandardItem *item;

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT10));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT10), PCx_Tables::TableIdUserRole);
  item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT10(1) + "</span>");
  }
  model->invisibleRootItem()->appendRow(item);

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT11));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT11), PCx_Tables::TableIdUserRole);
  item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT11(1) + "</span>");
  }
  model->invisibleRootItem()->appendRow(item);

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT12));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT12), PCx_Tables::TableIdUserRole);
  item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT12(1) + "</span>");
  }
  model->invisibleRootItem()->appendRow(item);

  QMap<MODES::DFRFDIRI, QStandardItem *> listModesToParentItem;

  QStandardItem *itemDF, *itemRF, *itemDI, *itemRI;

  itemDF = new QStandardItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF));
  itemRF = new QStandardItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF));
  itemDI = new QStandardItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI));
  itemRI = new QStandardItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI));
  listModesToParentItem.insert(MODES::DFRFDIRI::DF, itemDF);
  listModesToParentItem.insert(MODES::DFRFDIRI::RF, itemRF);
  listModesToParentItem.insert(MODES::DFRFDIRI::DI, itemDI);
  listModesToParentItem.insert(MODES::DFRFDIRI::RI, itemRI);
  model->invisibleRootItem()->appendRows({itemDF, itemRF, itemDI, itemRI});

  QList<PCATABLES> listOfRemainingTablesToShow = {
      PCATABLES::PCAT1, PCATABLES::PCAT2, PCATABLES::PCAT2BIS, PCATABLES::PCAT3, PCATABLES::PCAT3BIS, PCATABLES::PCAT4,
      PCATABLES::PCAT5, PCATABLES::PCAT6, PCATABLES::PCAT7,    PCATABLES::PCAT8, PCATABLES::PCAT9};

  foreach (MODES::DFRFDIRI mode, listModesToParentItem.keys()) {
    foreach (PCATABLES table, listOfRemainingTablesToShow) {
      item = new QStandardItem(pcaTablesDescription.value(table));
      item->setData(static_cast<int>(table), PCx_Tables::TableIdUserRole);
      item->setData(static_cast<int>(mode), PCx_Tables::ModeIdUserRole);
      if (richTooltip) {
        item->setToolTip("<span style='font-size:8pt'>" + getPCAT1(1, mode) + "</span>");
      }
      listModesToParentItem.value(mode)->appendRow(item);
    }
  }
  return model;
}

QStandardItemModel *PCx_Tables::getListModelOfAvailableAuditTables(bool splitDFRFDIRI, bool richTooltip) const {

  QStandardItemModel *model = new QStandardItemModel();
  QStandardItem *item;

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT10));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT10), PCx_Tables::TableIdUserRole);
  if (splitDFRFDIRI) {
    item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  }
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT10(1) + "</span>");
  }
  model->appendRow(item);

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT11));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT11), PCx_Tables::TableIdUserRole);
  if (splitDFRFDIRI) {
    item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  }
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT11(1) + "</span>");
  }
  model->appendRow(item);

  item = new QStandardItem(pcaTablesDescription.value(PCATABLES::PCAT12));
  item->setData(static_cast<int>(PCx_Tables::PCATABLES::PCAT12), PCx_Tables::TableIdUserRole);
  if (splitDFRFDIRI) {
    item->setData(static_cast<int>(MODES::DFRFDIRI::GLOBAL), PCx_Tables::ModeIdUserRole);
  }
  if (richTooltip) {
    item->setToolTip("<span style='font-size:8pt'>" + getPCAT12(1) + "</span>");
  }
  model->appendRow(item);

  QList<PCATABLES> listOfRemainingTablesToShow = {
      PCATABLES::PCAT1, PCATABLES::PCAT2, PCATABLES::PCAT2BIS, PCATABLES::PCAT3, PCATABLES::PCAT3BIS, PCATABLES::PCAT4,
      PCATABLES::PCAT5, PCATABLES::PCAT6, PCATABLES::PCAT7,    PCATABLES::PCAT8, PCATABLES::PCAT9};

  QList<MODES::DFRFDIRI> listModes = {MODES::DFRFDIRI::DF, MODES::DFRFDIRI::RF, MODES::DFRFDIRI::DI,
                                      MODES::DFRFDIRI::RI};

  QMap<MODES::DFRFDIRI, QString> modeToPrefix = {{MODES::DFRFDIRI::DF, QObject::tr("DF - ")},
                                                 {MODES::DFRFDIRI::RF, QObject::tr("RF - ")},
                                                 {MODES::DFRFDIRI::DI, QObject::tr("DI - ")},
                                                 {MODES::DFRFDIRI::RI, QObject::tr("RI - ")}};

  QMap<MODES::DFRFDIRI, QBrush> modeToBackground = {{MODES::DFRFDIRI::DF, QBrush(Qt::GlobalColor::darkGray)},
                                                    {MODES::DFRFDIRI::RF, QBrush(Qt::GlobalColor::gray)},
                                                    {MODES::DFRFDIRI::DI, QBrush(Qt::GlobalColor::darkGray)},
                                                    {MODES::DFRFDIRI::RI, QBrush(Qt::GlobalColor::gray)}};

  foreach (MODES::DFRFDIRI mode, listModes) {
    foreach (PCATABLES table, listOfRemainingTablesToShow) {
      item = new QStandardItem(modeToPrefix.value(mode) + pcaTablesDescription.value(table));
      item->setBackground(modeToBackground.value(mode));
      item->setData(static_cast<int>(table), PCx_Tables::TableIdUserRole);
      item->setData(static_cast<int>(mode), PCx_Tables::ModeIdUserRole);
      if (richTooltip) {
        item->setToolTip("<span style='font-size:8pt'>" + getPCAT1(1, mode) + "</span>");
      }
      model->appendRow(item);
    }
  }
  return model;
}

QString PCx_Tables::getPCAPresetOverview(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  QString out = getPCAT1(node, mode) + "<br>\n" + getPCAT4(node, mode, referenceNode) + "<br>\n" +
                getPCAT8(node, mode) + "<br>\n";
  return out;
}

QString PCx_Tables::getPCAPresetResults(unsigned int node) const {
  QString out = getPCAT10(node) + "<br>\n" + getPCAT11(node) + "<br>\n" + getPCAT12(node) + "<br>\n";
  return out;
}

QString PCx_Tables::getCSS() {
  return QStringLiteral("\ntable{margin-top:2em;color:navy;font-weight:400;font-size:8pt;page-"
                        "break-inside:avoid;}"

                        "\ntable.t1{width:90%;}"
                        "\ntd.t1annee,td.t3annee{background-color:#b3b3b3;}"
                        "\ntd.t1pourcent{background-color:#b3b3b3;color:#FFF;}"
                        "\ntd.t1valeur,td.t2valeur,td.t3valeur,td.t4annee,td.t4valeur,td."
                        "t4pourcent,td.t5annee,td.t7annee{background-color:#e6e6e6;}"
                        "\ntd.t2annee{background-color:#b3b3b3;color:green;}"
                        "\ntd.t2pourcent,td.t3pourcent{background-color:#e6e6e6;color:#000;}"
                        "\ntd.t5pourcent,td.t6pourcent{background-color:#b3b3b3;color:#000;}"
                        "\ntd.t5valeur,td.t6valeur,td.t7valeur{background-color:#b3b3b3;color:#"
                        "000;font-weight:400;}"
                        "\ntd.t6annee{background-color:#e6e6e6;color:green;}"
                        "\ntd.t7pourcent{background-color:#666;color:#FFF;}"
                        "\ntd.t8pourcent{background-color:#e6e6e6;text-align:center;color:#000;}"
                        "\ntd.t8valeur{background-color:#e6e6e6;font-weight:400;text-align:"
                        "center;color:#000;}"
                        "\ntd.t9pourcent{background-color:#666;text-align:center;color:#FFF;}"
                        "\ntd.t9valeur{background-color:#666;color:#FFF;font-weight:400;text-"
                        "align:center;}"
                        "\ntr.t1entete,tr.t3entete,td.t9annee{background-color:#b3b3b3;text-"
                        "align:center;}"
                        "\ntr.t2entete,td.t8annee{background-color:#b3b3b3;color:green;text-"
                        "align:center;}"
                        "\ntr.t4entete,tr.t5entete,tr.t7entete,tr.t9entete{background-color:#"
                        "e6e6e6;text-align:center;}"
                        "\ntr.t6entete{background-color:#e6e6e6;color:green;text-align:center;}"
                        "\ntr.t8entete{background-color:#e6e6e6;text-align:center;color:green;}"
                        "\n");
}

QString PCx_Tables::getPCARawData(unsigned int node, MODES::DFRFDIRI mode) const {
  if (node == 0 || auditModel == nullptr) {
    qFatal("Assertion failed");
  }

  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId());

  QString output =
      QString("\n<table class='tPCARAW' align='center' cellpadding='5'>"
              "<tr class='t1entete'><td align='center' colspan=5><b>%1 (<span "
              "style='color:#7c0000'>%2</span>)</b></td></tr>"
              "<tr class='t1entete'><th>Exercice</th><th>Crédits ouverts</th>"
              "<th>R&eacute;alis&eacute;</th><th>Engag&eacute;</"
              "th><th>Disponible</th></tr>")
          .arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped(), MODES::modeToCompleteString(mode));

  QSqlQuery q;
  q.prepare(QString("select * from %1 where id_node=:id order by annee").arg(tableName));
  q.bindValue(":id", node);
  q.exec();

  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    qint64 ouverts = q.value(QStringLiteral("ouverts")).toLongLong();
    qint64 realises = q.value(QStringLiteral("realises")).toLongLong();
    qint64 engages = q.value(QStringLiteral("engages")).toLongLong();
    qint64 disponibles = q.value(QStringLiteral("disponibles")).toLongLong();

    output.append(QString("<tr><td class='t1annee'>%1</td><td align='right' "
                          "class='t1valeur'>%2</td><td align='right' class='t1valeur'>%3</td>"
                          "<td align='right' class='t1valeur'>%4</td><td align='right' "
                          "class='t1valeur'>%5</td></tr>")
                      .arg(q.value("annee").toUInt())
                      .arg(formatFixedPoint(ouverts), formatFixedPoint(realises), formatFixedPoint(engages),
                           formatFixedPoint(disponibles)));
  }

  output.append(QStringLiteral("</table>"));
  return output;
}

QString PCx_Tables::getPCAPresetEvolution(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  QString out = getPCAT2bis(node, mode, referenceNode) + "<br>\n" + getPCAT3bis(node, mode) + "<br>\n";
  return out;
}

QString PCx_Tables::getPCAPresetEvolutionCumul(unsigned int node, MODES::DFRFDIRI mode,
                                               unsigned int referenceNode) const {
  QString out = getPCAT2(node, mode, referenceNode) + "<br>\n" + getPCAT3(node, mode) + "<br>\n";
  return out;
}

QString PCx_Tables::getPCAPresetBase100(unsigned int node, MODES::DFRFDIRI mode, unsigned int referenceNode) const {
  QString out = getPCAT5(node, mode, referenceNode) + "<br>\n" + getPCAT6(node, mode) + "<br>\n";
  return out;
}

QString PCx_Tables::getPCAPresetDayOfWork(unsigned int node, MODES::DFRFDIRI mode) const {
  QString out = getPCAT7(node, mode) + "<br>\n" + getPCAT9(node, mode) + "<br>\n";
  return out;
}
