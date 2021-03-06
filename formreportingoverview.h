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

#ifndef FORMREPORTINGOVERVIEW_H
#define FORMREPORTINGOVERVIEW_H

#include "pcx_reportingtableoverviewmodel.h"
#include "pcx_reportingwithtreemodel.h"
#include <QSortFilterProxyModel>
#include <QTextDocument>
#include <QWidget>

namespace Ui {
class FormReportingOverview;
}

class FormReportingOverview : public QWidget {
  Q_OBJECT

public:
  explicit FormReportingOverview(QWidget *parent = nullptr);
  ~FormReportingOverview();

public slots:
  void onListOfReportingsChanged();
  void onReportingDataChanged(unsigned int reportingId);

private slots:
  void on_comboListReportings_activated(int index);

  void on_treeView_clicked(const QModelIndex &index);

  void on_radioButtonDF_toggled(bool checked);

  void on_radioButtonRF_toggled(bool checked);

  void on_radioButtonRI_toggled(bool checked);

  void on_radioButtonDI_toggled(bool checked);

  void on_checkBoxBP_toggled(bool checked);

  void on_checkBoxReports_toggled(bool checked);

  void on_checkBoxOCDM_toggled(bool checked);

  void on_checkBoxVCDM_toggled(bool checked);

  void on_checkBoxBudgetVote_toggled(bool checked);

  void on_checkBoxVCInternes_toggled(bool checked);

  void on_checkBoxRattaches_toggled(bool checked);

  void on_checkBoxOuverts_toggled(bool checked);

  void on_checkBoxRealises_toggled(bool checked);

  void on_checkBoxEngages_toggled(bool checked);

  void on_checkBoxDisponibles_toggled(bool checked);

  void on_pushButtonCopyToDocument_clicked();

  void on_comboBoxRefColumn_activated(int index);

  void on_comboBoxRefDate_activated(int index);

  void on_radioButtonRFDF_toggled(bool checked);

  void on_radioButtonRIDI_toggled(bool checked);

  void on_radioButtonRFDFRIDI_toggled(bool checked);

  void on_pushButtonClear_clicked();

  void on_pushButtonSelectAll_clicked();

  void on_pushButtonSelectNone_clicked();

  void on_pushButtonExportHTML_clicked();

  void on_pushButtonExpandAll_clicked();

  void on_pushButtonCollapseAll_clicked();

private:
  Ui::FormReportingOverview *ui;
  void updateListOfReportings();
  void updateComboRefDate(QComboBox *combo);
  void changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode);

  bool dateExistsForNodeAndMode(time_t timeT, unsigned int nodeId,
                                PCx_ReportingTableOverviewModel::OVERVIEWMODE mode) const;
  PCx_ReportingWithTreeModel *selectedReporting;
  PCx_ReportingTableOverviewModel *tableOverviewModel;
  QSortFilterProxyModel *proxyModel;
  PCx_ReportingTableOverviewModel::OVERVIEWMODE getSelectedMode() const;
  unsigned int selectedNodeId;
};

#endif // FORMREPORTINGOVERVIEW_H
