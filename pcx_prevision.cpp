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

#include "pcx_prevision.h"
#include "pcx_previsionitem.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

PCx_Prevision::PCx_Prevision(unsigned int previsionId) : previsionId(previsionId) {
  QSqlQuery q;
  attachedAudit = nullptr;
  if (!q.exec(QString("select * from index_previsions where id='%1'").arg(previsionId))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.next()) {
    previsionName = q.value("nom").toString();
    attachedAuditId = q.value("id_audit").toUInt();
    attachedAudit = new PCx_Audit(attachedAuditId);
    attachedTree = attachedAudit->getAttachedTree();
    attachedTreeId = attachedAudit->getAttachedTreeId();
    creationTimeUTC = QDateTime::fromString(q.value("le_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
    creationTimeUTC.setTimeSpec(Qt::UTC);
    creationTimeLocal = creationTimeUTC.toLocalTime();
  } else {
    qCritical() << "Invalid prevision ID !";
    die();
  }
}

int PCx_Prevision::toPrevisionalExtendedAudit(const QString &newAuditName) {
  if (PCx_Audit::auditNameExists(newAuditName)) {
    qWarning() << QObject::tr("Il existe déjà un audit portant ce nom");
    return -1;
  }
  QList<int> years = attachedAudit->getYears();
  qSort(years);
  int lastYear = years.last() + 1;
  years.append(lastYear);

  int res = attachedAudit->duplicateAudit(newAuditName, years, true, true, true, true);
  if (res < 0) {
    return res;
  }

  PCx_Audit dupAudit(static_cast<unsigned int>(res));

  QList<unsigned int> leaves = attachedTree->getLeavesId();
  QList<MODES::DFRFDIRI> modes{MODES::DFRFDIRI::DF, MODES::DFRFDIRI::RF, MODES::DFRFDIRI::DI, MODES::DFRFDIRI::RI};
  QMap<PCx_Audit::ORED, double> values;
  QSqlDatabase::database().transaction();
  foreach (unsigned int leaf, leaves) {
    foreach (MODES::DFRFDIRI unMode, modes) {
      PCx_PrevisionItem prevItem(this, unMode, leaf, lastYear);
      prevItem.loadFromDb();
      values.clear();
      values.insert(PCx_Audit::ORED::OUVERTS,
                    NUMBERSFORMAT::fixedPointToDouble(prevItem.getSummedPrevisionItemValue()));
      dupAudit.setLeafValues(leaf, unMode, lastYear, values, true);
    }
  }
  QSqlDatabase::database().commit();
  return res;
}

bool PCx_Prevision::isPrevisionEmpty() const {
  QSqlQuery q;

  q.exec(QString("select count(*) from prevision_DF_%1 where "
                 "prevision_operators_to_add is not null or "
                 "prevision_operators_to_substract is not null")
             .arg(previsionId));
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next() && q.value(0).toInt() > 0) {
    return false;
  }

  q.exec(QString("select count(*) from prevision_RF_%1 where "
                 "prevision_operators_to_add is not null or "
                 "prevision_operators_to_substract is not null")
             .arg(previsionId));
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next() && q.value(0).toInt() > 0) {
    return false;
  }

  q.exec(QString("select count(*) from prevision_DI_%1 where "
                 "prevision_operators_to_add is not null or "
                 "prevision_operators_to_substract is not null")
             .arg(previsionId));
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next() && q.value(0).toInt() > 0) {
    return false;
  }

  q.exec(QString("select count(*) from prevision_RI_%1 where "
                 "prevision_operators_to_add is not null or "
                 "prevision_operators_to_substract is not null")
             .arg(previsionId));
  if (!q.isActive()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next() && q.value(0).toInt() > 0) {
    return false;
  }

  return true;
}

unsigned int PCx_Prevision::duplicatePrevision(const QString &newName) const {
  if (newName.isEmpty() || newName.size() > MAXOBJECTNAMELENGTH) {
    qFatal("Assertion failed, name out-of-bound");
  }
  unsigned int newPrevId = addNewPrevision(attachedAuditId, newName);
  if (newPrevId == 0) {
    return 0;
  }

  QSqlDatabase::database().transaction();
  QSqlQuery q;
  q.exec(QString("insert into prevision_DF_%1 select * from prevision_DF_%2").arg(newPrevId).arg(previsionId));
  if (q.numRowsAffected() < 0) {
    QSqlDatabase::database().rollback();
    PCx_Prevision::deletePrevision(newPrevId);
    qCritical() << q.lastError();
    die();
  }
  q.exec(QString("insert into prevision_RF_%1 select * from prevision_RF_%2").arg(newPrevId).arg(previsionId));
  if (q.numRowsAffected() < 0) {
    QSqlDatabase::database().rollback();
    PCx_Prevision::deletePrevision(newPrevId);
    qCritical() << q.lastError();
    die();
  }
  q.exec(QString("insert into prevision_DI_%1 select * from prevision_DI_%2").arg(newPrevId).arg(previsionId));
  if (q.numRowsAffected() < 0) {
    QSqlDatabase::database().rollback();
    PCx_Prevision::deletePrevision(newPrevId);
    qCritical() << q.lastError();
    die();
  }
  q.exec(QString("insert into prevision_RI_%1 select * from prevision_RI_%2").arg(newPrevId).arg(previsionId));
  if (q.numRowsAffected() < 0) {
    QSqlDatabase::database().rollback();
    PCx_Prevision::deletePrevision(newPrevId);
    qCritical() << q.lastError();
    die();
  }

  QSqlDatabase::database().commit();
  return newPrevId;
}

PCx_Prevision::~PCx_Prevision() {
  attachedTree = nullptr;
  delete attachedAudit;
}

bool PCx_Prevision::previsionNameExists(const QString &previsionName) {
  QSqlQuery q;
  q.prepare("select count(*) from index_previsions where nom=:name");
  q.bindValue(":name", previsionName);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.next()) {
    qCritical() << q.lastError();
    die();
  }

  if (q.value(0).toLongLong() > 0) {
    return true;
  }
  return false;
}

bool PCx_Prevision::deletePrevision(unsigned int previsionId) {
  if (previsionId == 0) {
    qFatal("Assertion failed");
  }
  QSqlQuery q(QString("select count(*) from index_previsions where id='%1'").arg(previsionId));
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next()) {
    if (q.value(0).toInt() == 0) {
      qWarning() << "Attempting to delete an inexistant prevision !";
      return false;
    }
  }

  QSqlDatabase::database().transaction();
  if (!q.exec(QString("delete from index_previsions where id='%1'").arg(previsionId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table prevision_DF_%1").arg(previsionId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table prevision_RF_%1").arg(previsionId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table prevision_DI_%1").arg(previsionId))) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("drop table prevision_RI_%1").arg(previsionId))) {
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

QList<QPair<unsigned int, QString>> PCx_Prevision::getListOfPrevisions() {
  QList<QPair<unsigned int, QString>> listOfPrevisions;
  QDateTime dt;

  QSqlQuery query;
  if (!query.exec("select * from index_previsions order by datetime(le_timestamp)")) {
    qCritical() << query.lastError();
    die();
  }

  while (query.next()) {
    QString item;
    dt = QDateTime::fromString(query.value("le_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
    dt.setTimeSpec(Qt::UTC);
    QDateTime dtLocal = dt.toLocalTime();
    QPair<unsigned int, QString> p;
    item = QString("%1 - %2").arg(query.value("nom").toString(), dtLocal.toString(Qt::SystemLocaleShortDate));
    p.first = query.value("id").toUInt();
    p.second = item;
    listOfPrevisions.append(p);
  }
  return listOfPrevisions;
}

unsigned int PCx_Prevision::addNewPrevision(unsigned int auditId, const QString &name) {
  if (auditId == 0 || name.isEmpty() || previsionNameExists(name) || name.size() > MAXOBJECTNAMELENGTH) {
    qFatal("Assertion failed");
  }
  QSqlQuery q;

  PCx_Audit tmpAudit(auditId, true);

  if (tmpAudit.isFinished() == false) {
    qCritical() << QObject::tr("Audit non terminé !");
    return 0;
  }

  QSqlDatabase::database().transaction();
  q.prepare("insert into index_previsions (nom,id_audit) values (:nom,:id_audit)");
  q.bindValue(":nom", name);
  q.bindValue(":id_audit", auditId);
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

  if (!q.exec(QString("create table prevision_DF_%1(id integer primary key "
                      "autoincrement, "
                      "id_node integer not null references arbre_%2(id) on "
                      "delete restrict, year integer not null, label text,"
                      "prevision_operators_to_add text, "
                      "prevision_operators_to_substract text, "
                      "computedPrevision integer,"
                      " unique(id_node,year) on conflict replace)")
                  .arg(uLastId)
                  .arg(tmpAudit.getAttachedTreeId()))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("create index idx_idnode_prevision_DF_%1 on "
                      "prevision_DF_%1(id_node)")
                  .arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create table prevision_RF_%1(id integer primary key "
                      "autoincrement, "
                      "id_node integer not null references arbre_%2(id) on "
                      "delete restrict, year integer not null, label text,"
                      "prevision_operators_to_add text, "
                      "prevision_operators_to_substract text, "
                      "computedPrevision integer,"
                      " unique(id_node,year) on conflict replace)")
                  .arg(uLastId)
                  .arg(tmpAudit.getAttachedTreeId()))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create index idx_idnode_prevision_RF_%1 on "
                      "prevision_RF_%1(id_node)")
                  .arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create table prevision_DI_%1(id integer primary key "
                      "autoincrement, "
                      "id_node integer not null references arbre_%2(id) on "
                      "delete restrict, year integer not null, label text,"
                      "prevision_operators_to_add text, "
                      "prevision_operators_to_substract text, "
                      "computedPrevision integer,"
                      " unique(id_node,year) on conflict replace)")
                  .arg(uLastId)
                  .arg(tmpAudit.getAttachedTreeId()))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create index idx_idnode_prevision_DI_%1 on "
                      "prevision_DI_%1(id_node)")
                  .arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }

  if (!q.exec(QString("create table prevision_RI_%1(id integer primary key "
                      "autoincrement, "
                      "id_node integer not null references arbre_%2(id) on "
                      "delete restrict, year integer not null, label text,"
                      "prevision_operators_to_add text, "
                      "prevision_operators_to_substract text, "
                      "computedPrevision integer,"
                      " unique(id_node,year) on conflict replace)")
                  .arg(uLastId)
                  .arg(tmpAudit.getAttachedTreeId()))) {
    qCritical() << q.lastError();
    die();
  }

  if (q.numRowsAffected() == -1) {
    qCritical() << q.lastError();
    die();
  }
  if (!q.exec(QString("create index idx_idnode_prevision_RI_%1 on "
                      "prevision_RI_%1(id_node)")
                  .arg(uLastId))) {
    qCritical() << q.lastError();
    die();
  }
  QSqlDatabase::database().commit();
  return uLastId;
}

QString PCx_Prevision::generateHTMLPrevisionTitle() const {
  return QString("<h3>Prévision %1 portant sur l'audit %2 (%3)</h3>")
      .arg(previsionName.toHtmlEscaped(), attachedAudit->getAuditName().toHtmlEscaped(),
           attachedAudit->getYearsString());
}
