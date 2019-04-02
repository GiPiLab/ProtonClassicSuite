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

#include "formauditexplore.h"
#include "ui_formauditexplore.h"
#include "utils.h"
#include <QFileDialog>
#include <QPrintDialog>
#include <QScrollBar>

FormAuditExplore::FormAuditExplore(QWidget *parent) : QWidget(parent), ui(new Ui::FormAuditExplore) {
  model = nullptr;
  ready = false;
  newAuditSelected = false;
  ui->setupUi(this);

  ui->splitter_2->setStretchFactor(1, 1);

  doc = new QTextDocument();
  ui->textEdit->setDocument(doc);

  listOfDFRFDIRIModel = MODES::getListModelOfDFRFDIRI();
  listOfPresetModel = PCx_Tables::getListModelOfAvailableAuditPresets();
  ui->comboBoxTable->setModel(listOfPresetModel);
  ui->comboBoxDFRFDIRITable->setModel(listOfDFRFDIRIModel);
  ui->comboBoxDFRFDIRIChart1->setModel(listOfDFRFDIRIModel);
  ui->comboBoxDFRFDIRIChart2->setModel(listOfDFRFDIRIModel);
  if (static_cast<PCx_Tables::PCAPRESETS>(listOfPresetModel->item(0)->data(PCx_Tables::PresetIdUserRole).toUInt()) ==
      PCx_Tables::PCAPRESETS::PCARESULTS) {
    ui->comboBoxDFRFDIRITable->setEnabled(false);
  }

  referenceNode = 1;
  updateListOfAudits();
}

FormAuditExplore::~FormAuditExplore() {
  delete doc;
  if (model != nullptr) {
    delete model;
    delete report;
  }
  delete ui;
  delete listOfPresetModel;
  delete listOfDFRFDIRIModel;
}

void FormAuditExplore::onListOfAuditsChanged() { updateListOfAudits(); }

void FormAuditExplore::onSettingsChanged() {
  if (model == nullptr) {
    return;
  }
  if (ui->treeView->currentIndex().isValid()) {
    on_treeView_clicked(ui->treeView->currentIndex());
  } else {
    QModelIndex rootIndex = model->getAttachedTree()->index(0, 0);
    on_treeView_clicked(rootIndex);
  }
}

void FormAuditExplore::updateListOfAudits() {
  ui->comboListAudits->clear();

  QList<QPair<unsigned int, QString>> listOfAudits =
      PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);
  // do not update text browser if no audit are available

  if (listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Créez et terminez un audit dans la fenêtre de gestion des audits"));
  }

  bool nonEmpty = !listOfAudits.isEmpty();
  this->setEnabled(nonEmpty);
  doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit "
                  "et n'oubliez pas de le terminer</h1>"));

  ready = nonEmpty;
  QPair<unsigned int, QString> p;
  foreach (p, listOfAudits) { ui->comboListAudits->insertItem(0, p.second, p.first); }
  ui->comboListAudits->setCurrentIndex(0);
  on_comboListAudits_activated(0);
}

void FormAuditExplore::updateViews() {
  ui->saveTablesButton->setEnabled(ready);
  if (!ready) {
    doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un "
                    "audit et n'oubliez pas de le terminer</h1>"));
    return;
  }

  unsigned int selectedNode =
      ui->treeView->selectionModel()->currentIndex().data(PCx_TreeModel::NodeIdUserRole).toUInt();

  QString output = model->generateHTMLHeader();
  output.append(model->generateHTMLAuditTitle());

  MODES::DFRFDIRI currentTablePresetMode =
      static_cast<MODES::DFRFDIRI>(ui->comboBoxDFRFDIRITable->currentData(MODES::ModeIdUserRole).toInt());

  PCx_Tables::PCAPRESETS currentTablePreset =
      static_cast<PCx_Tables::PCAPRESETS>(ui->comboBoxTable->currentData(PCx_Tables::PresetIdUserRole).toUInt());

  output.append(report->generateHTMLAuditReportForNode({currentTablePreset}, {}, {}, selectedNode,
                                                       currentTablePresetMode, referenceNode, doc));
  output.append("</body></html>");
  doc->setHtml(output);
}

QSize FormAuditExplore::sizeHint() const { return {850, 550}; }

void FormAuditExplore::on_comboListAudits_activated(int index) {
  if (index == -1 || ui->comboListAudits->count() == 0) {
    return;
  }
  unsigned int selectedAuditId = ui->comboListAudits->currentData().toUInt();

  if (model != nullptr) {
    delete model;
    delete report;
  }

  referenceNode = 1;

  model = new PCx_AuditWithTreeModel(selectedAuditId);
  report = new PCx_Report(model);

  QItemSelectionModel *m = ui->treeView->selectionModel();
  ui->treeView->setModel(model->getAttachedTree());
  delete m;

  ui->treeView->expandToDepth(1);
  QModelIndex rootIndex = model->getAttachedTree()->index(0, 0);
  newAuditSelected = true;
  ui->treeView->setCurrentIndex(rootIndex);
  on_treeView_clicked(rootIndex);
}

void FormAuditExplore::on_treeView_clicked(const QModelIndex &index) {
  Q_UNUSED(index);
  // ui->groupBoxMode->setTitle(index.data().toString());

  QScrollBar *sb = ui->textEdit->verticalScrollBar();
  int sbval = sb->value();
  updateViews();
  if (newAuditSelected == false) {

    sb->setValue(sbval);
  } else {
    sb->setValue(sb->minimum());
    newAuditSelected = false;
  }
}

void FormAuditExplore::on_saveTablesButton_clicked() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "", tr("Fichiers HTML (*.html *.htm)"));
  if (fileName.isEmpty()) {
    return;
  }

  QFileInfo fi(fileName);
  if (fi.suffix().compare("html", Qt::CaseInsensitive) != 0 && fi.suffix().compare("htm", Qt::CaseInsensitive) != 0) {
    fileName.append(".html");
  }
  fi = QFileInfo(fileName);

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    return;
  }
  // Will reopen after computation
  file.close();
  file.remove();

  unsigned int node = ui->treeView->selectionModel()->currentIndex().data(PCx_TreeModel::NodeIdUserRole).toUInt();

  PCx_Tables::PCAPRESETS currentTablePreset =
      static_cast<PCx_Tables::PCAPRESETS>(ui->comboBoxTable->currentData(PCx_Tables::PresetIdUserRole).toUInt());

  MODES::DFRFDIRI currentTablePresetMode =
      static_cast<MODES::DFRFDIRI>(ui->comboBoxDFRFDIRITable->currentData(MODES::ModeIdUserRole).toInt());

  QSettings settings;

  // Generate report in non-embedded mode, saving images
  QString output = model->generateHTMLHeader();
  output.append(model->generateHTMLAuditTitle());

  output.append(report->generateHTMLAuditReportForNode({currentTablePreset}, {}, {}, node, currentTablePresetMode,
                                                       referenceNode, nullptr, nullptr, nullptr, nullptr));
  output.append("</body></html>");

  QString settingStyle = settings.value("output/style", "CSS").toString();
  if (settingStyle == "INLINE") {
    // Pass HTML through a temp QTextDocument to reinject css into tags (more
    // compatible with text editors)
    QTextDocument formattedOut;
    formattedOut.setHtml(output);
    output = formattedOut.toHtml("utf-8");

    // Cleanup the output a bit
    output.replace(" -qt-block-indent:0;", "");
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    return;
  }
  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << output;
  stream.flush();
  file.close();

  if (stream.status() == QTextStream::Ok) {
    QMessageBox::information(this, tr("Information"),
                             tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName().toHtmlEscaped()));
  } else {
    QMessageBox::critical(this, tr("Attention"), tr("Le document n'a pas pu être enregistré !"));
  }
}

void FormAuditExplore::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormAuditExplore::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormAuditExplore::on_treeView_doubleClicked(const QModelIndex &index) {
  referenceNode = index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
  QMessageBox::information(this, "Information",
                           tr("Nouveau noeud de référence pour les calculs : %1")
                               .arg(model->getAttachedTree()->getNodeName(referenceNode).toHtmlEscaped()));
  updateViews();
}

void FormAuditExplore::on_comboBoxTable_activated(int index) {
  qDebug() << "comboTable activated";
  if (index == -1 || ui->comboBoxTable->count() == 0) {
    return;
  }

  PCx_Tables::PCAPRESETS currentTablePreset =
      static_cast<PCx_Tables::PCAPRESETS>(ui->comboBoxTable->currentData(PCx_Tables::PresetIdUserRole).toUInt());

  if (currentTablePreset == PCx_Tables::PCAPRESETS::PCARESULTS) {
    ui->comboBoxDFRFDIRITable->setEnabled(false);
  } else {
    ui->comboBoxDFRFDIRITable->setEnabled(true);
  }
  updateViews();
}

void FormAuditExplore::on_comboBoxDFRFDIRITable_activated(int index) {
  qDebug() << "comboDFRFDIRI activated";

  if (index == -1 || ui->comboBoxTable->count() == 0) {
    return;
  }
  updateViews();
}

void FormAuditExplore::on_comboBoxChart1_activated(int index) {}

void FormAuditExplore::on_comboBoxDFRFDIRIChart1_activated(int index) {}

void FormAuditExplore::on_comboBoxChart2_activated(int index) {}

void FormAuditExplore::on_comboBoxDFRFDIRIChart2_activated(int index) {}

void FormAuditExplore::showEvent(QShowEvent *ev) {
  QWidget::showEvent(ev);

  // Set scrollbar of text document to the top, as it cannot be set in treeView_clicked before the form has been shown
  // (ie, just after opening it)
  QScrollBar *sb = ui->textEdit->verticalScrollBar();
  sb->setValue(0);
}
