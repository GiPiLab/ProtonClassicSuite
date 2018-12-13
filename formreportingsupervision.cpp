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

#include "formreportingsupervision.h"
#include "ui_formreportingsupervision.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextDocument>

FormReportingSupervision::FormReportingSupervision(QWidget *parent)
    : QWidget(parent), ui(new Ui::FormReportingSupervision) {
  ui->setupUi(this);
  selectedReporting = nullptr;
  model = nullptr;
  proxyModel = nullptr;
  updateListOfReportings();
}

void FormReportingSupervision::setColumnVisibility() {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP),
                                 !ui->checkBoxBP->isChecked());
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS),
                                 !ui->checkBoxOuverts->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES),
      !ui->checkBoxRealises->isChecked());
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES),
                                 !ui->checkBoxEngages->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES),
      !ui->checkBoxDisponibles->isChecked());
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::CPP15NR),
                                 !ui->checkBoxCPP15N->isChecked());
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::RAC),
                                 !ui->checkBoxRAC->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::TAUXECART),
      !ui->checkBoxTauxEcart->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTBP),
      !ui->checkBoxVariationBP->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES),
      !ui->checkBoxTauxDisponible->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTENGAGES),
      !ui->checkBoxTauxEngage->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTREALISES),
      !ui->checkBoxTauxReal->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::NB15NRESTANTES),
      !ui->checkBox15NRest->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS),
      !ui->checkBoxDifference->isChecked());
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DECICP),
                                 !ui->checkBoxDECICO->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DERPSUR2),
      !ui->checkBoxERO2->isChecked());
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISESPREDITS),
      !ui->checkBoxRealisePredit->isChecked());
}

FormReportingSupervision::~FormReportingSupervision() {
  delete ui;
  if (proxyModel != nullptr) {
    delete proxyModel;
  }
  if (selectedReporting != nullptr) {
    delete selectedReporting;
  }
  if (model != nullptr) {
    delete model;
  }
}

void FormReportingSupervision::updateListOfReportings() {
  ui->comboListReportings->clear();

  QList<QPair<unsigned int, QString>> listOfReportings = PCx_Reporting::getListOfReportings();
  QPair<unsigned int, QString> p;
  if (listOfReportings.count() == 0) {
    QMessageBox::information(this, tr("Information"),
                             tr("Commencez par créer et ajouter des données à un reporting dans la "
                                "fenêtre de gestion des reportings"));
    setEnabled(false);
    return;
  } else {
    setEnabled(true);
  }

  foreach (p, listOfReportings) { ui->comboListReportings->insertItem(0, p.second, p.first); }
  ui->comboListReportings->setCurrentIndex(0);
  this->on_comboListReportings_activated(0);
}

void FormReportingSupervision::onListOfReportingsChanged() { updateListOfReportings(); }

void FormReportingSupervision::onReportingDataChanged(unsigned int reportingId) {
  if (selectedReporting != nullptr && selectedReporting->getReportingId() == reportingId) {
    updateDateRefCombo();
    model->setMode(getSelectedMode());
  }
}

void FormReportingSupervision::on_comboListReportings_activated(int index) {
  if (index == -1 || ui->comboListReportings->count() == 0) {
    return;
  }
  unsigned int selectedReportingId = ui->comboListReportings->currentData().toUInt();

  if (selectedReporting != nullptr) {
    delete selectedReporting;
    selectedReporting = nullptr;
  }

  if (model != nullptr) {
    delete model;
    model = nullptr;
  }

  if (proxyModel != nullptr) {
    delete proxyModel;
    proxyModel = nullptr;
  }

  selectedReporting = new PCx_Reporting(selectedReportingId);

  model = new PCx_ReportingTableSupervisionModel(selectedReporting, getSelectedMode());
  updateDateRefCombo();
  proxyModel = new QSortFilterProxyModel(this);
  // To sort numericaly when '%' is appended, instead of sorting by string
  proxyModel->setSortRole(Qt::EditRole);
  proxyModel->setSourceModel(model);

  ui->tableView->setModel(proxyModel);
  setColumnVisibility();
  ui->tableView->resizeColumnsToContents();
}

MODES::DFRFDIRI FormReportingSupervision::getSelectedMode() const {
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

  } else {
    qWarning() << "Invalid selection";
  }
  return MODES::DFRFDIRI::DF;
}

void FormReportingSupervision::on_radioButtonDF_toggled(bool checked) {
  if (checked) {
    model->setMode(MODES::DFRFDIRI::DF);
    ui->tableView->resizeColumnsToContents();
    updateDateRefCombo();
  }
}

void FormReportingSupervision::on_radioButtonRF_toggled(bool checked) {
  if (checked) {
    model->setMode(MODES::DFRFDIRI::RF);
    ui->tableView->resizeColumnsToContents();
    updateDateRefCombo();
  }
}

void FormReportingSupervision::on_radioButtonDI_toggled(bool checked) {
  if (checked) {
    model->setMode(MODES::DFRFDIRI::DI);
    ui->tableView->resizeColumnsToContents();
    updateDateRefCombo();
  }
}

void FormReportingSupervision::on_radioButtonRI_toggled(bool checked) {
  if (checked) {
    model->setMode(MODES::DFRFDIRI::RI);
    ui->tableView->resizeColumnsToContents();
    updateDateRefCombo();
  }
}

void FormReportingSupervision::on_checkBoxBP_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::BP),
                                 !checked);
}

void FormReportingSupervision::on_checkBoxOuverts_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::OUVERTS),
                                 !checked);
}

void FormReportingSupervision::on_checkBoxRealises_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISES), !checked);
}

void FormReportingSupervision::on_checkBoxEngages_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::ENGAGES),
                                 !checked);
}

void FormReportingSupervision::on_checkBoxDisponibles_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DISPONIBLES), !checked);
}

void FormReportingSupervision::on_checkBoxTauxReal_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTREALISES), !checked);
}

void FormReportingSupervision::on_checkBoxTauxEcart_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::TAUXECART), !checked);
}

void FormReportingSupervision::on_checkBoxVariationBP_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTBP), !checked);
}

void FormReportingSupervision::on_checkBoxTauxEngage_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTENGAGES), !checked);
}

void FormReportingSupervision::on_checkBoxTauxDisponible_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES), !checked);
}

void FormReportingSupervision::on_checkBoxRealisePredit_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::REALISESPREDITS), !checked);
}

void FormReportingSupervision::on_checkBoxDifference_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS),
      !checked);
}

void FormReportingSupervision::on_checkBoxDECICO_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DECICP),
                                 !checked);
}

void FormReportingSupervision::on_checkBoxERO2_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::DERPSUR2), !checked);
}

void FormReportingSupervision::on_checkBoxRAC_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::RAC),
                                 !checked);
}

void FormReportingSupervision::on_checkBox15NRest_toggled(bool checked) {
  ui->tableView->setColumnHidden(
      static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::NB15NRESTANTES), !checked);
}

void FormReportingSupervision::on_checkBoxCPP15N_toggled(bool checked) {
  ui->tableView->setColumnHidden(static_cast<int>(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS::CPP15NR),
                                 !checked);
}

void FormReportingSupervision::on_pushButtonExportHTML_clicked() {
  QString out = selectedReporting->generateHTMLReportingTitle();

  out.append(QString("<p>%1</p>").arg(MODES::modeToCompleteString(getSelectedMode())));

  out.append(ui->textEdit->toHtml());

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

  QTextDocument doc;
  doc.setHtml(out);

  QString output = doc.toHtml("utf-8");
  output.replace(" -qt-block-indent:0;", "");

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

QSize FormReportingSupervision::sizeHint() const { return {900, 500}; }

void FormReportingSupervision::updateDateRefCombo() {
  ui->comboBoxListDates->clear();
  QList<QDate> listDates = selectedReporting->getDatesForNodeAndMode(1, getSelectedMode()).toList();
  qSort(listDates.begin(), listDates.end(), qGreater<QDate>());

  ui->comboBoxListDates->addItem("Dernière situation", -1);
  foreach (const QDate &date, listDates) {
    ui->comboBoxListDates->addItem(date.toString(Qt::DefaultLocaleShortDate), QDateTime(date).toTime_t());
  }

  if (model != nullptr && selectedReporting != nullptr) {
    time_t currentDateTimeT = model->getSelectedDateTimeT();
    if (currentDateTimeT != -1) {
      if (selectedReporting->dateExistsForNodeAndMode(currentDateTimeT, 1, getSelectedMode())) {
        for (int i = 0; i < ui->comboBoxListDates->count(); i++) {
          if (ui->comboBoxListDates->itemData(i).toInt() == currentDateTimeT) {
            ui->comboBoxListDates->setCurrentIndex(i);
            on_comboBoxListDates_activated(i);
            break;
          }
        }
      } else {
        ui->comboBoxListDates->setCurrentIndex(0);
        on_comboBoxListDates_activated(0);
      }
    }
  }
}

void FormReportingSupervision::on_pushButtonSelectNone_clicked() {
  ui->checkBox15NRest->setChecked(false);
  ui->checkBoxBP->setChecked(false);
  ui->checkBoxCPP15N->setChecked(false);
  ui->checkBoxDECICO->setChecked(false);
  ui->checkBoxDifference->setChecked(false);
  ui->checkBoxDisponibles->setChecked(false);
  ui->checkBoxEngages->setChecked(false);
  ui->checkBoxERO2->setChecked(false);
  ui->checkBoxDECICO->setChecked(false);
  ui->checkBoxTauxDisponible->setChecked(false);
  ui->checkBoxTauxEcart->setChecked(false);
  ui->checkBoxTauxEngage->setChecked(false);
  ui->checkBoxTauxReal->setChecked(false);
  ui->checkBoxRAC->setChecked(false);
  ui->checkBoxRealisePredit->setChecked(false);
  ui->checkBoxRealises->setChecked(false);
  ui->checkBoxOuverts->setChecked(false);
  ui->checkBoxVariationBP->setChecked(false);
}

void FormReportingSupervision::on_pushButtonSelectAll_clicked() {
  ui->checkBox15NRest->setChecked(true);
  ui->checkBoxBP->setChecked(true);
  ui->checkBoxCPP15N->setChecked(true);
  ui->checkBoxDECICO->setChecked(true);
  ui->checkBoxDifference->setChecked(true);
  ui->checkBoxDisponibles->setChecked(true);
  ui->checkBoxEngages->setChecked(true);
  ui->checkBoxERO2->setChecked(true);
  ui->checkBoxDECICO->setChecked(true);
  ui->checkBoxTauxDisponible->setChecked(true);
  ui->checkBoxTauxEcart->setChecked(true);
  ui->checkBoxTauxEngage->setChecked(true);
  ui->checkBoxTauxReal->setChecked(true);
  ui->checkBoxRAC->setChecked(true);
  ui->checkBoxRealisePredit->setChecked(true);
  ui->checkBoxRealises->setChecked(true);
  ui->checkBoxOuverts->setChecked(true);
  ui->checkBoxVariationBP->setChecked(true);
}

void FormReportingSupervision::on_comboBoxListDates_activated(int index) {
  Q_UNUSED(index);
  if (ui->comboBoxListDates->count() == 0) {
    return;
  }
  time_t selectedDate = ui->comboBoxListDates->currentData().toInt();
  model->setDateTimeT(selectedDate);

  ui->tableView->resizeColumnsToContents();
}

void FormReportingSupervision::on_pushButtonCopy_clicked() {
  QTextCursor cursor = ui->textEdit->textCursor();
  cursor.insertHtml("<p>" + MODES::modeToCompleteString(getSelectedMode()) + "</p>");
  cursor.insertHtml(qTableViewToHtml(ui->tableView) + "<br>");
}

void FormReportingSupervision::on_pushButtonClear_clicked() { ui->textEdit->clear(); }
