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

#ifndef FORMAUDITPREVISIONS_H
#define FORMAUDITPREVISIONS_H

#include "pcx_graphics.h"
#include "pcx_prevision.h"
#include "pcx_previsionitemtablemodel.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class FormAuditPrevisions;
}

class FormAuditPrevisions : public QWidget {
  Q_OBJECT

public:
  explicit FormAuditPrevisions(QWidget *parent = nullptr);
  ~FormAuditPrevisions();

public slots:
  void onListOfPrevisionsChanged();
  void onSettingsChanged();

signals:
  void previsionUpdated(unsigned int previsionId);

private slots:
  void on_treeView_clicked(const QModelIndex &index);

  void on_comboListPrevisions_activated(int index);
  void on_comboBoxOperators_activated(int index);

  void onAnchorClicked(QUrl url);

  void on_pushButtonAddCriteriaToAdd_clicked();

  void on_pushButtonAddCriteriaToSubstract_clicked();

  void on_pushButtonDelCriteria_clicked();

  void on_radioButtonDF_toggled(bool checked);

  void on_radioButtonRF_toggled(bool checked);

  void on_radioButtonDI_toggled(bool checked);

  void on_radioButtonRI_toggled(bool checked);

  void on_pushButtonDeleteAll_clicked();

  void on_pushButtonApplyToNode_clicked();

  void on_pushButtonApplyToLeaves_clicked();

  void on_checkBoxDisplayLeafCriteria_toggled(bool checked);

  void on_pushButtonDisplayReport_clicked();

  void on_pushButtonSaveBigReport_clicked();

  void on_checkBoxShowCriteriaToApply_toggled(bool checked);

  void on_pushButtonExpandAll_clicked();

  void on_pushButtonCollapseAll_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

private:
  PCx_Prevision *previsionModel;
  PCx_AuditWithTreeModel *auditWithTreeModel;
  PCx_PrevisionItem *currentPrevisionItem, *recentPrevisionItem;
  PCx_PrevisionItemTableModel *currentPrevisionItemTableModel, *recentPrevisionItemTableModel;

  MODES::DFRFDIRI currentMode;
  unsigned int currentNodeId;
  Ui::FormAuditPrevisions *ui;
  QSize sizeHint() const;
  PCx_Graphics *graphics;

  unsigned int referenceNode;

  void updateListOfPrevisions();
  void updatePrevisionItemTableModel();
  void updateLabels();
};

#endif // FORMAUDITPREVISIONS_H
