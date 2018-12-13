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

#include "pcx_reportingtablesupervisionmodel.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QtMath>

PCx_ReportingTableSupervisionModel::PCx_ReportingTableSupervisionModel(PCx_Reporting *reporting, MODES::DFRFDIRI mode,
                                                                       unsigned int selectedDateTimeT, QObject *parent)
    : QAbstractTableModel(parent), reporting(reporting), currentMode(mode) {
  this->selectedDateTimeT = selectedDateTimeT;
  updateQuery();
}

void PCx_ReportingTableSupervisionModel::setMode(MODES::DFRFDIRI mode) {
  currentMode = mode;
  beginResetModel();
  updateQuery();
  endResetModel();
}

void PCx_ReportingTableSupervisionModel::setDateTimeT(time_t dateTimeT) {
  selectedDateTimeT = dateTimeT;
  beginResetModel();
  updateQuery();
  endResetModel();
}

QString
PCx_ReportingTableSupervisionModel::getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS column) {
  switch (column) {
  case TABLESUPERVISIONCOLUMNS::BP:
    return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP);
  case TABLESUPERVISIONCOLUMNS::OUVERTS:
    return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS);
  case TABLESUPERVISIONCOLUMNS::REALISES:
    return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES);
  case TABLESUPERVISIONCOLUMNS::ENGAGES:
    return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES);
  case TABLESUPERVISIONCOLUMNS::DISPONIBLES:
    return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::DISPONIBLES);
  case TABLESUPERVISIONCOLUMNS::PERCENTREALISES:
    return tr("% réalisé");
  case TABLESUPERVISIONCOLUMNS::TAUXECART:
    return tr("Taux d'écart");
  case TABLESUPERVISIONCOLUMNS::PERCENTBP:
    return tr("% BP");
  case TABLESUPERVISIONCOLUMNS::PERCENTENGAGES:
    return tr("% engagé");
  case TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES:
    return tr("% disponible");
  case TABLESUPERVISIONCOLUMNS::REALISESPREDITS:
    return tr("Réalises prédits");
  case TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS:
    return tr("Diff. estimée");
  case TABLESUPERVISIONCOLUMNS::DECICP:
    return tr("DECICP");
  case TABLESUPERVISIONCOLUMNS::DERPSUR2:
    return tr("DER=P/2");
  case TABLESUPERVISIONCOLUMNS::RAC:
    return tr("RAC");
  case TABLESUPERVISIONCOLUMNS::NB15NRESTANTES:
    return tr("#15NR");
  case TABLESUPERVISIONCOLUMNS::CPP15NR:
    return tr("CPP15N");
  }
  return QString();
}

int PCx_ReportingTableSupervisionModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return entries.count();
}

int PCx_ReportingTableSupervisionModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 19;
}

QVariant PCx_ReportingTableSupervisionModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::ToolTipRole) {
    QString out;
    switch (index.column()) {
    case 0:
      out = tr("Liste des noeuds de l'arbre actif");
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTBP):
      out = tr("Pourcentage du BP par rapport aux crédits ouverts");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTREALISES):
      out = tr("Pourcentage des réalisés par rapport aux crédits ouverts. En "
               "<span style='color:red'><b>rouge</b></span> lorsque cette valeur"
               " est strictement supérieure à 100%, en <span "
               "style='color:darkgreen'><b>vert</b></span> sinon");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTENGAGES):
      out = tr("Pourcentage des engagés par rapport aux crédits ouverts");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES):
      out = tr("Pourcentage des disponibles par rapport aux crédits ouverts");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::REALISESPREDITS):
      out = tr("Quantité de réalisés qui seront consommés"
               " à la fin de l'année en suivant le rythme actuel\n(= réalisés "
               "/ nombre_de_jours_depuis_le_début_d'année * 365)");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS):
      out = tr("Différence estimée entre les réalisés prédits pour la fin de "
               "l'année et les crédits ouverts au rythme de consommation actuel."
               "\nEn rouge si les crédits ouverts seront insuffisants,"
               " en vert sinon");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::TAUXECART):
      out = tr("Taux d'écart entre les crédits ouverts et le réalisé "
               "prévisionnel de fin d'exercice.\n"
               "Taux d'écart = (réalisé_prévisonnel - "
               "crédits_ouverts)/crédits_ouverts * 100\n"
               "avec réalisé_prévisionnel = réalisés / "
               "nombre_de_jours_depuis_le_début_d'année * 365\n\n"
               " En rouge si les crédits ouverts seront insuffisants, en "
               "vert sinon");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DECICP):
      out = tr("Date estimée de consommation intégrale des crédits ouverts au "
               "rythme actuel");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DERPSUR2):
      out = tr("Date estimée à laquelle les réalisés atteindront la moitié des "
               "crédits ouverts au rythme actuel");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::NB15NRESTANTES):
      out = tr("Nombre de quinzaines avant la fin de l'année");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::RAC):
      out = tr("Crédits restant à consommer (=crédits ouverts-réalisés)");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::CPP15NR):
      out = tr("Consommé Prévu Par Quinzaine Restante (=reste à consommer / "
               "quinzaines restantes)");
      break;
    }
    return out;
  }

  if (role == Qt::DisplayRole || role == Qt::TextColorRole || role == Qt::EditRole) {
    int row = index.row();
    Entry entry = entries.at(row);
    QString output;
    bool percentMode = false;
    double computedValue = std::numeric_limits<double>::quiet_NaN();
    switch (index.column()) {
    case 0:
      return reporting->getAttachedTree()->getNodeName(entry.nodeId);
    case 1:
      return entry.date;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::BP):
      computedValue = entry.bp;
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::OUVERTS):
      computedValue = entry.ouverts;
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::REALISES):
      computedValue = entry.realises;
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::ENGAGES):
      computedValue = entry.engages;
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DISPONIBLES):
      computedValue = entry.disponibles;
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTREALISES):
      if (!qIsNaN(entry.percentReal)) {
        percentMode = true;
        computedValue = entry.percentReal;
      } else
        return ("DIV0");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::TAUXECART):
      if (!qIsNaN(entry.tauxEcart)) {
        percentMode = true;
        computedValue = entry.tauxEcart;
      } else
        return ("DIV0");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTBP):
      if (!qIsNaN(entry.percentBP)) {
        percentMode = true;
        computedValue = entry.percentBP;
      } else
        return ("DIV0");
      break;
    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTENGAGES):
      if (!qIsNaN(entry.percentEngage)) {
        percentMode = true;
        computedValue = entry.percentEngage;
      } else
        return ("DIV0");
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES):
      if (!qIsNaN(entry.percentDisponible)) {
        percentMode = true;
        computedValue = entry.percentDisponible;
      } else
        return ("DIV0");
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::REALISESPREDITS):
      computedValue = entry.realisesPredits;
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS):
      computedValue = entry.diffRealisesPreditsOuverts;
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DECICP):
      return entry.dECICO;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::DERPSUR2):
      return entry.dERO2;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::RAC):
      computedValue = entry.resteAConsommer;
      break;

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::NB15NRESTANTES):
      return (entry.nb15NRestantes);

    case static_cast<int>(TABLESUPERVISIONCOLUMNS::CPP15NR):
      computedValue = entry.consommePrevPar15N;
      break;
    }

    output = NUMBERSFORMAT::formatDouble(computedValue);

    if (role == Qt::DisplayRole) {
      if (percentMode == true)
        output.append("%");
      return output;
    }
    // For correct QSortFilterProxyModel sorting when '%' (otherwise sorted as
    // string, not as number)
    else if (role == Qt::EditRole) {
      return computedValue;
    } else if (role == Qt::TextColorRole) {
      switch (index.column()) {
      case static_cast<int>(TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS):
      case static_cast<int>(TABLESUPERVISIONCOLUMNS::TAUXECART): {
        if (computedValue > 0.0)
          return QVariant::fromValue(QColor(Qt::red));
        else
          return QVariant::fromValue(QColor(Qt::darkGreen));
      }
      case static_cast<int>(TABLESUPERVISIONCOLUMNS::PERCENTREALISES): {
        if (computedValue > 100.0)
          return QVariant::fromValue(QColor(Qt::red));
        else
          return QVariant::fromValue(QColor(Qt::darkGreen));
      }
      }
    }
  } else if (role == Qt::TextAlignmentRole && index.column() > 1) {
    return Qt::AlignCenter;
  }
  return QVariant();
}

QVariant PCx_ReportingTableSupervisionModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    if (section == 0)
      return tr("Nom");
    else if (section == 1)
      return tr("Date");
    else
      return getColumnName(static_cast<TABLESUPERVISIONCOLUMNS>(section));
  }
  return QVariant();
}

void PCx_ReportingTableSupervisionModel::updateQuery() {
  QSqlQuery q;
  entries.clear();

  if (selectedDateTimeT == -1) {
    if (!q.exec(QString("select id_node,MAX(date) as "
                        "mDate,bp,ouverts,realises,engages,disponibles from "
                        "reporting_%1_%2 group by id_node")
                    .arg(MODES::modeToTableString(currentMode))
                    .arg(reporting->getReportingId()))) {
      qCritical() << q.lastError();
      die();
    }
  } else {
    q.prepare(QString("select id_node,date as "
                      "mDate,bp,ouverts,realises,engages,disponibles from "
                      "reporting_%1_%2 where date=:date order by id_node")
                  .arg(MODES::modeToTableString(currentMode))
                  .arg(reporting->getReportingId()));
    q.bindValue(":date", static_cast<int>(selectedDateTimeT));
    if (!q.exec()) {
      qCritical() << q.lastError();
      die();
    }
  }
  while (q.next()) {
    Entry entry(q.value("id_node").toUInt(), q.value("mDate").toUInt(), q.value("bp").toLongLong(),
                q.value("ouverts").toLongLong(), q.value("realises").toLongLong(), q.value("engages").toLongLong(),
                q.value("disponibles").toLongLong());
    entries.append(entry);
  }
}

PCx_ReportingTableSupervisionModel::Entry::Entry(unsigned int nodeId, unsigned int dateTimeT, qint64 _bp,
                                                 qint64 ouverts, qint64 realises, qint64 engages, qint64 disponibles)
    : nodeId(nodeId)

{
  this->bp = NUMBERSFORMAT::fixedPointToDouble(_bp);
  this->ouverts = NUMBERSFORMAT::fixedPointToDouble(ouverts);
  this->realises = NUMBERSFORMAT::fixedPointToDouble(realises);
  this->engages = NUMBERSFORMAT::fixedPointToDouble(engages);
  this->disponibles = NUMBERSFORMAT::fixedPointToDouble(disponibles);
  date = QDateTime::fromTime_t(dateTimeT).date();

  int nbJoursDepuisDebutAnnee = date.dayOfYear();
  int nbJoursRestants = 365 - nbJoursDepuisDebutAnnee;
  nb15NRestantes = static_cast<unsigned int>(qFloor(static_cast<double>(nbJoursRestants) / 15.0));
  resteAConsommer = this->ouverts - this->realises;
  if (nb15NRestantes != 0) {
    consommePrevPar15N = resteAConsommer / nb15NRestantes;
  } else
    consommePrevPar15N = std::numeric_limits<double>::quiet_NaN();

  realisesPredits = this->realises / nbJoursDepuisDebutAnnee * 365;

  diffRealisesPreditsOuverts = realisesPredits - this->ouverts;

  this->dECICO = QDate();
  this->dERO2 = QDate();
  if (realisesPredits != 0.0) {
    auto joursDCICO = static_cast<int>(365 * this->ouverts / realisesPredits);
    if (joursDCICO > 0) {
      QDate tmpDate(date.year(), 1, 1);
      dECICO = tmpDate.addDays(joursDCICO);
    }
  }

  double ouvertsDemi = this->ouverts / 2.0;
  if (this->realises < ouvertsDemi) {
    if (this->realises != 0.0) {
      QDate tmpDate(date.year(), 1, 1);
      auto joursCoupure = static_cast<int>(ouvertsDemi * nbJoursDepuisDebutAnnee / this->realises);
      this->dERO2 = tmpDate.addDays(joursCoupure);
    }
  }

  if (ouverts != 0) {
    percentReal = this->realises / this->ouverts * 100.0;
    percentBP = this->bp / this->ouverts * 100.0;
    percentEngage = this->engages / this->ouverts * 100.0;
    percentDisponible = this->disponibles / this->ouverts * 100.0;
    tauxEcart = (realisesPredits - this->ouverts) / this->ouverts * 100.0;
  } else {
    percentReal = std::numeric_limits<double>::quiet_NaN();
    percentBP = std::numeric_limits<double>::quiet_NaN();
    percentEngage = std::numeric_limits<double>::quiet_NaN();
    percentDisponible = std::numeric_limits<double>::quiet_NaN();
    tauxEcart = std::numeric_limits<double>::quiet_NaN();
  }
}
