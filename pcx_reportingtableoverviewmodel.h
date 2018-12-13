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

#ifndef PCX_REPORTINGTABLEOVERVIEWMODEL_H
#define PCX_REPORTINGTABLEOVERVIEWMODEL_H

#include "pcx_reporting.h"
#include <QAbstractTableModel>
#include <QSqlQueryModel>

class PCx_ReportingTableOverviewModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum class OVERVIEWMODE { DF, RF, DI, RI, RFDF, RIDI, RFDFRIDI };

  explicit PCx_ReportingTableOverviewModel(PCx_Reporting *reporting, unsigned int nodeId, OVERVIEWMODE mode,
                                           QObject *parent = nullptr);

  unsigned int getNodeId() const { return nodeId; }
  PCx_ReportingTableOverviewModel::OVERVIEWMODE getMode() const { return mode; }
  void setNodeId(unsigned int nodeId);
  void setMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode);
  PCx_Reporting::OREDPCR getColRef() const { return colRef; }
  void setColRef(PCx_Reporting::OREDPCR ref);

  static QString OVERVIEWMODEToCompleteString(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode);

  /**
   * @brief getDateRef returns the date that should be used as reference, -1 if
   * none
   * @return the date in time_t format
   */
  time_t getDateRef() const { return dateRef; }
  /**
   * @brief setDateRef sets the date that will be used as reference, as stored
   * in DB
   * @param dateFromTimet the date (in time_t format)
   */
  void setDateRef(time_t dateFromTimet);

  int rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return queryModel.rowCount();
  }
  int columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return queryModel.columnCount();
  }
  QVariant data(const QModelIndex &item, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:
  PCx_Reporting *reporting;
  unsigned int nodeId;
  PCx_ReportingTableOverviewModel::OVERVIEWMODE mode;
  PCx_Reporting::OREDPCR colRef;
  time_t dateRef;
  QSqlQueryModel queryModel;
  void updateQuery();
};

#endif // PCX_REPORTINGTABLEOVERVIEWMODEL_H
