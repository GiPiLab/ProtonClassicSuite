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

#include "formreportingexplore.h"
#include "pcx_report.h"
#include "ui_formreportingexplore.h"
#include "utils.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

FormReportingExplore::FormReportingExplore(QWidget *parent) : QWidget(parent), ui(new Ui::FormReportingExplore) {
  ui->setupUi(this);
  selectedReporting = nullptr;
  document = new QTextDocument();
  report = nullptr;
  updateListOfReportings();
  ui->splitter->setStretchFactor(1, 1);
}

FormReportingExplore::~FormReportingExplore() {
  delete selectedReporting;
  delete report;
  delete document;
  delete ui;
}

void FormReportingExplore::onListOfReportingsChanged() { updateListOfReportings(); }

void FormReportingExplore::onReportingDataChanged(unsigned int reportingId) {
  if (selectedReporting != nullptr && selectedReporting->getReportingId() == reportingId) {
    if (ui->treeView->currentIndex().isValid()) {
      on_treeView_clicked(ui->treeView->currentIndex());
    } else {
      QModelIndex rootIndex = selectedReporting->getAttachedTree()->index(0, 0);
      on_treeView_clicked(rootIndex);
    }
  }
}

void FormReportingExplore::onSettingsChanged() {
  if (selectedReporting == nullptr) {
    return;
  }
  if (ui->treeView->currentIndex().isValid()) {
    on_treeView_clicked(ui->treeView->currentIndex());
  } else {
    QModelIndex rootIndex = selectedReporting->getAttachedTree()->index(0, 0);
    on_treeView_clicked(rootIndex);
  }
}

void FormReportingExplore::updateListOfReportings() {
  ui->comboListReportings->clear();

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

  foreach (p, listOfReportings) { ui->comboListReportings->insertItem(0, p.second, p.first); }
  ui->comboListReportings->setCurrentIndex(0);
  this->on_comboListReportings_activated(0);
}

QSize FormReportingExplore::sizeHint() const { return {1000, 500}; }

void FormReportingExplore::on_comboListReportings_activated(int index) {
  if (index == -1 || ui->comboListReportings->count() == 0) {
    return;
  }
  unsigned int selectedReportingId = ui->comboListReportings->currentData().toUInt();

  delete selectedReporting;

  selectedReporting = new PCx_ReportingWithTreeModel(selectedReportingId);

  delete report;

  QSettings settings;
  int graphicsWidth = settings.value("graphics/width", PCx_Graphics::DEFAULTWIDTH).toInt();
  int graphicsHeight = settings.value("graphics/height", PCx_Graphics::DEFAULTHEIGHT).toInt();

  report = new PCx_Report(selectedReporting, graphicsWidth, graphicsHeight);

  document->clear();

  ui->treeView->setModel(selectedReporting->getAttachedTree());
  ui->treeView->expandToDepth(1);
  QModelIndex rootIndex = selectedReporting->getAttachedTree()->index(0, 0);
  selectedNode = 1;
  ui->treeView->setCurrentIndex(rootIndex);
  on_treeView_clicked(rootIndex);
}

QList<PCx_Report::PCRPRESETS> FormReportingExplore::getPresets() const {
  QList<PCx_Report::PCRPRESETS> presets;
  if (ui->checkBoxA->isChecked()) {
    presets.append(PCx_Report::PCRPRESETS::PCRPRESET_A);
  }
  if (ui->checkBoxB->isChecked()) {
    presets.append(PCx_Report::PCRPRESETS::PCRPRESET_B);
  }
  if (ui->checkBoxC->isChecked()) {
    presets.append(PCx_Report::PCRPRESETS::PCRPRESET_C);
  }
  if (ui->checkBoxD->isChecked()) {
    presets.append(PCx_Report::PCRPRESETS::PCRPRESET_D);
  }
  if (ui->checkBoxS->isChecked()) {
    presets.append(PCx_Report::PCRPRESETS::PCRPRESET_S);
  }
  return presets;
}

MODES::DFRFDIRI FormReportingExplore::getSelectedMode() const {
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

void FormReportingExplore::updateTextEdit() {
  QScrollBar *sb = ui->textEdit->verticalScrollBar();
  int sbval = sb->value();

  QString output = PCx_Report::generateMainHTMLHeader() + selectedReporting->generateHTMLReportingTitle();
  document->clear();

  output.append(
      QString("<h1>%1</h1>").arg(selectedReporting->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped()));

  output.append(
      report->generateHTMLReportingReportForNode(getPresets(), selectedNode, getSelectedMode(), true, document));
  document->setHtml(output);

  ui->textEdit->setDocument(document);
  sb->setValue(sbval);
}

void FormReportingExplore::on_treeView_clicked(const QModelIndex &index) {
  unsigned int nodeId = index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
  selectedNode = nodeId;
  updateTextEdit();
}

void FormReportingExplore::on_checkBoxA_toggled(bool checked) {
  Q_UNUSED(checked);
  updateTextEdit();
}

void FormReportingExplore::on_checkBoxB_toggled(bool checked) {
  Q_UNUSED(checked);
  updateTextEdit();
}

void FormReportingExplore::on_checkBoxC_toggled(bool checked) {
  Q_UNUSED(checked);
  updateTextEdit();
}

void FormReportingExplore::on_checkBoxD_toggled(bool checked) {
  Q_UNUSED(checked);
  updateTextEdit();
}

void FormReportingExplore::on_checkBoxS_toggled(bool checked) {
  Q_UNUSED(checked);
  updateTextEdit();
}

void FormReportingExplore::on_radioButtonDF_toggled(bool checked) {
  if (checked) {
    updateTextEdit();
  }
}

void FormReportingExplore::on_radioButtonRF_toggled(bool checked) {
  if (checked) {
    updateTextEdit();
  }
}

void FormReportingExplore::on_radioButtonDI_toggled(bool checked) {
  if (checked) {
    updateTextEdit();
  }
}

void FormReportingExplore::on_radioButtonRI_toggled(bool checked) {
  if (checked) {
    updateTextEdit();
  }
}

void FormReportingExplore::on_pushButtonExport_clicked() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName = chooseHTMLFileNameWithDialog();
  if(fileName.isEmpty())return;

  QFile file(fileName);

  QFileInfo fi(fileName);
  QString relativeImagePath = fi.fileName() + "_files";
  QString absoluteImagePath = fi.absoluteFilePath() + "_files";

  QFileInfo imageDirInfo(absoluteImagePath);

  if (!imageDirInfo.exists()) {
    if (!fi.absoluteDir().mkdir(relativeImagePath)) {
      QMessageBox::critical(this, tr("Attention"), tr("Création du dossier des noeuds impossible"));
      return;
    }
  } else {
    if (!imageDirInfo.isWritable()) {
      QMessageBox::critical(this, tr("Attention"), tr("Ecriture impossible dans le dossier des noeuds"));
      return;
    }
  }

  QSettings settings;

  report->getGraphics().setGraphicsWidth(settings.value("graphics/width", PCx_Graphics::DEFAULTWIDTH).toInt());
  report->getGraphics().setGraphicsHeight(settings.value("graphics/height", PCx_Graphics::DEFAULTHEIGHT).toInt());

  // Generate report in non-embedded mode, saving images
  QString output = PCx_Report::generateMainHTMLHeader();
  output.append(selectedReporting->generateHTMLReportingTitle());
  output.append(QString("<h1 id='node%2'>%1</h1>")
                    .arg(selectedReporting->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped())
                    .arg(selectedNode));
  output.append(report->generateHTMLReportingReportForNode(getPresets(), selectedNode, getSelectedMode(), true, nullptr));
  output.append("</body></html>");

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    QDir dir(absoluteImagePath);
    dir.removeRecursively();
    return;
  }
  QTextStream stream(&file);
  stream << output;
  stream.flush();
  file.close();
  if (stream.status() == QTextStream::Ok) {
    QMessageBox::information(this, tr("Information"),
                             tr("Le document <b>%1</b> a bien été enregistré. Les noeuds sont "
                                "stockés dans le dossier <b>%2</b>")
                                 .arg(fileName.toHtmlEscaped(), relativeImagePath.toHtmlEscaped()));
  } else {
    QMessageBox::critical(this, tr("Attention"), tr("Le document n'a pas pu être enregistré !"));
  }
}

void FormReportingExplore::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(1);
}

void FormReportingExplore::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }
