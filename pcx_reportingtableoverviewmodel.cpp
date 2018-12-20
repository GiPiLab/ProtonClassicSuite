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

#include "pcx_reportingtableoverviewmodel.h"
#include "utils.h"
#include <QSqlRecord>

PCx_ReportingTableOverviewModel::PCx_ReportingTableOverviewModel(PCx_Reporting *reporting, unsigned int nodeId,
                                                                 PCx_ReportingTableOverviewModel::OVERVIEWMODE mode,
                                                                 QObject *parent)
    : QAbstractTableModel(parent), reporting(reporting), nodeId(nodeId), mode(mode) {
  if (reporting == nullptr || nodeId == 0) {
    qFatal("Assertion failed");
  }
  updateQuery();
  colRef = PCx_Reporting::OREDPCR::NONELAST;
  dateRef = -1;
}

void PCx_ReportingTableOverviewModel::setNodeId(unsigned int nodeId) {
  this->nodeId = nodeId;
  updateQuery();
}

void PCx_ReportingTableOverviewModel::setMode(OVERVIEWMODE mode) {
  this->mode = mode;
  updateQuery();
}

void PCx_ReportingTableOverviewModel::setColRef(PCx_Reporting::OREDPCR ref) {
  colRef = ref;
  dateRef = -1;
  updateQuery();
}

QString
PCx_ReportingTableOverviewModel::OVERVIEWMODEToCompleteString(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode) {
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF) {
    return MODES::modeToCompleteString(MODES::DFRFDIRI::DF);
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF) {
    return MODES::modeToCompleteString(MODES::DFRFDIRI::RF);
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI) {
    return MODES::modeToCompleteString(MODES::DFRFDIRI::DI);
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI) {
    return MODES::modeToCompleteString(MODES::DFRFDIRI::RI);
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF) {
    return tr("Synthèse de fonctionnement (RF - DF)");
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI) {
    return tr("Synthèse d'investissement (RI - DI)");
  }
  if (mode == PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI) {
    return tr("Synthèse fonctionnement + investissement (RF - DF + RI - DI)");
  }
  qWarning() << "Invalid mode";

  return QString();
}

void PCx_ReportingTableOverviewModel::setDateRef(time_t dateFromTimet) {
  dateRef = dateFromTimet;
  colRef = PCx_Reporting::OREDPCR::NONELAST;
  updateQuery();
}

void PCx_ReportingTableOverviewModel::updateQuery() {
  beginResetModel();
  switch (mode) {
  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF:
    queryModel.setQuery(QString("select "
                                "date,bp,ouverts,realises,engages,disponibles,reports,ocdm,"
                                "vcdm,budgetvote,vcinterne,rattachenmoins1 from "
                                "reporting_DF_%1 where id_node=%2 order by date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF:
    queryModel.setQuery(QString("select "
                                "date,bp,ouverts,realises,engages,disponibles,reports,ocdm,"
                                "vcdm,budgetvote,vcinterne,rattachenmoins1 from "
                                "reporting_RF_%1 where id_node=%2 order by date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI:
    queryModel.setQuery(QString("select "
                                "date,bp,ouverts,realises,engages,disponibles,reports,ocdm,"
                                "vcdm,budgetvote,vcinterne,rattachenmoins1 from "
                                "reporting_DI_%1 where id_node=%2 order by date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI:
    queryModel.setQuery(QString("select "
                                "date,bp,ouverts,realises,engages,disponibles,reports,ocdm,"
                                "vcdm,budgetvote,vcinterne,rattachenmoins1 from "
                                "reporting_RI_%1 where id_node=%2 order by date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF:
    queryModel.setQuery(QString("select a.date,"
                                "coalesce(a.bp,0)-coalesce(b.bp,0) as bp,"
                                "coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as ouverts,"
                                "coalesce(a.realises,0)-coalesce(b.realises,0) as realises,"
                                "coalesce(a.engages,0)-coalesce(b.engages,0) as engages,"
                                "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) as "
                                "disponibles,"
                                "coalesce(a.reports,0)-coalesce(b.reports,0) as reports,"
                                "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) as ocdm,"
                                "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) as vcdm,"
                                "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) as budgetvote,"
                                "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) as vcinterne,"
                                "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) as "
                                "rattachenmoins1 "
                                "from reporting_RF_%1 as a, reporting_DF_%1 as b where "
                                "a.id_node=%2 and b.id_node=%2 "
                                "and a.date=b.date order by a.date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI:
    queryModel.setQuery(QString("select a.date,"
                                "coalesce(a.bp,0)-coalesce(b.bp,0) as bp,"
                                "coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as ouverts,"
                                "coalesce(a.realises,0)-coalesce(b.realises,0) as realises,"
                                "coalesce(a.engages,0)-coalesce(b.engages,0) as engages,"
                                "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) as "
                                "disponibles,"
                                "coalesce(a.reports,0)-coalesce(b.reports,0) as reports,"
                                "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) as ocdm,"
                                "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) as vcdm,"
                                "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) as budgetvote,"
                                "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) as vcinterne,"
                                "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) as "
                                "rattachenmoins1 "
                                "from reporting_RI_%1 as a, reporting_DI_%1 as b where "
                                "a.id_node=%2 and b.id_node=%2 "
                                "and a.date=b.date order by a.date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;

  case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI:
    queryModel.setQuery(QString("select a.date,"
                                "coalesce(a.bp,0)-coalesce(b.bp,0) + "
                                "coalesce(c.bp,0)-coalesce(d.bp,0) as bp,"
                                "coalesce(a.ouverts,0)-coalesce(b.ouverts,0) + "
                                "coalesce(c.ouverts,0)-coalesce(d.ouverts,0) as ouverts,"
                                "coalesce(a.realises,0)-coalesce(b.realises,0) + "
                                "coalesce(c.realises,0)-coalesce(d.realises,0) as realises,"
                                "coalesce(a.engages,0)-coalesce(b.engages,0) + "
                                "coalesce(c.engages,0)-coalesce(d.engages,0) as engages,"
                                "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) + "
                                "coalesce(c.disponibles,0)-coalesce(d.disponibles,0) as "
                                "disponibles,"
                                "coalesce(a.reports,0)-coalesce(b.reports,0) + "
                                "coalesce(c.reports,0)-coalesce(d.reports,0) as reports,"
                                "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) + "
                                "coalesce(c.ocdm,0)-coalesce(d.ocdm,0) as ocdm,"
                                "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) + "
                                "coalesce(c.vcdm,0)-coalesce(d.vcdm,0) as vcdm,"
                                "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) + "
                                "coalesce(c.budgetvote,0)-coalesce(d.budgetvote,0) as budgetvote,"
                                "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) + "
                                "coalesce(c.vcinterne,0)-coalesce(d.vcinterne,0) as vcinterne,"
                                "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) + "
                                "coalesce(c.rattachenmoins1,0)-coalesce(d.rattachenmoins1,0) as "
                                "rattachenmoins1 "
                                "from reporting_RF_%1 as a, reporting_DF_%1 as b, "
                                "reporting_RI_%1 "
                                "as c, reporting_DI_%1 as d where a.id_node=%2 and b.id_node=%2 "
                                "and c.id_node=%2 and d.id_node=%2 and a.date=b.date and "
                                "b.date=c.date and c.date=d.date order by a.date desc")
                            .arg(reporting->getReportingId())
                            .arg(nodeId));
    break;
  }
  endResetModel();
}

QVariant PCx_ReportingTableOverviewModel::headerData(int section, Qt::Orientation orientation, int role) const {
  QVariant value = queryModel.headerData(section, orientation, role);
  if (value.isValid() && role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    QString val = value.toString();
    if (val != "date") {
      return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCRFromTableString(val));
    }
  }
  return value;
}

QVariant PCx_ReportingTableOverviewModel::data(const QModelIndex &item, int role) const {
  QVariant value;
  bool percentMode = false;

  if (role == Qt::TextColorRole || role == Qt::DisplayRole || role == Qt::EditRole) {
    value = queryModel.record(item.row()).value(item.column());
    // The date column
    if (item.column() == 0) {
      return QDateTime::fromTime_t(value.toUInt()).date();
    }

    double computedValue = NUMBERSFORMAT::fixedPointToDouble(value.toLongLong());
    QString retVal;

    if (dateRef != -1) {
      qint64 valRefDate = -1;
      for (int i = 0; i < rowCount(item); i++) {
        if (queryModel.record(i).value("date").toInt() == dateRef) {
          valRefDate = queryModel.record(i).value(item.column()).toLongLong();
          break;
        }
      }
      if (valRefDate == 0) {
        return "DIV0";
      } else {
        computedValue = static_cast<double>(value.toLongLong() / static_cast<double>(valRefDate) * 100);
        retVal = NUMBERSFORMAT::formatDouble(computedValue) + "%";
      }
      percentMode = true;
    }

    else if (colRef != PCx_Reporting::OREDPCR::NONELAST) {
      qint64 valRef = queryModel.record(item.row()).value(PCx_Reporting::OREDPCRtoTableString(colRef)).toLongLong();
      if (valRef != 0) {
        computedValue = static_cast<double>(value.toLongLong() / static_cast<double>(valRef) * 100);
        retVal = NUMBERSFORMAT::formatDouble(computedValue) + "%";
      } else {
        return "DIV0";
      }
      percentMode = true;
    } else {
      retVal = NUMBERSFORMAT::formatDouble(computedValue);
    }

    if (role == Qt::DisplayRole) {
      return retVal;
    }

    // For correct numeric sorting when '%' appended
    if (role == Qt::EditRole) {
      return computedValue;
    }

    if (role == Qt::TextColorRole) {
      if (computedValue < 0.0) {
        return QVariant::fromValue(QColor(Qt::red));
      }
      if (percentMode) {
        if (computedValue > 100.0) {
          return QVariant::fromValue(QColor(Qt::green));
        }
        if (computedValue > 0.0 && computedValue < 100.0) {
          return QVariant::fromValue(QColor(Qt::blue));
        }
      }
    }
  }

  else if (role == Qt::TextAlignmentRole && item.column() > 0) {
    return Qt::AlignCenter;
  }

  return QVariant();
}
