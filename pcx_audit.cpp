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

#include "pcx_audit.h"
#include "pcx_prevision.h"
#include "pcx_query.h"
#include "pcx_report.h"
#include "utils.h"
#include "xlsxdocument.h"
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>

using namespace NUMBERSFORMAT;

PCx_Audit::PCx_Audit(unsigned int auditId, bool _noLoadAttachedTree) : auditId(auditId) {
  if (auditId == 0) {
    qFatal("Assertion failed");
  }

  QSqlQuery q;
  if (!q.exec(QString("select * from index_audits where id='%1'").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.next()) {
    auditName = q.value("nom").toString();
    attachedTreeId = q.value("id_arbre").toUInt();

    yearsString = q.value("annees").toString();
    QStringList yearsSplitted = yearsString.split(',');

    QSet<int> yearsTemp;
    foreach (const QString &uneAnnee, yearsSplitted) { yearsTemp.insert(uneAnnee.toInt()); }
    years = yearsTemp.values();
    std::sort(years.begin(), years.end());

    foreach (int annee, years) { yearsStringList.append(QString::number(annee)); }

    yearsString = QString("%1 - %2").arg(years.first()).arg(years.last());

    finished = q.value("termine").toBool();
    if (finished) {
      finishedString = QObject::tr("oui");
    } else {
      finishedString = QObject::tr("non");
    }
    creationTimeUTC = QDateTime::fromString(q.value("le_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
    creationTimeUTC.setTimeSpec(Qt::UTC);
    creationTimeLocal = creationTimeUTC.toLocalTime();
  } else {
    qCritical() << "Invalid audit ID !";
    die();
  }
  if (_noLoadAttachedTree) {
    attachedTree = nullptr;
  } else {
    attachedTree = new PCx_Tree(attachedTreeId);
    attachedTreeName = attachedTree->getName();
  }
}

PCx_Audit::~PCx_Audit() {
  delete attachedTree;
  /*idToChildren.clear();
idToChildrenString.clear();
idToPid.clear();*/
}

QStandardItemModel *PCx_Audit::getListModelOfAuditYears() const {
  QStandardItemModel *model = new QStandardItemModel();
  QStandardItem *item;

  foreach (int year, years) {
    item = new QStandardItem(QString::number(year));
    item->setData(year, Qt::UserRole + 1);
    model->appendRow(item);
  }
  return model;
}

void PCx_Audit::finishAudit() {
  finished = true;
  QSqlQuery q;
  q.prepare("update index_audits set termine=1 where id=:id");
  q.bindValue(":id", auditId);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }
  finishedString = QObject::tr("oui");
}

void PCx_Audit::unFinishAudit() {
  finished = false;
  QSqlQuery q;
  q.prepare("update index_audits set termine=0 where id=:id");
  q.bindValue(":id", auditId);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }
  finishedString = QObject::tr("non");
}

bool PCx_Audit::setLeafValues(unsigned int leafId, MODES::DFRFDIRI mode, int year,
                              const QMap<PCx_Audit::ORED, double> &vals, bool fastMode) {
  if (!fastMode) {
    if (finished) {
      qWarning() << "Will not modify a finished audit";
      return false;
    }
    if (year < years.first() || year > years.last()) {
      qWarning() << "Invalid years !";
      return false;
    }
    if (!getAttachedTree()->isLeaf(leafId)) {
      qWarning() << "Not a leaf !";
      return false;
    }
  }

  QString reqString;

  if (vals.contains(PCx_Audit::ORED::DISPONIBLES)) {
    qDebug() << "DISPONIBLES will be recomputed and overwritten";
  }

  if (vals.contains(PCx_Audit::ORED::OUVERTS)) {
    if (!reqString.isEmpty()) {
      reqString.append(",");
    }
    reqString.append("ouverts=:vouverts");
  }
  if (vals.contains(PCx_Audit::ORED::REALISES)) {
    if (!reqString.isEmpty()) {
      reqString.append(",");
    }
    reqString.append("realises=:vrealises");
  }
  if (vals.contains(PCx_Audit::ORED::ENGAGES)) {
    if (!reqString.isEmpty()) {
      reqString.append(",");
    }
    reqString.append("engages=:vengages");
  }

  QSqlQuery q;
  QString tableName = QString("audit_%1_%2").arg(MODES::modeToTableString(mode)).arg(auditId);

  // qDebug()<<"Request String ="<<reqString;

  q.prepare(QString("update %1 set %2 where id_node=:id_node and annee=:year").arg(tableName, reqString));

  if (vals.contains(PCx_Audit::ORED::OUVERTS)) {
    q.bindValue(":vouverts", doubleToFixedPoint(vals.value(PCx_Audit::ORED::OUVERTS)));
  }

  if (vals.contains(PCx_Audit::ORED::REALISES)) {
    q.bindValue(":vrealises", doubleToFixedPoint(vals.value(PCx_Audit::ORED::REALISES)));
  }

  if (vals.contains(PCx_Audit::ORED::ENGAGES)) {
    q.bindValue(":vengages", doubleToFixedPoint(vals.value(PCx_Audit::ORED::ENGAGES)));
  }

  q.bindValue(":id_node", leafId);
  q.bindValue(":year", year);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }

  q.prepare(QString("update %1 set disponibles=ouverts-(realises+engages) "
                    "where id_node=:id_node and annee=:year")
                .arg(tableName));
  q.bindValue(":id_node", leafId);
  q.bindValue(":year", year);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }

  updateParent(tableName, year, leafId);
  return true;
}

qint64 PCx_Audit::getNodeValue(unsigned int nodeId, MODES::DFRFDIRI mode, PCx_Audit::ORED ored, int year) const {
  QSqlQuery q;
  q.prepare(QString("select %1 from audit_%2_%3 where annee=:year and id_node=:node")
                .arg(OREDtoTableString(ored), modeToTableString(mode))
                .arg(auditId));
  q.bindValue(":year", year);
  q.bindValue(":node", nodeId);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.next()) {
    qWarning() << "No value for node" << nodeId;
    return -MAX_NUM;
  }
  if (q.value(OREDtoTableString(ored)).isNull()) {
    return -MAX_NUM;
  }
  return q.value(OREDtoTableString(ored)).toLongLong();
}

void PCx_Audit::clearAllData(MODES::DFRFDIRI mode) {
  QSqlQuery q(QString("update audit_%1_%2 set "
                      "ouverts=NULL,realises=NULL,engages=NULL,disponibles=NULL")
                  .arg(modeToTableString(mode))
                  .arg(auditId));
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() < 0) {
    qCritical() << q.lastError();
    die();
  }
}

int PCx_Audit::duplicateAudit(const QString &newName, QList<int> years, bool copyDF, bool copyRF, bool copyDI,
                              bool copyRI, bool copyQueries) const {
  if (newName.isEmpty() || newName.size() > MAXOBJECTNAMELENGTH) {
    qFatal("Assertion failed");
  }

  if (PCx_Audit::auditNameExists(newName)) {
    qWarning() << "Audit name already exists !";
    return -1;
  }

  QStringList modes;

  if (copyDF) {
    modes.append(MODES::modeToTableString(MODES::DFRFDIRI::DF));
  }
  if (copyRF) {
    modes.append(MODES::modeToTableString(MODES::DFRFDIRI::RF));
  }
  if (copyDI) {
    modes.append(MODES::modeToTableString(MODES::DFRFDIRI::DI));
  }
  if (copyRI) {
    modes.append(MODES::modeToTableString(MODES::DFRFDIRI::RI));
  }

  QProgressDialog progress(QObject::tr("Données en cours de recopie..."), nullptr, 0, modes.size() * 4 + 1);
  // progress.setMinimumDuration(0);

  progress.setWindowModality(Qt::ApplicationModal);
  // progress.show();

  int progval = 1;
  progress.setValue(0);

  unsigned int newAuditId = PCx_Audit::addNewAudit(newName, years, attachedTreeId);

  if (newAuditId == 0) {
    qFatal("Assertion failed");
  }

  std::sort(years.begin(), years.end());
  progress.setValue(1);
  int year1 = years.first();
  int year2 = years.last();

  QSqlDatabase::database().transaction();

  QSqlQuery q;

  foreach (const QString &lemode, modes) {
    q.prepare(QString("update audit_%3_%1 set ouverts="
                      "(select ouverts from audit_%3_%2 where "
                      "audit_%3_%2.id_node=audit_%3_%1.id_node "
                      "and audit_%3_%2.annee=audit_%3_%1.annee and "
                      "audit_%3_%2.annee>=:year1 "
                      "and audit_%3_%2.annee<=:year2)")
                  .arg(newAuditId)
                  .arg(auditId)
                  .arg(lemode));
    q.bindValue(":year1", year1);
    q.bindValue(":year2", year2);
    if (!q.exec()) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
    if (q.numRowsAffected() < 0) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
    progress.setValue(++progval);

    q.prepare(QString("update audit_%3_%1 set realises="
                      "(select realises from audit_%3_%2 where "
                      "audit_%3_%2.id_node=audit_%3_%1.id_node "
                      "and audit_%3_%2.annee=audit_%3_%1.annee and "
                      "audit_%3_%2.annee>=:year1 "
                      "and audit_%3_%2.annee<=:year2)")
                  .arg(newAuditId)
                  .arg(auditId)
                  .arg(lemode));
    q.bindValue(":year1", year1);
    q.bindValue(":year2", year2);
    if (!q.exec()) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
    if (q.numRowsAffected() < 0) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }

    progress.setValue(++progval);

    q.prepare(QString("update audit_%3_%1 set engages="
                      "(select engages from audit_%3_%2 where "
                      "audit_%3_%2.id_node=audit_%3_%1.id_node "
                      "and audit_%3_%2.annee=audit_%3_%1.annee and "
                      "audit_%3_%2.annee>=:year1 "
                      "and audit_%3_%2.annee<=:year2)")
                  .arg(newAuditId)
                  .arg(auditId)
                  .arg(lemode));
    q.bindValue(":year1", year1);
    q.bindValue(":year2", year2);
    if (!q.exec()) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
    if (q.numRowsAffected() < 0) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }

    progress.setValue(++progval);

    q.prepare(QString("update audit_%3_%1 set disponibles="
                      "(select disponibles from audit_%3_%2 where "
                      "audit_%3_%2.id_node=audit_%3_%1.id_node "
                      "and audit_%3_%2.annee=audit_%3_%1.annee and "
                      "audit_%3_%2.annee>=:year1 "
                      "and audit_%3_%2.annee<=:year2)")
                  .arg(newAuditId)
                  .arg(auditId)
                  .arg(lemode));
    q.bindValue(":year1", year1);
    q.bindValue(":year2", year2);
    if (!q.exec()) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
    if (q.numRowsAffected() < 0) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }

    progress.setValue(++progval);
  }

  progress.setValue(progress.maximum());

  if (copyQueries) {
    q.exec(QString("insert into audit_queries_%1 select * from audit_queries_%2").arg(newAuditId).arg(auditId));
    if (q.numRowsAffected() < 0) {
      QSqlDatabase::database().rollback();
      PCx_Audit::deleteAudit(newAuditId);
      qCritical() << q.lastError();
      die();
    }
  }

  QSqlDatabase::database().commit();
  return static_cast<int>(newAuditId);
}

void PCx_Audit::updateParent(const QString &tableName, int annee, unsigned int nodeId) {
  QString childrenString;
  unsigned int parent;
  parent = idToPid.value(nodeId);
  if (parent == 0) {
    parent = getAttachedTree()->getParentId(nodeId);
    idToPid.insert(nodeId, parent);
  }

  // qDebug()<<"Parent of "<<nodeId<<" = "<<parent;
  QList<unsigned int> listOfChildren;
  if (idToChildren.contains(parent)) {
    listOfChildren = idToChildren.value(parent);
    childrenString = idToChildrenString.value(parent);
  } else {
    listOfChildren = getAttachedTree()->getChildren(parent);
    idToChildren.insert(parent, listOfChildren);
    QStringList l;
    foreach (unsigned int childId, listOfChildren) { l.append(QString::number(childId)); }
    childrenString = l.join(',');
    idToChildrenString.insert(parent, childrenString);
  }

  // qDebug()<<"Children of "<<nodeId<<" = "<<listOfChildren;
  QSqlQuery q;

  q.prepare(QString("select ouverts,realises,engages,disponibles from %1 where "
                    "id_node in(%2) and annee=:annee")
                .arg(tableName, childrenString));
  q.bindValue(":annee", annee);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  qint64 sumOuverts = 0;
  qint64 sumRealises = 0;
  qint64 sumEngages = 0;
  qint64 sumDisponibles = 0;

  // To check if we need to insert 0.0 or NULL
  bool nullOuverts = true;
  bool nullEngages = true;
  bool nullRealises = true;
  bool nullDisponibles = true;

  while (q.next()) {
    // qDebug()<<"Node ID = "<<q.value("id_node")<< "Ouverts =
    // "<<q.value("ouverts")<<" Realises = "<<q.value("realises")<<" Engages =
    // "<<q.value("engages")<<" Disponibles = "<<q.value("disponibles"); Uses
    // index to speedup
    if (!q.value(0).isNull()) {
      sumOuverts += q.value(0).toLongLong();
      nullOuverts = false;
    }

    if (!q.value(1).isNull()) {
      sumRealises += q.value(1).toLongLong();
      nullRealises = false;
    }
    if (!q.value(2).isNull()) {
      sumEngages += q.value(2).toLongLong();
      nullEngages = false;
    }
    if (!q.value(3).isNull()) {
      sumDisponibles += q.value(3).toLongLong();
      nullDisponibles = false;
    }
  }

  q.prepare(QString("update %1 set "
                    "ouverts=:ouverts,realises=:realises,engages=:engages,disponibles=:"
                    "disponibles where annee=:annee and id_node=:id_node")
                .arg(tableName));
  if (nullOuverts) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    q.bindValue(":ouverts", QVariant(QMetaType::fromType<qlonglong>()));
#else
    q.bindValue(":ouverts", QVariant(QVariant::LongLong));
#endif
  } else {
    q.bindValue(":ouverts", sumOuverts);
  }

  if (nullRealises) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    q.bindValue(":realises", QVariant(QMetaType::fromType<qlonglong>()));
#else
    q.bindValue(":realises", QVariant(QVariant::LongLong));
#endif
  } else {
    q.bindValue(":realises", sumRealises);
  }

  if (nullEngages) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    q.bindValue(":engages", QVariant(QMetaType::fromType<qlonglong>()));
#else
    q.bindValue(":engages", QVariant(QVariant::LongLong));
#endif

  } else {
    q.bindValue(":engages", sumEngages);
  }

  if (nullDisponibles) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    q.bindValue(":disponibles", QVariant(QMetaType::fromType<qlonglong>()));
#else
    q.bindValue(":disponibles", QVariant(QVariant::LongLong));
#endif

  } else {
    q.bindValue(":disponibles", sumDisponibles);
  }

  q.bindValue(":annee", annee);
  q.bindValue(":id_node", parent);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() <= 0) {
    qCritical() << q.lastError();
    die();
  }

  if (parent > 1) {
    updateParent(tableName, annee, parent);
  }
}

QString PCx_Audit::getHTMLAuditStatistics() const {
  QLocale defaultLocale;
  QString out = QString("\n<table cellpadding='5' border='1' align='center'>\n"
                        "<tr><td>Arbre associé</td><td align='right'>%1 (%2 noeuds)</td></tr>"
                        "<tr><td>Années d'application</td><td align='right'>%3</td></tr>"
                        "<tr><td>Date de création</td><td align='right'>%4</td></tr>"
                        "<tr><td>Audit terminé</td><td align='right'>%5</td></tr>"
                        "</table>\n")
                    .arg(attachedTreeName.toHtmlEscaped())
                    .arg(getAttachedTree()->getNumberOfNodes())
                    .arg(yearsString, defaultLocale.toString(creationTimeLocal, QLocale::LongFormat).toHtmlEscaped(),
                         finishedString);

  out.append("\n<br><table cellpadding='5' border='1' align='center'>\n"
             "<tr><th>&nbsp;</th><th>DF</th><th>RF</th><th>DI</th><th>RI</th></tr>");

  PCx_Tree tree(attachedTreeId);

  QList<QList<unsigned int> *> listOfListOfNodes;
  QList<unsigned int> nodesDF, nodesRF, nodesDI, nodesRI;
  listOfListOfNodes.append(&nodesDF);
  listOfListOfNodes.append(&nodesRF);
  listOfListOfNodes.append(&nodesDI);
  listOfListOfNodes.append(&nodesRI);

  foreach (int year, years) {
    nodesDF = getNodesWithNonNullValues(MODES::DFRFDIRI::DF, year);
    nodesRF = getNodesWithNonNullValues(MODES::DFRFDIRI::RF, year);
    nodesDI = getNodesWithNonNullValues(MODES::DFRFDIRI::DI, year);
    nodesRI = getNodesWithNonNullValues(MODES::DFRFDIRI::RI, year);
    out.append(QString("\n<tr><th colspan='5'>%1</th></tr>\n"
                       "<tr><td>Noeuds contenant au moins une valeur (même zéro)</td><td "
                       "align='right'>%2</td><td align='right'>%3</td><td  "
                       "align='right'>%4</td><td  align='right'>%5</td></tr>")
                   .arg(year)
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

    nodesDF = getNodesWithAllZeroValues(MODES::DFRFDIRI::DF, year);
    nodesRF = getNodesWithAllZeroValues(MODES::DFRFDIRI::RF, year);
    nodesDI = getNodesWithAllZeroValues(MODES::DFRFDIRI::DI, year);
    nodesRI = getNodesWithAllZeroValues(MODES::DFRFDIRI::RI, year);

    out.append(QString("<tr><td>Noeuds dont les valeurs sont toutes à zéro</td><td "
                       "align='right'>%1</td><td align='right'>%2</td><td "
                       "align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));

    if (!nodesDF.isEmpty() || !nodesRF.isEmpty() || !nodesDI.isEmpty() || !nodesRI.isEmpty()) {
      out.append("<tr><td></td>");

      foreach (QList<unsigned int> *listOfNodes, listOfListOfNodes) {
        out.append("<td>");
        if (!listOfNodes->isEmpty()) {
          out.append("<ul>");

          foreach (unsigned int node, *listOfNodes) {
            out.append(QString("<li>%1</li>").arg(tree.getNodeName(node).toHtmlEscaped()));
          }
          out.append("</ul>");
        }
        out.append("</td>");
      }
      out.append("</tr>");
    }

    nodesDF = getNodesWithAllNullValues(MODES::DFRFDIRI::DF, year);
    nodesRF = getNodesWithAllNullValues(MODES::DFRFDIRI::RF, year);
    nodesDI = getNodesWithAllNullValues(MODES::DFRFDIRI::DI, year);
    nodesRI = getNodesWithAllNullValues(MODES::DFRFDIRI::RI, year);

    out.append(QString("<tr><td>Noeuds non remplis</td><td "
                       "align='right'>%1</td><td align='right'>%2</td><td "
                       "align='right'>%3</td><td align='right'>%4</td></tr>")
                   .arg(nodesDF.size())
                   .arg(nodesRF.size())
                   .arg(nodesDI.size())
                   .arg(nodesRI.size()));
  }

  out.append("</table>\n");

  return out;
}

QList<unsigned int> PCx_Audit::getNodesWithAllNullValues(MODES::DFRFDIRI mode, int year) const {
  QString tableMode = MODES::modeToTableString(mode);
  if (!(year >= years.first() && year <= years.last())) {
    qFatal("Assertion failed");
  }
  QList<unsigned int> nodes;
  QStringList oredStrings;
  oredStrings << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
  QSqlQuery q;
  q.prepare(QString("select id_node from audit_%1_%2 where (%3 is null and %4 "
                    "is null and %5 is null) and annee=:year")
                .arg(tableMode)
                .arg(auditId)
                .arg(oredStrings.at(0), oredStrings.at(1), oredStrings.at(2)));

  q.bindValue(":year", year);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    nodes.append(q.value("id_node").toUInt());
  }
  return nodes;
}

QList<unsigned int> PCx_Audit::getNodesWithNonNullValues(MODES::DFRFDIRI mode, int year) const {
  QString tableMode = MODES::modeToTableString(mode);
  if (!(year >= years.first() && year <= years.last())) {
    qFatal("Assertion failed");
  }
  QList<unsigned int> nodes;
  QStringList oredStrings;
  oredStrings << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
  QSqlQuery q;
  q.prepare(QString("select id_node from audit_%1_%2 where (%3 not null or %4 "
                    "not null or %5 not null) and annee=:year")
                .arg(tableMode)
                .arg(auditId)
                .arg(oredStrings.at(0), oredStrings.at(1), oredStrings.at(2)));

  q.bindValue(":year", year);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    nodes.append(q.value("id_node").toUInt());
  }
  return nodes;
}

QList<unsigned int> PCx_Audit::getNodesWithAllZeroValues(MODES::DFRFDIRI mode, int year) const {
  QString tableMode = MODES::modeToTableString(mode);
  if (!(year >= years.first() && year <= years.last())) {
    qFatal("Assertion failed");
  }
  QList<unsigned int> nodes;
  QStringList oredStrings;
  oredStrings << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES)
              << PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES);
  QSqlQuery q;
  QString sq = QString("select id_node from audit_%1_%2 where (%3 = 0 and %4 = 0 and %5 "
                       "= 0) and annee=:year")
                   .arg(tableMode)
                   .arg(auditId)
                   .arg(oredStrings.at(0), oredStrings.at(1), oredStrings.at(2));

  q.prepare(sq);
  q.bindValue(":year", year);

  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  while (q.next()) {
    nodes.append(q.value("id_node").toUInt());
  }
  return nodes;
}

bool PCx_Audit::importDataFromXLSX(const QString &fileName, MODES::DFRFDIRI mode) {
  if (fileName.isEmpty()) {
    qFatal("Assertion failed");
  }
  // QElapsedTimer timer;
  // timer.start();

  QFileInfo fi(fileName);
  if (!fi.isReadable() || !fi.isFile()) {
    QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Fichier invalide ou non lisible"));
    return false;
  }

  QXlsx::Document xlsx(fileName);

  if (!(xlsx.read(1, 1).isValid() && xlsx.read(1, 2).isValid() && xlsx.read(1, 3).isValid() &&
        xlsx.read(1, 4).isValid() && xlsx.read(1, 5).isValid() && xlsx.read(1, 6).isValid() &&
        xlsx.read(1, 7).isValid())) {
    QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                          QObject::tr("Format de fichier invalide. Vous pouvez exporter un "
                                      "fichier pré-rempli à l'aide du bouton exporter dans la "
                                      "fenêtre de saisie des données."));
    return false;
  }

  int rowCount = xlsx.dimension().rowCount();

  int row = 2;
  if (rowCount < 2) {
    QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                          QObject::tr("Fichier vide. Vous pouvez exporter un fichier pré-rempli à l'aide "
                                      "du bouton exporter dans la fenêtre de saisie des données."));
    return false;
  }

  QProgressDialog progress(QObject::tr("Vérification en cours..."), QObject::tr("Annuler"), 2, rowCount);
  progress.setMinimumDuration(200);

  progress.setWindowModality(Qt::ApplicationModal);

  progress.setValue(2);

  do {
    QVariant cellNodeId, nodeType, nodeName, year, ouverts, realises, engages;
    cellNodeId = xlsx.read(row, 1);
    nodeType = xlsx.read(row, 2);
    nodeName = xlsx.read(row, 3);
    year = xlsx.read(row, 4);
    ouverts = xlsx.read(row, 5);
    realises = xlsx.read(row, 6);
    engages = xlsx.read(row, 7);

    if (!(cellNodeId.isValid() && nodeType.isValid() && nodeName.isValid() && year.isValid())) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Erreur de format ligne %1, remplissez l'identifiant, le "
                                        "type et le nom du noeud ainsi que l'année d'application")
                                .arg(row));
      return false;
    }

    QPair<QString, QString> typeAndNode;
    typeAndNode.first = nodeType.toString().simplified();
    typeAndNode.second = nodeName.toString().simplified();
    if (typeAndNode.first.isEmpty() || typeAndNode.second.isEmpty()) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Erreur de format ligne %1, le type et le nom du noeud ne "
                                        "peuvent pas être vides (ni composés uniquement d'espaces)")
                                .arg(row));
      return false;
    }

    if (typeAndNode.first.size() > PCx_Tree::MAXNODENAMELENGTH ||
        typeAndNode.second.size() > PCx_Tree::MAXNODENAMELENGTH) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Erreur de format ligne %1, le type et "
                                        "le nom du noeud sont trop longs")
                                .arg(row));
      return false;
    }

    if (!years.contains(year.toInt())) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("L'année ligne %1 n'est pas valable pour cet audit !").arg(row));
      return false;
    }
    double dblOuv = ouverts.toDouble();
    double dblReal = realises.toDouble();
    double dblEng = engages.toDouble();

    if (dblOuv > MAX_NUM || dblReal > MAX_NUM || dblEng > MAX_NUM) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Valeur trop grande ligne %1 (valeur maximale : %2) !").arg(row).arg(MAX_NUM));
      return false;
    }

    if (!qIsFinite(dblOuv) || !qIsFinite(dblReal) || !qIsFinite(dblEng)) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Valeur infinie ligne %1 non autorisée !").arg(row));
      return false;
    }

    if (dblOuv < 0.0 || dblReal < 0.0 || dblEng < 0.0) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Valeur négative ligne %1 non autorisée !").arg(row));
      return false;
    }

    unsigned int nodeId = cellNodeId.toUInt();
    if (nodeId == 0) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("L'identifiant du noeud ligne %1 est invalide").arg(row));
      return false;
    }
    // nodeId=getAttachedTree()->getNodeIdFromTypeAndNodeName(typeAndNode);
    if (!getAttachedTree()->checkIdToTypeAndName(nodeId, typeAndNode.first, typeAndNode.second)) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("L'identifiant du noeud ligne %1 ne correspond pas aux "
                                        "type et nom indiqués sur la même ligne")
                                .arg(row));
      return false;
    }

    bool leaf = getAttachedTree()->isLeaf(nodeId);

    if (!leaf) {
      QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                            QObject::tr("Le noeud ligne %1 n'est pas une feuille ! Remplissez les "
                                        "données pour les feuilles seulement, en utilisant un fichier "
                                        "modèle obtenu à l'aide du bouton d'exportation")
                                .arg(row));
      return false;
    }
    row++;

    if (!progress.wasCanceled()) {
      progress.setValue(row);
    } else {
      return false;
    }

  } while (row <= rowCount);

  row = 2;

  QSqlDatabase::database().transaction();

  this->clearAllData(mode);

  // QCoreApplication::processEvents(QEventLoop::AllEvents);

  QProgressDialog progress2(QObject::tr("Chargement des données en cours..."), QObject::tr("Annuler"), 2, rowCount);

  progress2.setMinimumDuration(200);

  progress2.setWindowModality(Qt::ApplicationModal);

  progress2.setValue(2);

  do {
    QVariant cellNodeId, nodeType, nodeName, year, ouverts, realises, engages;
    cellNodeId = xlsx.read(row, 1);
    nodeType = xlsx.read(row, 2);
    nodeName = xlsx.read(row, 3);
    year = xlsx.read(row, 4);
    ouverts = xlsx.read(row, 5);
    realises = xlsx.read(row, 6);
    engages = xlsx.read(row, 7);

    // Here year and node are found and correct
    QPair<QString, QString> typeAndNode;
    typeAndNode.first = nodeType.toString().simplified();
    typeAndNode.second = nodeName.toString().simplified();
    unsigned int nodeId;
    nodeId = cellNodeId.toUInt();

    QMap<PCx_Audit::ORED, double> vals;
    if (ouverts.isValid()) {
      double valDbl = ouverts.toDouble();
      vals.insert(PCx_Audit::ORED::OUVERTS, valDbl);
    }
    if (realises.isValid()) {
      double valDbl = realises.toDouble();
      vals.insert(PCx_Audit::ORED::REALISES, valDbl);
    }
    if (engages.isValid()) {
      double valDbl = engages.toDouble();
      vals.insert(PCx_Audit::ORED::ENGAGES, valDbl);
    }
    if (!vals.empty()) {
      // setLeafValues in fast mode only die on db error
      setLeafValues(nodeId, mode, year.toInt(), vals, true);
    }
    vals.clear();

    row++;
    if (!progress2.wasCanceled()) {
      progress2.setValue(row);
    } else {
      QSqlDatabase::database().rollback();
      return false;
    }

  } while (row <= rowCount);

  QSqlDatabase::database().commit();
  // qDebug()<<"Import done in "<<timer.elapsed()<<" ms";
  return true;
}

bool PCx_Audit::exportLeavesDataXLSX(MODES::DFRFDIRI mode, const QString &fileName) const {
  QXlsx::Document xlsx;
  QList<unsigned int> leavesId = getAttachedTree()->getLeavesId();
  xlsx.write(1, 1, "Identifiant noeud");
  xlsx.write(1, 2, "Type noeud");
  xlsx.write(1, 3, "Nom noeud");
  xlsx.write(1, 4, "Année");
  xlsx.write(1, 5, PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS, true));
  xlsx.write(1, 6, PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES, true));
  xlsx.write(1, 7, PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES, true));

  QSqlQuery q;
  int row = 2;
  foreach (unsigned int leafId, leavesId) {
    QPair<QString, QString> typeAndNodeName = getAttachedTree()->getTypeNameAndNodeName(leafId);
    q.prepare(QString("select * from audit_%1_%2 where id_node=:idnode order by annee")
                  .arg(MODES::modeToTableString(mode))
                  .arg(auditId));

    q.bindValue(":idnode", leafId);
    if (!q.exec()) {
      qCritical() << q.lastError();
      die();
    }
    while (q.next()) {
      QVariant valOuverts = q.value("ouverts");
      QVariant valRealises = q.value("realises");
      QVariant valEngages = q.value("engages");

      xlsx.write(row, 1, leafId);
      xlsx.write(row, 2, typeAndNodeName.first);
      xlsx.write(row, 3, typeAndNodeName.second);
      xlsx.write(row, 4, q.value("annee").toUInt());

      if (!valOuverts.isNull()) {
        // QString
        // vOuverts=formatDouble((double)valOuverts.toLongLong()/FIXEDPOINTCOEFF,2,true);
        xlsx.write(row, 5, fixedPointToDouble(valOuverts.toLongLong()));
      }

      if (!valRealises.isNull()) {
        xlsx.write(row, 6, fixedPointToDouble(valRealises.toLongLong()));
      }

      if (!valEngages.isNull()) {
        xlsx.write(row, 7, fixedPointToDouble(valEngages.toLongLong()));
      }
      row++;
    }
  }

  return xlsx.saveAs(fileName);
}

void PCx_Audit::fillWithRandomData(MODES::DFRFDIRI mode)
{
  QList<unsigned int> leaves = getAttachedTree()->getLeavesId();

  QMap<PCx_Audit::ORED, double> data;

  QSqlDatabase::database().transaction();

  clearAllData(mode);

  QRandomGenerator *randomGenerator = QRandomGenerator::global();

  int randValOuverts, randValRealises, randValEngages;

  foreach (unsigned int leaf, leaves) {
    int lastOuverts = -1;
    foreach (int year, years) {
      data.clear();

      if (lastOuverts == -1) {
        randValOuverts = randomGenerator->bounded(100, MAX_RANDOM_NUM_FOR_A_LEAF / 2);
        lastOuverts = randValOuverts;
      } else {
        randValOuverts = randomGenerator->bounded(lastOuverts / 2, lastOuverts * 2);
      }
      data.insert(PCx_Audit::ORED::OUVERTS, randValOuverts);

      randValRealises = randomGenerator->bounded(randValOuverts + 1);
      data.insert(PCx_Audit::ORED::REALISES, randValRealises);

      randValEngages = randomGenerator->bounded(randValOuverts - randValRealises + 1);
      data.insert(PCx_Audit::ORED::ENGAGES, randValEngages);

      // the transaction will be rollback in setLeafValues=>die
      setLeafValues(leaf, mode, year, data, true);
    }
  }
  QSqlDatabase::database().commit();
}

QString PCx_Audit::generateHTMLAuditTitle() const {
  return QString("<h3>Audit %1 (%2), arbre %3</h3>")
      .arg(auditName.toHtmlEscaped(), yearsString, attachedTreeName.toHtmlEscaped());
}

QString PCx_Audit::OREDtoCompleteString(ORED ored, bool plural) {

  if (!plural) {
    switch (ored) {
    case ORED::OUVERTS:
      return QObject::tr("crédits ouverts");
    case ORED::REALISES:
      return QObject::tr("réalisé");
    case ORED::ENGAGES:
      return QObject::tr("engagé");
    case ORED::DISPONIBLES:
      return QObject::tr("disponible");
    case ORED::NONELAST:
      qWarning() << "Invalid ORED specified !";
      return {};
    }
  } else {
    switch (ored) {
    case ORED::OUVERTS:
      return QObject::tr("crédits ouverts");
    case ORED::REALISES:
      return QObject::tr("réalisés");
    case ORED::ENGAGES:
      return QObject::tr("engagés");
    case ORED::DISPONIBLES:
      return QObject::tr("disponibles");
    case ORED::NONELAST:
      qWarning() << "Invalid ORED specified !";
      return {};
    }
  }
  return {};
}

QString PCx_Audit::OREDtoTableString(ORED ored) {
  switch (ored) {
  case ORED::OUVERTS:
    return "ouverts";
  case ORED::REALISES:
    return "realises";
  case ORED::ENGAGES:
    return "engages";
  case ORED::DISPONIBLES:
    return "disponibles";
  case ORED::NONELAST:
    qWarning() << "Invalid ORED specified !";
  }
  return {};
}

PCx_Audit::ORED PCx_Audit::OREDFromTableString(const QString &oredString) {
  if (oredString == OREDtoTableString(ORED::OUVERTS)) {
    return ORED::OUVERTS;
  }
  if (oredString == OREDtoTableString(ORED::REALISES)) {
    return ORED::REALISES;
  }
  if (oredString == OREDtoTableString(ORED::ENGAGES)) {
    return ORED::ENGAGES;
  }
  if (oredString == OREDtoTableString(ORED::DISPONIBLES)) {
    return ORED::DISPONIBLES;
  }

  qWarning() << "Invalid ORED string !";
  return ORED::OUVERTS;
}

unsigned int PCx_Audit::addNewAudit(const QString &name, const QList<int> &years, unsigned int attachedTreeId) {
  if (years.isEmpty() || name.isEmpty() || attachedTreeId == 0 || name.size() > MAXOBJECTNAMELENGTH) {
    qFatal("Assertion failed");
  }

  // Removes duplicates and sort years
  QSet<int> yearsSet(years.begin(), years.end());
  QList<int> yearsList = yearsSet.values();
  std::sort(yearsList.begin(), yearsList.end());

  QString yearsString;
  foreach (int annee, yearsList) {
    yearsString.append(QString::number(annee));
    yearsString.append(',');
  }
  yearsString.chop(1);

  // qDebug()<<"years string = "<<yearsString;

  QList<unsigned int> treeIds = PCx_Tree::getListOfTreesId(true);
  if (!treeIds.contains(attachedTreeId)) {
    QMessageBox::warning(nullptr, QObject::tr("Attention"),
                         QObject::tr("Impossible de construire un audit sur un "
                                     "arbre non existant ou non terminé"));
    return 0;
  }

  if (auditNameExists(name)) {
    QMessageBox::warning(nullptr, QObject::tr("Attention"),
                         QObject::tr("Il existe déjà un audit portant ce nom ! "
                                     "Choisissez un autre nom"));
    return 0;
  }

  QSqlQuery q;

  if (!q.exec(QString("select count(*) from index_arbres where id='%1'").arg(attachedTreeId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next()) {
    if (q.value(0).toInt() == 0) {
      qCritical() << "Trying to create an audit on a non-existant tree !";
      die();
    }
  }

  QSqlDatabase::database().transaction();
  q.prepare("insert into index_audits (nom,id_arbre,annees) values "
            "(:nom,:id_arbre,:annees)");
  q.bindValue(":nom", name);
  q.bindValue(":id_arbre", attachedTreeId);
  q.bindValue(":annees", yearsString);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }

  QVariant lastId = q.lastInsertId();
  if (!lastId.isValid()) {
    qCritical() << q.lastError();
    die();
  }
  unsigned int uLastId = lastId.toUInt();

  // Data are integer for fixed points arithmetics, stored with 3 decimals
  if (!q.exec(QString("create table audit_DF_%1(id integer primary key autoincrement, "
                      "id_node integer not null references arbre_%2(id) on delete "
                      "restrict, "
                      "annee integer not null, ouverts integer, realises integer, "
                      "engages integer, "
                      "disponibles integer)")
                  .arg(uLastId)
                  .arg(attachedTreeId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create index idx_idnode_audit_DF_%1 on audit_DF_%1(id_node)").arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("create table audit_RF_%1(id integer primary key autoincrement, "
                      "id_node integer not null references arbre_%2(id) on delete "
                      "restrict, "
                      "annee integer not null, ouverts integer, realises integer, "
                      "engages integer, "
                      "disponibles integer)")
                  .arg(uLastId)
                  .arg(attachedTreeId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create index idx_idnode_audit_RF_%1 on audit_RF_%1(id_node)").arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create table audit_DI_%1(id integer primary key autoincrement, "
                      "id_node integer not null references arbre_%2(id) on delete "
                      "restrict, "
                      "annee integer not null, ouverts integer, realises integer, "
                      "engages integer, "
                      "disponibles integer)")
                  .arg(uLastId)
                  .arg(attachedTreeId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("create index idx_idnode_audit_DI_%1 on audit_DI_%1(id_node)").arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create table audit_RI_%1(id integer primary key autoincrement, "
                      "id_node integer not null references arbre_%2(id) on delete "
                      "restrict, "
                      "annee integer not null, ouverts integer, realises integer, "
                      "engages integer, "
                      "disponibles integer)")
                  .arg(uLastId)
                  .arg(attachedTreeId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("create index idx_idnode_audit_RI_%1 on audit_RI_%1(id_node)").arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  PCx_Query::createTableQueries(uLastId);

  // Populate tables with years for each node of the attached tree

  PCx_Tree tree(attachedTreeId);
  QList<unsigned int> nodes = tree.getNodesId();
  // qDebug()<<"Nodes ids = "<<nodes;

  foreach (unsigned int node, nodes) {
    foreach (int annee, yearsList) {
      q.prepare(QString("insert into audit_DF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
      q.bindValue(":idnode", node);
      q.bindValue(":annee", annee);
      if (!q.exec()) {
        qCritical() << q.lastError();
        die();
      }
      if (q.numRowsAffected() == -1) {
        qCritical() << q.lastError();
        die();
      }

      q.prepare(QString("insert into audit_RF_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
      q.bindValue(":idnode", node);
      q.bindValue(":annee", annee);
      if (!q.exec()) {
        qCritical() << q.lastError();
        die();
      }
      if (q.numRowsAffected() == -1) {
        qCritical() << q.lastError();
        die();
      }

      q.prepare(QString("insert into audit_DI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
      q.bindValue(":idnode", node);
      q.bindValue(":annee", annee);
      if (!q.exec()) {
        qCritical() << q.lastError();
        die();
      }
      if (q.numRowsAffected() == -1) {
        qCritical() << q.lastError();
        die();
      }

      q.prepare(QString("insert into audit_RI_%1(id_node,annee) values (:idnode,:annee)").arg(uLastId));
      q.bindValue(":idnode", node);
      q.bindValue(":annee", annee);
      if (!q.exec()) {
        qCritical() << q.lastError();
        die();
      }
      if (q.numRowsAffected() == -1) {
        qCritical() << q.lastError();
        die();
      }
    }
  }
  QSqlDatabase::database().commit();

  return uLastId;
}

bool PCx_Audit::deleteAudit(unsigned int auditId) {
  if (auditId == 0) {
    qFatal("Assertion failed");
  }
  QSqlQuery q(QString("select count(*) from index_audits where id='%1'").arg(auditId));
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next()) {
    if (q.value(0).toInt() == 0) {
      qWarning() << "Attempting to delete an inexistant audit !";
      return false;
    }
  }

  if (!q.exec(QString("select count(*) from index_previsions where id_audit='%1'").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next()) {
    if (q.value(0).toInt() > 0) {
      QMessageBox::warning(nullptr, QObject::tr("Attention"),
                           QObject::tr("Il existe des prévisions attachées à "
                                       "cet audit, supprimez-les d'abord"));
      return false;
    }
  }

  QSqlDatabase::database().transaction();
  if (!q.exec(QString("delete from index_audits where id='%1'").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table audit_DF_%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table audit_RF_%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table audit_DI_%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table audit_RI_%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("drop table audit_queries_%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  QSqlDatabase::database().commit();
  return true;
}

bool PCx_Audit::auditNameExists(const QString &auditName) {
  QSqlQuery q;
  q.prepare("select count(*) from index_audits where nom=:name");
  q.bindValue(":name", auditName);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.next()) {
    qCritical() << q.lastError();
    die();
  }

  return q.value(0).toLongLong() > 0;
}

bool PCx_Audit::auditIdExists(unsigned int auditId) {
  QSqlQuery q;
  if (!q.exec(QString("select count(*) from index_audits where id=%1").arg(auditId))) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.next()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.value(0).toInt() == 0) {
    return false;
  }
  return true;
}

QList<QPair<unsigned int, QString>> PCx_Audit::getListOfAudits(ListAuditsMode mode) {
  QList<QPair<unsigned int, QString>> listOfAudits;
  QDateTime dt;

  QSqlQuery query;
  if (!query.exec("select * from index_audits order by datetime(le_timestamp)")) {
    qCritical() << query.lastError();
    die();
  }

  while (query.next()) {
    QString item;
    dt = QDateTime::fromString(query.value("le_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
    dt.setTimeSpec(Qt::UTC);
    QDateTime dtLocal = dt.toLocalTime();
    QLocale defaultLocale;
    QPair<unsigned int, QString> p;
    if (query.value("termine").toBool()) {
      // Finished audit
      item = QString("%1 - %2 (audit terminé)")
                 .arg(query.value("nom").toString(), defaultLocale.toString(dtLocal, QLocale::ShortFormat));
      if (mode != PCx_Audit::ListAuditsMode::UnFinishedAuditsOnly) {
        p.first = query.value("id").toUInt();
        p.second = item;
        listOfAudits.append(p);
      }
    } else if (mode != PCx_Audit::ListAuditsMode::FinishedAuditsOnly) {
      // Unfinished audit
      item =
          QString("%1 - %2").arg(query.value("nom").toString(), defaultLocale.toString(dtLocal, QLocale::ShortFormat));
      p.first = query.value("id").toUInt();
      p.second = item;
      listOfAudits.append(p);
    }
  }
  return listOfAudits;
}

QList<QPair<unsigned int, QString>> PCx_Audit::getListOfAuditsAttachedWithThisTree(unsigned int treeId,
                                                                                   PCx_Audit::ListAuditsMode mode) {
  if (treeId == 0) {
    qFatal("Assertion failed");
  }
  QList<QPair<unsigned int, QString>> listOfAudits;
  QDateTime dt;

  QSqlQuery query;
  query.prepare("select * from index_audits where id_arbre=:idarbre order by "
                "datetime(le_timestamp)");
  query.bindValue(":idarbre", treeId);
  if (!query.exec()) {
    qCritical() << query.lastError();
    die();
  }

  while (query.next()) {
    QString item;
    dt = QDateTime::fromString(query.value("le_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
    dt.setTimeSpec(Qt::UTC);
    QDateTime dtLocal = dt.toLocalTime();
    QLocale defaultLocale;
    QPair<unsigned int, QString> p;
    if (query.value("termine").toBool()) {
      // Finished audit
      item = QString("%1 - %2 (audit terminé)")
                 .arg(query.value("nom").toString(), defaultLocale.toString(dtLocal, QLocale::ShortFormat));
      if (mode != PCx_Audit::ListAuditsMode::UnFinishedAuditsOnly) {
        p.first = query.value("id").toUInt();
        p.second = item;
        listOfAudits.append(p);
      }
    } else if (mode != PCx_Audit::ListAuditsMode::FinishedAuditsOnly) {
      // Unfinished audit
      item =
          QString("%1 - %2").arg(query.value("nom").toString(), defaultLocale.toString(dtLocal, QLocale::ShortFormat));
      p.first = query.value("id").toUInt();
      p.second = item;
      listOfAudits.append(p);
    }
  }
  return listOfAudits;
}

bool PCx_Audit::exportLeavesSkeleton(const QString &fileName) const {
  QXlsx::Document xlsx;
  QList<unsigned int> leavesId = getAttachedTree()->getLeavesId();
  xlsx.write(1, 1, "Identifiant noeud");
  xlsx.write(1, 2, "Type noeud");
  xlsx.write(1, 3, "Nom noeud");
  xlsx.write(1, 4, "Année");
  xlsx.write(1, 5, PCx_Audit::OREDtoCompleteString(ORED::OUVERTS, true));
  xlsx.write(1, 6, PCx_Audit::OREDtoCompleteString(ORED::REALISES, true));
  xlsx.write(1, 7, PCx_Audit::OREDtoCompleteString(ORED::ENGAGES, true));

  int row = 2;
  foreach (int year, years) {
    foreach (unsigned int leaf, leavesId) {
      QPair<QString, QString> typeAndNodeName = getAttachedTree()->getTypeNameAndNodeName(leaf);
      xlsx.write(row, 1, leaf);
      xlsx.write(row, 2, typeAndNodeName.first);
      xlsx.write(row, 3, typeAndNodeName.second);
      xlsx.write(row, 4, year);
      xlsx.write(row, 5, 0);
      xlsx.write(row, 6, 0);
      xlsx.write(row, 7, 0);
      row++;
    }
  }

  return xlsx.saveAs(fileName);
}
