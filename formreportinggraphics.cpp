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

#include "formreportinggraphics.h"
#include "ui_formreportinggraphics.h"
#include <QFileDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

FormReportingGraphics::FormReportingGraphics(QWidget *parent) : QWidget(parent), ui(new Ui::FormReportingGraphics) {
  ui->setupUi(this);
  selectedReporting = nullptr;
  graphics = nullptr;
  // Root ID
  selectedNodeId = 1;
  ui->plot->setRenderHint(QPainter::RenderHint::Antialiasing, true);
  ui->splitter->setStretchFactor(1, 1);
  updateListOfReportings();
}

FormReportingGraphics::~FormReportingGraphics() {
  delete ui;
  delete selectedReporting;
  delete graphics;
}

void FormReportingGraphics::onListOfReportingsChanged() { updateListOfReportings(); }

void FormReportingGraphics::onReportingDataChanged(unsigned int reportingId) {
  if (selectedReporting != nullptr && selectedReporting->getReportingId() == reportingId) {
    if (ui->treeView->currentIndex().isValid()) {
      on_treeView_clicked(ui->treeView->currentIndex());
    } else {
      QModelIndex rootIndex = selectedReporting->getAttachedTree()->index(0, 0);
      on_treeView_clicked(rootIndex);
    }
  }
}

void FormReportingGraphics::updateListOfReportings() {
  ui->comboListOfReportings->clear();

  QList<QPair<unsigned int, QString>> listOfReportings = PCx_Reporting::getListOfReportings();
  QPair<unsigned int, QString> p;
  if (listOfReportings.count() == 0) {
    QMessageBox::information(this, tr("Information"),
                             tr("Commencez par créer et ajouter des données à un reporting dans la "
                                "fenêtre de gestion des reportings"));
    setEnabled(false);
    return;
  }
  setEnabled(true);

  foreach (p, listOfReportings) { ui->comboListOfReportings->insertItem(0, p.second, p.first); }
  ui->comboListOfReportings->setCurrentIndex(0);
  this->on_comboListOfReportings_activated(0);
}

void FormReportingGraphics::on_comboListOfReportings_activated(int index) {
  if (index == -1 || ui->comboListOfReportings->count() == 0) {
    return;
  }
  unsigned int selectedReportingId = ui->comboListOfReportings->currentData().toUInt();

  delete selectedReporting;
  selectedReporting = nullptr;
  delete graphics;
  graphics = nullptr;

  selectedReporting = new PCx_ReportingWithTreeModel(selectedReportingId);
  graphics = new PCx_Graphics(selectedReporting);
  ui->treeView->setModel(selectedReporting->getAttachedTree());
  ui->treeView->expandToDepth(1);
  QModelIndex rootIndex = selectedReporting->getAttachedTree()->index(0, 0);
  selectedNodeId = 1;
  ui->treeView->setCurrentIndex(rootIndex);
  on_treeView_clicked(rootIndex);
}

void FormReportingGraphics::on_treeView_clicked(const QModelIndex &index) {
  unsigned int nodeId = index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
  selectedNodeId = nodeId;
  updatePlot();
}

MODES::DFRFDIRI FormReportingGraphics::getSelectedMode() const {
  if (ui->radioButtonDF->isChecked()) {
    return MODES::DFRFDIRI::DF;
  }
  if (ui->radioButtonRF->isChecked()) {
    return MODES::DFRFDIRI::RF;
  }
  if (ui->radioButtonDI->isChecked()) {
    return MODES::DFRFDIRI::DI;
  }
  if (ui->radioButtonRI->isChecked()) {
    return MODES::DFRFDIRI::RI;
  }

  qWarning() << "Invalid selection";
  return MODES::DFRFDIRI::DF;
}

QList<PCx_Reporting::OREDPCR> FormReportingGraphics::getSelectedOREDPCR() const {
  QList<PCx_Reporting::OREDPCR> selection;
  if (ui->checkBoxBP->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::BP);
  }
  if (ui->checkBoxReports->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::REPORTS);
  }
  if (ui->checkBoxOCDM->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::OCDM);
  }
  if (ui->checkBoxVCDM->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::VCDM);
  }
  if (ui->checkBoxVInternes->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::VIREMENTSINTERNES);
  }
  if (ui->checkBoxBudgetVote->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::BUDGETVOTE);
  }
  if (ui->checkBoxRattachesN1->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::RATTACHENMOINS1);
  }
  if (ui->checkBoxOuverts->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::OUVERTS);
  }
  if (ui->checkBoxRealises->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::REALISES);
  }
  if (ui->checkBoxEngages->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::ENGAGES);
  }
  if (ui->checkBoxDisponibles->isChecked()) {
    selection.append(PCx_Reporting::OREDPCR::DISPONIBLES);
  }
  return selection;
}

/**
 * @brief FormReportingGraphics::updatePlot draw the plot with selected datas
 */
void FormReportingGraphics::updatePlot() {

  QChart *oldChart = ui->plot->chart();
  ui->plot->setChart(graphics->getPCRHistoryChart(selectedNodeId, getSelectedMode(), getSelectedOREDPCR()));
  delete oldChart;
}

void FormReportingGraphics::on_radioButtonDF_toggled(bool checked) {
  if (checked) {
    updatePlot();
  }
}

void FormReportingGraphics::on_radioButtonRF_toggled(bool checked) {
  if (checked) {
    updatePlot();
  }
}

void FormReportingGraphics::on_radioButtonDI_toggled(bool checked) {
  if (checked) {
    updatePlot();
  }
}

void FormReportingGraphics::on_radioButtonRI_toggled(bool checked) {
  if (checked) {
    updatePlot();
  }
}

void FormReportingGraphics::on_checkBoxBP_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxReports_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxOCDM_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxVCDM_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxBudgetVote_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxVInternes_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxRattachesN1_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxOuverts_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxRealises_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxEngages_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

void FormReportingGraphics::on_checkBoxDisponibles_toggled(bool checked) {
  Q_UNUSED(checked);
  updatePlot();
}

QSize FormReportingGraphics::sizeHint() const { return {800, 500}; }

void FormReportingGraphics::on_pushButtonExportPlot_clicked() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en PNG"), "", tr("Images PNG (*.png)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFileInfo fi(fileName);
  if (fi.suffix().compare("png", Qt::CaseInsensitive) != 0) {
    fileName.append(".png");
  }
  fi = QFileInfo(fileName);

  if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
    QMessageBox::critical(this, tr("Attention"), tr("Fichier non accessible en écriture"));
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    return;
  }
  file.close();

  if (PCx_Graphics::savePixmapToDisk(PCx_Graphics::chartViewToPixmap(ui->plot), fileName) == true) {
    QMessageBox::information(this, tr("Information"), tr("Image enregistrée"));
  } else {
    QMessageBox::critical(this, tr("Attention"), tr("Image non enregistrée !"));
  }
}

void FormReportingGraphics::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormReportingGraphics::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}
