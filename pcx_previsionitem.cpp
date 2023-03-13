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

#include "pcx_previsionitem.h"
#include "pcx_report.h"
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSqlError>
#include <QSqlQuery>

PCx_PrevisionItem::PCx_PrevisionItem(PCx_Prevision *prevision, MODES::DFRFDIRI mode, unsigned int nodeId, int year)
    : prevision(prevision), mode(mode), nodeId(nodeId), year(year), computedValue(0) {}

PCx_PrevisionItem::~PCx_PrevisionItem() = default;

void PCx_PrevisionItem::loadFromDb() {
  QSqlQuery q;
  computedValue = 0;
  itemsToAdd.clear();
  itemsToSubstract.clear();
  label = QString();

  q.prepare(QString("select * from prevision_%1_%2 where id_node=:id_node and year=:year")
                .arg(MODES::modeToTableString(mode))
                .arg(prevision->getPrevisionId()));
  q.bindValue(":id_node", nodeId);
  q.bindValue(":year", year);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.next()) {
    label = q.value("label").toString();
    computedValue = q.value("computedPrevision").toLongLong();
    QStringList items = q.value("prevision_operators_to_add").toString().split(';', Qt::SkipEmptyParts);
    foreach (const QString &item, items) {
      PCx_PrevisionItemCriteria criteria(item);
      itemsToAdd.append(criteria);
    }

    items.clear();
    items = q.value("prevision_operators_to_substract").toString().split(';', Qt::SkipEmptyParts);

    foreach (const QString &item, items) {
      PCx_PrevisionItemCriteria criteria(item);
      itemsToSubstract.append(criteria);
    }
  }
}

QString PCx_PrevisionItem::displayPrevisionItemReportInQTextDocument(QTextDocument *document,
                                                                     unsigned int referenceNode) const {
  QString out;
  PCx_Report report(prevision->getAttachedAudit());
  QSettings settings;
  report.getGraphics().setGraphicsWidth(settings.value("graphics/width", PCx_Graphics::DEFAULTWIDTH).toInt());
  report.getGraphics().setGraphicsHeight(settings.value("graphics/height", PCx_Graphics::DEFAULTHEIGHT).toInt());

  out = PCx_Report::generateMainHTMLHeader();
  out.append(prevision->generateHTMLPrevisionTitle());
  out.append("<h4>" + MODES::modeToCompleteString(mode).toHtmlEscaped() + "</h4>");
  QList<PCx_Tables::PCATABLES> tables = {PCx_Tables::PCATABLES::PCARAWDATA};
  QList<PCx_Graphics::PCAGRAPHICS> graphics = {PCx_Graphics::PCAGRAPHICS::PCAHISTORY, PCx_Graphics::PCAGRAPHICS::PCAG1,
                                               PCx_Graphics::PCAGRAPHICS::PCAG2};

  out.append("<h2>" + prevision->getAttachedTree()->getNodeName(nodeId).toHtmlEscaped() + "</h2>");
  out.append(QString("<p align='center' style='font-size:14pt'>Valeur prévue pour %1 "
                     ": <b>%2€</b></p>")
                 .arg(year)
                 .arg(NUMBERSFORMAT::formatFixedPoint(getSummedPrevisionItemValue())));
  out.append(getPrevisionItemAsHTML());
  out.append("<p align='center'><b>Données historiques et prévision</b></p>");
  out.append(report.generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS>(), tables, graphics, nodeId, mode,
                                                   referenceNode, document, nullptr, this));
  out.append("</body></html>");
  document->setHtml(out);
  return out;
}


bool PCx_PrevisionItem::savePrevisionItemReport(const QString &fileName, bool showDescendants,
                                                unsigned int referenceNode) const {
  QFileInfo fi(fileName);

  PCx_Report report(prevision->getAttachedAudit());
  QSettings settings;
  report.getGraphics().setGraphicsWidth(settings.value("graphics/width", PCx_Graphics::DEFAULTWIDTH).toInt());
  report.getGraphics().setGraphicsHeight(settings.value("graphics/height", PCx_Graphics::DEFAULTHEIGHT).toInt());

  QString out = PCx_Report::generateMainHTMLHeader();
  out.append(prevision->generateHTMLPrevisionTitle());
  QList<unsigned int> descendants;

  out.append("<h4>" + MODES::modeToCompleteString(mode).toHtmlEscaped() + "</h4>");

  if (showDescendants) {
    descendants = prevision->getAttachedTree()->getDescendantsId(nodeId);
    QList<unsigned int> dPlusMe = descendants;
    dPlusMe.prepend(nodeId);    

    QMap<unsigned int,QUrl>nodeToUrl;
    foreach(unsigned int node,dPlusMe)
    {
        nodeToUrl.insert(node,QUrl(QString("#PCXnode%1").arg(node)));
    }
    out.append(report.generateSVGTOC(nodeToUrl));
  }

  QList<PCx_Tables::PCATABLES> tables = {PCx_Tables::PCATABLES::PCARAWDATA};
  QList<PCx_Graphics::PCAGRAPHICS> graphics = {PCx_Graphics::PCAGRAPHICS::PCAHISTORY, PCx_Graphics::PCAGRAPHICS::PCAG1,
                                               PCx_Graphics::PCAGRAPHICS::PCAG2};

  //Warning with id : graphviz generates 'nodexx' id, no duplicates allowed
  out.append(QString("<h2 id='PCXnode%1'>" + prevision->getAttachedTree()->getNodeName(nodeId).toHtmlEscaped() + "</h2>")
                 .arg(nodeId));
  out.append(QString("<p align='center' style='font-size:14pt'>Valeur prévue pour %1 "
                     ": <b>%2€</b></p>")
                 .arg(year)
                 .arg(NUMBERSFORMAT::formatFixedPoint(getSummedPrevisionItemValue())));
  out.append(getPrevisionItemAsHTML());
  qint64 prevNode;
  out.append("<p align='center'><b>Données historiques et prévision</b></p>");

  out.append(report.generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS>(), tables, graphics, nodeId, mode,
                                                   referenceNode, nullptr, nullptr, this));
  if (showDescendants) {
    int maximumProgressValue = (descendants.count()) * (graphics.size());
    QProgressDialog progress(QObject::tr("Enregistrement en cours..."), QObject::tr("Annuler"), 0,
                             maximumProgressValue);
    progress.setMinimumDuration(10);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setValue(0);
    foreach (unsigned int descendant, descendants) {
      out.append(
                  //Warning with id : graphviz generates 'nodexx' id, no duplicates allowed
          QString("<h2 id='PCXnode%1'>" + prevision->getAttachedTree()->getNodeName(descendant).toHtmlEscaped() + "</h2>")
              .arg(descendant));

      PCx_PrevisionItem tmpItem(prevision, mode, descendant, year);
      tmpItem.loadFromDb();
      prevNode = tmpItem.getSummedPrevisionItemValue();
      out.append("<p align='center'><b>Données historiques et prévision</b></p>");
      out.append(QString("<p align='center' style='font-size:14pt'>Valeur "
                         "prévue pour %1 : <b>%2€</b></p>")
                     .arg(year)
                     .arg(NUMBERSFORMAT::formatFixedPoint(prevNode)));

      out.append(tmpItem.getPrevisionItemAsHTML());
      out.append(report.generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS>(), tables, graphics, descendant,
                                                       mode, referenceNode, nullptr, &progress, &tmpItem));
      if (progress.wasCanceled()) {
        return false;
      }
    }
  }

  out.append("</body></html>");

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(nullptr, QObject::tr("Attention"),
                          QObject::tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    return false;
  }
  QTextStream stream(&file);
  stream << out;
  stream.flush();
  file.close();
  // progress.setValue(maximumProgressValue);
  if (stream.status() == QTextStream::Ok) {
    QMessageBox::information(nullptr, QObject::tr("Information"),
                             QObject::tr("Le document <b>%1</b> a bien été enregistré")
                                 .arg(fi.fileName().toHtmlEscaped()));
  } else {
    QMessageBox::critical(nullptr, QObject::tr("Attention"), QObject::tr("Le document n'a pas pu être enregistré !"));
    return false;
  }

  return true;
}

QString PCx_PrevisionItem::getPrevisionItemsOfDescendantsAsHTML() const {
  QString output;

  if (!prevision->getAttachedTree()->isLeaf(nodeId)) {
    output =
        QString("<h3>Critères appliqués aux descendants de <b>%1</b> (%2)</h3>")
            .arg(prevision->getAttachedTree()->getNodeName(nodeId).toHtmlEscaped(), MODES::modeToCompleteString(mode));
    QList<unsigned int> descendants = prevision->getAttachedTree()->getDescendantsId(nodeId);
    foreach (unsigned int descendant, descendants) {
      PCx_PrevisionItem tmpItem(prevision, mode, descendant, year);
      tmpItem.loadFromDb();
      if (!tmpItem.getItemsToAdd().isEmpty() || !tmpItem.getItemsToSubstract().isEmpty()) {
        output.append(QString("<p><a href='#node_%3'><b>%1 : %2€</b></a></p>")
                          .arg(prevision->getAttachedTree()->getNodeName(descendant).toHtmlEscaped(),
                               NUMBERSFORMAT::formatFixedPoint(tmpItem.getSummedPrevisionItemValue()))
                          .arg(descendant));
        output.append(tmpItem.getPrevisionItemAsHTML());
      }
    }
  }

  else {
    if (!itemsToAdd.isEmpty() || !itemsToSubstract.isEmpty()) {
      output.append(QString("<p><b>%1 : %2€</b></p>")
                        .arg(prevision->getAttachedTree()->getNodeName(nodeId).toHtmlEscaped(),
                             NUMBERSFORMAT::formatFixedPoint(getSummedPrevisionItemValue())));

      output.append(getPrevisionItemAsHTML());
    }
  }
  return output;
}

qint64 PCx_PrevisionItem::getSummedPrevisionItemValue() const {
  qint64 total = 0;

  if (prevision->getAttachedTree()->isLeaf(nodeId)) {
    return computedValue;
  }

  QList<unsigned int> childrenLeaves = prevision->getAttachedTree()->getLeavesId(nodeId);

  /* Fastest but possibly limited by the number of statements in 'IN' sql
QStringList childrenLeavesIdString;
foreach(unsigned int leaf,childrenLeaves)
{
    childrenLeavesIdString.append(QString::number(leaf));
}

QSqlQuery q(QString("select sum(computedPrevision) from prevision_%1_%2 where
year=%3 and id_node in (%4)") .arg(MODES::modeToTableString(mode))
            .arg(prevision->getPrevisionId())
            .arg(year)
            .arg(childrenLeavesIdString.join(',')));

if(q.next())
{
    total=q.value(0).toLongLong();
}
else
{
    qCritical()<<q.lastError();
    die();
}
*/
  QString reqString = QString("select computedPrevision from prevision_%1_%2 "
                              "where year=%3 and id_node=")
                          .arg(MODES::modeToTableString(mode))
                          .arg(prevision->getPrevisionId())
                          .arg(year);

  foreach (unsigned int leaf, childrenLeaves) {
    QSqlQuery q(reqString + QString::number(leaf));

    if (q.next()) {
      total += q.value(0).toLongLong();
    }
  }
  return total;
}

qint64 PCx_PrevisionItem::getPrevisionItemValue() const {
  qint64 total = 0;

  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToAdd) {
    total += criteria.compute(prevision->getAttachedAuditId(), mode, nodeId);
  }
  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToSubstract) {
    total -= criteria.compute(prevision->getAttachedAuditId(), mode, nodeId);
  }
  return total;
}

void PCx_PrevisionItem::insertCriteriaToAdd(const PCx_PrevisionItemCriteria &criteria, bool compute) {
  itemsToAdd.append(criteria);
  if (compute) {
    computedValue = getPrevisionItemValue();
  }
}

void PCx_PrevisionItem::dispatchCriteriaItemsToChildrenLeaves() {
  QList<unsigned int> descendantsId;
  QList<unsigned int> leavesId;

  descendantsId = prevision->getAttachedTree()->getDescendantsId(nodeId);
  leavesId = prevision->getAttachedTree()->getLeavesId(nodeId);

  // Dispatch items to children leaves

  QSqlDatabase::database().transaction();
  foreach (unsigned int descendant, descendantsId) {
    PCx_PrevisionItem tmpItem(prevision, mode, descendant, year);

    if (leavesId.contains(descendant)) {
      foreach (const PCx_PrevisionItemCriteria &criteria, itemsToAdd) { tmpItem.insertCriteriaToAdd(criteria, false); }
      foreach (const PCx_PrevisionItemCriteria &criteria, itemsToSubstract) {
        tmpItem.insertCriteriaToSub(criteria, false);
      }
      tmpItem.computedValue = tmpItem.getPrevisionItemValue();
    }
    tmpItem.saveDataToDb();
  }

  // Remove current items
  if (!leavesId.contains(nodeId)) {
    deleteAllCriteria();
  }
  saveDataToDb();

  // Remove items from ancestors

  QList<unsigned int> ancestors = prevision->getAttachedTree()->getAncestorsId(nodeId);
  foreach (unsigned int ancestor, ancestors) {
    PCx_PrevisionItem tmpModel(prevision, mode, ancestor, year);
    tmpModel.deleteAllCriteria();
    tmpModel.saveDataToDb();
  }
  QSqlDatabase::database().commit();
}

bool PCx_PrevisionItem::dispatchComputedValueToChildrenLeaves(PCx_Audit::ORED oredReference) {
  QList<unsigned int> descendantsId = prevision->getAttachedTree()->getDescendantsId(nodeId);
  QList<unsigned int> leavesId = prevision->getAttachedTree()->getLeavesId(nodeId);

  int lastYear = prevision->getAttachedAudit()->getYears().last();
  qint64 total = prevision->getAttachedAudit()->getNodeValue(nodeId, mode, oredReference, lastYear);

  computedValue = getPrevisionItemValue();

  if (computedValue != 0 && (total == 0 || total == -MAX_NUM)) {
    QMessageBox::warning(nullptr, QObject::tr("Attention"),
                         QObject::tr("Je ne peux pas répartir cette somme sur la base de %2 "
                                     "nuls pour %1 !")
                             .arg(lastYear)
                             .arg(PCx_Audit::OREDtoCompleteString(oredReference, true)));
    return false;
  }

  QSqlDatabase::database().transaction();
  if (itemsToAdd.isEmpty() && itemsToSubstract.isEmpty()) {
    foreach (unsigned int descendant, descendantsId) {
      PCx_PrevisionItem tmpItem(prevision, mode, descendant, year);
      tmpItem.deleteAllCriteria();
      tmpItem.saveDataToDb();
    }
  }

  else {
    foreach (unsigned int descendant, descendantsId) {
      PCx_PrevisionItem tmpItem(prevision, mode, descendant, year);

      if (leavesId.contains(descendant)) {
        qint64 val = prevision->getAttachedAudit()->getNodeValue(descendant, mode, oredReference, lastYear);
        if (val != -MAX_NUM) {
          double percent = static_cast<double>(val) / total;
          double newVal = NUMBERSFORMAT::fixedPointToDouble(computedValue) * percent;
          PCx_PrevisionItemCriteria criteria(
              PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION, oredReference,
              NUMBERSFORMAT::doubleToFixedPoint(newVal));
          tmpItem.insertCriteriaToAdd(criteria);
          tmpItem.saveDataToDb();
        } else {
          PCx_PrevisionItemCriteria criteria(
              PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION, oredReference, 0);
          tmpItem.insertCriteriaToAdd(criteria);
          tmpItem.saveDataToDb();
        }
      } else {
        tmpItem.saveDataToDb();
      }
    }
  }
  // Remove items from ancestors
  QList<unsigned int> ancestors = prevision->getAttachedTree()->getAncestorsId(nodeId);
  foreach (unsigned int ancestor, ancestors) {
    PCx_PrevisionItem tmpItem(prevision, mode, ancestor, year);
    tmpItem.saveDataToDb();
  }
  QSqlDatabase::database().commit();
  return true;
}

void PCx_PrevisionItem::insertCriteriaToSub(const PCx_PrevisionItemCriteria &criteria, bool compute) {
  itemsToSubstract.append(criteria);
  if (compute) {
    computedValue = getPrevisionItemValue();
  }
}

bool PCx_PrevisionItem::deleteCriteria(QModelIndexList selectedIndexes, bool compute) {
  // WARNING : only single selection supported
  if (selectedIndexes.isEmpty()) {
    return true;
  }
  if (selectedIndexes.size() > 1) {
    qDebug() << "Multiselect not implemented";
  }
  const QModelIndex &index = selectedIndexes[0];

  if (index.row() < itemsToAdd.size()) {
    itemsToAdd.removeAt(index.row());
  } else {
    itemsToSubstract.removeAt(index.row() - itemsToAdd.size());
  }
  if (compute) {
    computedValue = getPrevisionItemValue();
  }
  //  saveDataToDb();

  return true;
}

void PCx_PrevisionItem::deleteAllCriteria() {
  itemsToAdd.clear();
  itemsToSubstract.clear();
  computedValue = 0;
  //  saveDataToDb();
}

QString PCx_PrevisionItem::getPrevisionItemAsHTML() const {
  QString output;
  bool toAdd = false, toSub = false;

  if (itemsToAdd.count() > 0) {
    toAdd = true;
    output.append("<p><i>Critères à ajouter :</i></p><ul>");
  }

  unsigned int auditId = prevision->getAttachedAuditId();

  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToAdd) {
    output.append(QString("<li style='color:darkgreen'>%1 : %2€</li>")
                      .arg(criteria.getCriteriaLongDescription().toHtmlEscaped(),
                           NUMBERSFORMAT::formatFixedPoint(criteria.compute(auditId, mode, nodeId))));
  }
  if (toAdd) {
    output.append("</ul>");
  }

  if (itemsToSubstract.count() > 0) {
    toSub = true;
    output.append("<p><i>Critères à soustraire :</i></p><ul>");
  }

  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToSubstract) {
    output.append(QString("<li style='color:darkred'>%1 : %2€</li>")
                      .arg(criteria.getCriteriaLongDescription().toHtmlEscaped(),
                           NUMBERSFORMAT::formatFixedPoint(criteria.compute(auditId, mode, nodeId))));
  }
  if (toSub) {
    output.append("</ul>");
  }
  return output;
}

void PCx_PrevisionItem::saveDataToDb() {
  QSqlQuery q;
  QStringList items;
  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToAdd) { items.append(criteria.serialize()); }
  QString addString = items.join(';');
  items.clear();
  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToSubstract) { items.append(criteria.serialize()); }
  QString subString = items.join(';');

  computedValue = getPrevisionItemValue();

  q.prepare(QString("insert into prevision_%1_%2 "
                    "(id_node,year,prevision_operators_to_add, "
                    "prevision_operators_to_substract,computedPrevision) "
                    "values (:idnode,:year,:prevopadd,:prevopsub,:computed)")
                .arg(MODES::modeToTableString(mode))
                .arg(prevision->getPrevisionId()));
  q.bindValue(":idnode", nodeId);
  q.bindValue(":year", year);
  q.bindValue(":prevopadd", addString);
  q.bindValue(":prevopsub", subString);
  q.bindValue(":computed", computedValue);
  if (!q.exec()) {
    qCritical() << q.lastError();
    die();
  }
  if (q.numRowsAffected() != 1) {
    qCritical() << q.lastError();
    die();
  }
}

QString PCx_PrevisionItem::getLongDescription() const {
  QString out;
  QStringList listItems;
  if (itemsToAdd.empty() && itemsToSubstract.empty()) {
    return QObject::tr("aucune prévision");
  }
  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToAdd) {
    listItems.append(criteria.getCriteriaLongDescription());
  }
  out.append(listItems.join('+'));

  listItems.clear();

  foreach (const PCx_PrevisionItemCriteria &criteria, itemsToSubstract) {
    listItems.append(criteria.getCriteriaLongDescription());
  }
  if (!listItems.empty()) {
    out.append('-');
    out.append(listItems.join('-'));
  }
  return out;
}
