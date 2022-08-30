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

#include "formqueries.h"
#include "pcx_query.h"
#include "pcx_queryminmax.h"
#include "pcx_queryrank.h"
#include "pcx_queryvariation.h"
#include "ui_formqueries.h"
#include "utils.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace NUMBERSFORMAT;

FormQueries::FormQueries(QWidget *parent) : QWidget(parent), ui(new Ui::FormQueries) {
  ui->setupUi(this);
  model = nullptr;
  queriesModel = nullptr;
  report = nullptr;

  onColorChanged();

  ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS, true),
                            static_cast<int>(PCx_Audit::ORED::OUVERTS));
  ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES, true),
                            static_cast<int>(PCx_Audit::ORED::REALISES));
  ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES, true),
                            static_cast<int>(PCx_Audit::ORED::ENGAGES));
  ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES, true),
                            static_cast<int>(PCx_Audit::ORED::DISPONIBLES));

  ui->comboBoxDFRFDIRI->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toLower(),
                                static_cast<int>(MODES::DFRFDIRI::DF));
  ui->comboBoxDFRFDIRI->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toLower(),
                                static_cast<int>(MODES::DFRFDIRI::RF));
  ui->comboBoxDFRFDIRI->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toLower(),
                                static_cast<int>(MODES::DFRFDIRI::DI));
  ui->comboBoxDFRFDIRI->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toLower(),
                                static_cast<int>(MODES::DFRFDIRI::RI));

  ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::INCREASEDECREASE::INCREASE),
                              static_cast<int>(PCx_QueryVariation::INCREASEDECREASE::INCREASE));
  ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::INCREASEDECREASE::DECREASE),
                              static_cast<int>(PCx_QueryVariation::INCREASEDECREASE::DECREASE));
  ui->comboBoxAugDim->addItem(PCx_QueryVariation::incDecToString(PCx_QueryVariation::INCREASEDECREASE::VARIATION),
                              static_cast<int>(PCx_QueryVariation::INCREASEDECREASE::VARIATION));

  ui->comboBoxPercentEuro->addItem(
      PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::PERCENTORPOINTS::PERCENT),
      static_cast<int>(PCx_QueryVariation::PERCENTORPOINTS::PERCENT));
  ui->comboBoxPercentEuro->addItem(
      PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::PERCENTORPOINTS::POINTS),
      static_cast<int>(PCx_QueryVariation::PERCENTORPOINTS::POINTS));

  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::LOWERTHAN),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::LOWERTHAN));
  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::LOWEROREQUAL),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::LOWEROREQUAL));
  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::EQUAL),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::EQUAL));
  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::NOTEQUAL),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::NOTEQUAL));
  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::GREATEROREQUAL),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::GREATEROREQUAL));
  ui->comboBoxOperator->addItem(PCx_QueryVariation::operatorToString(PCx_QueryVariation::OPERATORS::GREATERTHAN),
                                static_cast<int>(PCx_QueryVariation::OPERATORS::GREATERTHAN));

  ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS, true),
                              static_cast<int>(PCx_Audit::ORED::OUVERTS));
  ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES, true),
                              static_cast<int>(PCx_Audit::ORED::REALISES));
  ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES, true),
                              static_cast<int>(PCx_Audit::ORED::ENGAGES));
  ui->comboBoxORED_2->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES, true),
                              static_cast<int>(PCx_Audit::ORED::DISPONIBLES));

  ui->comboBoxDFRFDIRI_2->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::DF));
  ui->comboBoxDFRFDIRI_2->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::RF));
  ui->comboBoxDFRFDIRI_2->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::DI));
  ui->comboBoxDFRFDIRI_2->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::RI));

  ui->comboBoxBiggerSmaller->addItem(PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::GREATERSMALLER::GREATER),
                                     static_cast<int>(PCx_QueryRank::GREATERSMALLER::GREATER));
  ui->comboBoxBiggerSmaller->addItem(PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::GREATERSMALLER::SMALLER),
                                     static_cast<int>(PCx_QueryRank::GREATERSMALLER::SMALLER));

  ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS, true),
                              static_cast<int>(PCx_Audit::ORED::OUVERTS));
  ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES, true),
                              static_cast<int>(PCx_Audit::ORED::REALISES));
  ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES, true),
                              static_cast<int>(PCx_Audit::ORED::ENGAGES));
  ui->comboBoxORED_3->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES, true),
                              static_cast<int>(PCx_Audit::ORED::DISPONIBLES));

  ui->comboBoxDFRFDIRI_3->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DF).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::DF));
  ui->comboBoxDFRFDIRI_3->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RF).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::RF));
  ui->comboBoxDFRFDIRI_3->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::DI).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::DI));
  ui->comboBoxDFRFDIRI_3->addItem(MODES::modeToCompleteString(MODES::DFRFDIRI::RI).toLower(),
                                  static_cast<int>(MODES::DFRFDIRI::RI));

  ui->doubleSpinBox->setMaximum(INT64_MAX / FIXEDPOINTCOEFF);
  ui->doubleSpinBox_2->setRange(-INT64_MAX / FIXEDPOINTCOEFF, INT64_MAX / FIXEDPOINTCOEFF);
  ui->doubleSpinBox_3->setRange(-INT64_MAX / FIXEDPOINTCOEFF, INT64_MAX / FIXEDPOINTCOEFF);

  doc = new QTextDocument();
  xlsDoc = nullptr;
  ui->textEdit->setDocument(doc);
  updateListOfAudits();
}

FormQueries::~FormQueries() {
  delete model;
  delete report;
  delete queriesModel;
  doc->clear();
  delete doc;
  delete xlsDoc;
  delete ui;
}

void FormQueries::onListOfAuditsChanged() { updateListOfAudits(); }

void FormQueries::onColorChanged() {
  ui->frame->setStyleSheet(QString("background-color:" + PCx_QueryVariation::getColor().name()));
  ui->frame_2->setStyleSheet(QString("background-color:" + PCx_QueryRank::getColor().name()));
  ui->frame_3->setStyleSheet(QString("background-color:" + PCx_QueryMinMax::getColor().name()));
}

void FormQueries::onListOfQueriesChanged(unsigned int auditId) {
  if (model == nullptr || queriesModel == nullptr) {
    return;
  }

  if (model->getAuditId() == auditId) {
    queriesModel->update();
  }
}

void FormQueries::updateListOfAudits() {
  ui->comboBoxListAudits->clear();

  QList<QPair<unsigned int, QString>> listOfAudits =
      PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);

  if (listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Créez et terminez un audit dans la fenêtre de gestion des audits"));
  }

  // do not update text browser if no audit are available
  bool nonEmpty = !listOfAudits.isEmpty();
  this->setEnabled(nonEmpty);
  if (!nonEmpty) {
    doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un "
                    "audit et n'oubliez pas de le terminer</h1>"));
  }

  QPair<unsigned int, QString> p;
  foreach (p, listOfAudits) { ui->comboBoxListAudits->insertItem(0, p.second, p.first); }
  ui->comboBoxListAudits->setCurrentIndex(0);
  on_comboBoxListAudits_activated(0);
}

void FormQueries::on_comboBoxListAudits_activated(int index) {
  if (index == -1 || ui->comboBoxListAudits->count() == 0) {
    return;
  }
  unsigned int selectedAuditId = ui->comboBoxListAudits->currentData().toUInt();
  if (selectedAuditId == 0) {
    qFatal("Assertion failed");
  }
  // qDebug()<<"Selected audit ID = "<<selectedAuditId;

  delete model;
  delete report;
  delete queriesModel;

  model = new PCx_Audit(selectedAuditId);
  report = new PCx_Report(model);
  queriesModel = new PCx_QueriesModel(model, this);

  ui->listView->setModel(queriesModel);

  ui->comboBoxTypes->clear();
  ui->comboBoxTypes_2->clear();
  ui->comboBoxTypes_3->clear();
  QList<QPair<unsigned int, QString>> listOfTypes = model->getAttachedTree()->getAllTypes();
  if (listOfTypes.isEmpty()) {
    qFatal("Assertion failed");
  }
  QPair<unsigned int, QString> p;
  foreach (p, listOfTypes) {
    ui->comboBoxTypes->addItem(p.second, p.first);
    ui->comboBoxTypes_2->addItem(p.second, p.first);
    ui->comboBoxTypes_3->addItem(p.second, p.first);
  }
  ui->comboBoxTypes->insertItem(0, tr("[noeuds]"), PCx_Query::ALLTYPES);
  ui->comboBoxTypes_2->insertItem(0, tr("[noeuds]"), PCx_Query::ALLTYPES);
  ui->comboBoxTypes_3->insertItem(0, tr("[noeuds]"), PCx_Query::ALLTYPES);

  ui->comboBoxTypes->setCurrentIndex(0);
  ui->comboBoxTypes_2->setCurrentIndex(0);
  ui->comboBoxTypes_3->setCurrentIndex(0);

  ui->comboBoxYear1->clear();
  ui->comboBoxYear2->clear();

  QStringList yearsStringList = model->getYearsStringList();
  ui->comboBoxYear1->addItems(yearsStringList);
  ui->comboBoxYear2->addItems(yearsStringList);
  ui->comboBoxYear1->setCurrentIndex(0);
  ui->comboBoxYear2->setCurrentIndex(ui->comboBoxYear2->count() - 1);

  ui->comboBoxYear1_2->clear();
  ui->comboBoxYear2_2->clear();
  ui->comboBoxYear1_2->addItems(yearsStringList);
  ui->comboBoxYear2_2->addItems(yearsStringList);
  ui->comboBoxYear1_2->setCurrentIndex(0);
  ui->comboBoxYear2_2->setCurrentIndex(ui->comboBoxYear2_2->count() - 1);

  ui->comboBoxYear1_3->clear();
  ui->comboBoxYear2_3->clear();
  ui->comboBoxYear1_3->addItems(yearsStringList);
  ui->comboBoxYear2_3->addItems(yearsStringList);
  ui->comboBoxYear1_3->setCurrentIndex(0);
  ui->comboBoxYear2_3->setCurrentIndex(ui->comboBoxYear2_3->count() - 1);
  doc->clear();
}

bool FormQueries::getParamsReqRank(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri,
                                   unsigned int &number, PCx_QueryRank::GREATERSMALLER &grSm, int &year1, int &year2) {
  typeId = ui->comboBoxTypes_2->currentData().toUInt();
  ored = static_cast<PCx_Audit::ORED>(ui->comboBoxORED_2->currentData().toUInt());
  dfrfdiri = static_cast<MODES::DFRFDIRI>(ui->comboBoxDFRFDIRI_2->currentData().toUInt());
  grSm = static_cast<PCx_QueryRank::GREATERSMALLER>(ui->comboBoxBiggerSmaller->currentData().toUInt());
  number = static_cast<unsigned int>(ui->spinBoxNumRank->value());
  year1 = ui->comboBoxYear1_2->currentText().toInt();
  year2 = ui->comboBoxYear2_2->currentText().toInt();
  return true;
}

bool FormQueries::getParamsReqMinMax(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri,
                                     qint64 &val1, qint64 &val2, int &year1, int &year2) {
  typeId = ui->comboBoxTypes_3->currentData().toUInt();
  ored = static_cast<PCx_Audit::ORED>(ui->comboBoxORED_3->currentData().toUInt());
  dfrfdiri = static_cast<MODES::DFRFDIRI>(ui->comboBoxDFRFDIRI_3->currentData().toUInt());
  val1 = doubleToFixedPoint(ui->doubleSpinBox_2->value());
  val2 = doubleToFixedPoint(ui->doubleSpinBox_3->value());
  year1 = ui->comboBoxYear1_3->currentText().toInt();
  year2 = ui->comboBoxYear2_3->currentText().toInt();
  return true;
}

bool FormQueries::getParamsReqVariation(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri,
                                        PCx_QueryVariation::INCREASEDECREASE &increase,
                                        PCx_QueryVariation::PERCENTORPOINTS &percent,
                                        PCx_QueryVariation::OPERATORS &oper, qint64 &val, int &year1, int &year2) {
  typeId = ui->comboBoxTypes->currentData().toUInt();
  ored = static_cast<PCx_Audit::ORED>(ui->comboBoxORED->currentData().toUInt());
  dfrfdiri = static_cast<MODES::DFRFDIRI>(ui->comboBoxDFRFDIRI->currentData().toUInt());
  increase = static_cast<PCx_QueryVariation::INCREASEDECREASE>(ui->comboBoxAugDim->currentData().toUInt());
  percent = static_cast<PCx_QueryVariation::PERCENTORPOINTS>(ui->comboBoxPercentEuro->currentData().toUInt());
  oper = static_cast<PCx_QueryVariation::OPERATORS>(ui->comboBoxOperator->currentData().toUInt());
  val = doubleToFixedPoint(ui->doubleSpinBox->value());
  year1 = ui->comboBoxYear1->currentText().toInt();
  year2 = ui->comboBoxYear2->currentText().toInt();
  if (year1 == year2) {
    QMessageBox::warning(this, tr("Attention"), tr("Les années doivent être différentes !"));
    return false;
  }
  return true;
}

void FormQueries::on_pushButtonExecReqVariation_clicked() {
  unsigned int typeId;
  PCx_Audit::ORED ored;
  MODES::DFRFDIRI dfrfdiri;
  PCx_QueryVariation::PERCENTORPOINTS pop;
  PCx_QueryVariation::INCREASEDECREASE incdec;
  PCx_QueryVariation::OPERATORS oper;
  qint64 val;
  int year1, year2;
  if (getParamsReqVariation(typeId, ored, dfrfdiri, incdec, pop, oper, val, year1, year2) == false) {
    return;
  }

  PCx_QueryVariation qv(model, typeId, ored, dfrfdiri, incdec, pop, oper, val, year1, year2);

  currentHtmlDoc = PCx_Report::generateMainHTMLHeader();
  currentHtmlDoc.append(model->generateHTMLAuditTitle());
  if (xlsDoc != nullptr) {
    delete xlsDoc;
    xlsDoc = nullptr;
  }
  xlsDoc = new QXlsx::Document();
  currentHtmlDoc.append(qv.exec(xlsDoc));
  currentHtmlDoc.append("</body></html>");
  doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_comboBoxAugDim_activated(int index) {
  Q_UNUSED(index);
  if (static_cast<PCx_QueryVariation::INCREASEDECREASE>(ui->comboBoxAugDim->currentData().toInt()) ==
      PCx_QueryVariation::INCREASEDECREASE::VARIATION) {
    ui->doubleSpinBox->setMinimum(-INT64_MAX / FIXEDPOINTCOEFF);
  } else {
    ui->doubleSpinBox->setMinimum(0.0);
  }
}

void FormQueries::on_pushButtonSaveReqVariation_clicked() {
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Nouvelle requête"), tr("Nom de la requête : "), QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    unsigned int typeId;
    PCx_Audit::ORED ored;
    MODES::DFRFDIRI dfrfdiri;
    PCx_QueryVariation::PERCENTORPOINTS pop;
    PCx_QueryVariation::INCREASEDECREASE incdec;
    PCx_QueryVariation::OPERATORS oper;
    qint64 val;
    int year1, year2;
    if (getParamsReqVariation(typeId, ored, dfrfdiri, incdec, pop, oper, val, year1, year2) == false) {
      return;
    }

    PCx_QueryVariation qv(model, typeId, ored, dfrfdiri, incdec, pop, oper, val, year1, year2);
    if (!qv.canSave(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà une requête de ce type portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }
    if (qv.save(text) == 0) {
      QMessageBox::critical(this, tr("Attention"), tr("Impossible d'enregistrer la requête !"));
      return;
    }
    queriesModel->update();
    emit listOfQueriesChanged(model->getAuditId());
  }
}

void FormQueries::on_pushButtonDelete_clicked() {
  QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
  if (selection.isEmpty()) {
    return;
  }

  if (question(tr("Voulez-vous vraiment <b>supprimer</b> les requêtes "
                  "sélectionnées ? Cette action ne peut être annulée")) == QMessageBox::No) {
    return;
  }

  foreach (const QModelIndex &idx, selection) {
    unsigned int selectedQueryId = queriesModel->record(idx.row()).value("id").toUInt();
    PCx_Query::deleteQuery(model->getAuditId(), selectedQueryId);
  }
  emit listOfQueriesChanged(model->getAuditId());
  queriesModel->update();
}

QString FormQueries::execQueries(QModelIndexList items, QXlsx::Document *xlsDocument) {
  QProgressDialog progress(tr("Calculs en cours..."), tr("Annuler"), 0, items.count(), this);
  progress.setMinimumDuration(200);

  progress.setWindowModality(Qt::ApplicationModal);

  progress.setValue(0);
  QString output = PCx_Report::generateMainHTMLHeader();
  output.append(model->generateHTMLAuditTitle());

  int count = 0;
  foreach (const QModelIndex &idx, items) {
    unsigned int selectedQueryId = queriesModel->record(idx.row()).value("id").toUInt();
    PCx_Query::QUERIESTYPES selectedQueryType =
        static_cast<PCx_Query::QUERIESTYPES>(queriesModel->record(idx.row()).value("query_mode").toUInt());

    switch (selectedQueryType) {
    case PCx_Query::QUERIESTYPES::VARIATION: {
      PCx_QueryVariation pqv(model, selectedQueryId);
      output.append(pqv.exec(xlsDocument));
      break;
    }

    case PCx_Query::QUERIESTYPES::RANK: {
      PCx_QueryRank pqr(model, selectedQueryId);
      output.append(pqr.exec(xlsDocument));
      break;
    }

    case PCx_Query::QUERIESTYPES::MINMAX: {
      PCx_QueryMinMax pqmm(model, selectedQueryId);
      output.append(pqmm.exec(xlsDocument));
      break;
    }
    }
    count++;
    if (!progress.wasCanceled()) {
      progress.setValue(count);
    } else {
      return QString();
    }
  }
  output.append("</body></html>");
  return output;
}

void FormQueries::on_pushButtonExecFromList_clicked() {
  QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
  if (selection.isEmpty()) {
    return;
  }
  doc->clear();
  if (xlsDoc != nullptr) {
    delete xlsDoc;
    xlsDoc = nullptr;
  }
  xlsDoc = new QXlsx::Document();
  currentHtmlDoc = execQueries(selection, xlsDoc);

  // Aborded in progress dialog
  if (currentHtmlDoc.isEmpty()) {
    delete xlsDoc;
    xlsDoc = nullptr;
    return;
  }
  doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_listView_activated(const QModelIndex &index) {
  Q_UNUSED(index);
  on_pushButtonExecFromList_clicked();
}

void FormQueries::on_pushButtonSave_clicked() {
  if (doc->isEmpty()) {
    return;
  }
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le résultat de la requête en HTML"), "",
                                                tr("Fichiers HTML (*.html *.htm)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFileInfo fi(fileName);
  if (fi.suffix().compare("html", Qt::CaseInsensitive) != 0 && fi.suffix().compare("htm", Qt::CaseInsensitive) != 0) {
    fileName.append(".html");
  }
  fi = QFileInfo(fileName);

  if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
    QMessageBox::critical(this, tr("Attention"), tr("Fichier non accessible en écriture"));
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible"));
    return;
  }

  QTextStream stream(&file);

  QSettings settings;
  QString settingStyle = settings.value("output/style", "CSS").toString();

  if (settingStyle == "INLINE") {
    stream << doc->toHtml();
  } else {
    stream << currentHtmlDoc;
  }

  stream.flush();
  file.close();
}

void FormQueries::on_pushButtonExecReqRank_clicked() {
  unsigned int typeId, number;
  int year1, year2;
  PCx_Audit::ORED ored;
  MODES::DFRFDIRI dfrfdiri;
  PCx_QueryRank::GREATERSMALLER grSm;

  if (getParamsReqRank(typeId, ored, dfrfdiri, number, grSm, year1, year2) == false) {
    return;
  }

  PCx_QueryRank qr(model, typeId, ored, dfrfdiri, grSm, number, year1, year2);

  currentHtmlDoc = PCx_Report::generateMainHTMLHeader();
  currentHtmlDoc.append(model->generateHTMLAuditTitle());

  if (xlsDoc != nullptr) {
    delete xlsDoc;
    xlsDoc = nullptr;
  }
  xlsDoc = new QXlsx::Document();
  currentHtmlDoc.append(qr.exec(xlsDoc));
  currentHtmlDoc.append("</body></html>");
  doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_pushButtonSaveReqRank_clicked() {
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Nouvelle requête"), tr("Nom de la requête : "), QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    unsigned int typeId, number;
    int year1, year2;
    PCx_Audit::ORED ored;
    MODES::DFRFDIRI dfrfdiri;
    PCx_QueryRank::GREATERSMALLER grSm;

    if (getParamsReqRank(typeId, ored, dfrfdiri, number, grSm, year1, year2) == false) {
      return;
    }

    PCx_QueryRank qr(model, typeId, ored, dfrfdiri, grSm, number, year1, year2);

    if (!qr.canSave(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà une requête de ce type portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }
    if (qr.save(text) == 0) {
      QMessageBox::critical(this, tr("Attention"), tr("Impossible d'enregistrer la requête !"));
      return;
    }
    queriesModel->update();
    emit listOfQueriesChanged(model->getAuditId());
  }
}

void FormQueries::on_pushButtonExecReq3_clicked() {
  unsigned int typeId;
  int year1, year2;
  PCx_Audit::ORED ored;
  MODES::DFRFDIRI dfrfdiri;
  qint64 val1, val2;

  if (getParamsReqMinMax(typeId, ored, dfrfdiri, val1, val2, year1, year2) == false) {
    return;
  }

  PCx_QueryMinMax qmm(model, typeId, ored, dfrfdiri, val1, val2, year1, year2);

  currentHtmlDoc = PCx_Report::generateMainHTMLHeader();
  currentHtmlDoc.append(model->generateHTMLAuditTitle());

  if (xlsDoc != nullptr) {
    delete xlsDoc;
    xlsDoc = nullptr;
  }
  xlsDoc = new QXlsx::Document();
  currentHtmlDoc.append(qmm.exec(xlsDoc));
  currentHtmlDoc.append("</body></html>");
  doc->setHtml(currentHtmlDoc);
}

void FormQueries::on_pushButtonSaveReq3_clicked() {
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Nouvelle requête"), tr("Nom de la requête : "), QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    unsigned int typeId;
    int year1, year2;
    qint64 val1, val2;
    PCx_Audit::ORED ored;
    MODES::DFRFDIRI dfrfdiri;

    if (getParamsReqMinMax(typeId, ored, dfrfdiri, val1, val2, year1, year2) == false) {
      return;
    }

    PCx_QueryMinMax qr(model, typeId, ored, dfrfdiri, val1, val2, year1, year2);

    if (!qr.canSave(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà une requête de ce type portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }
    if (qr.save(text) == 0) {
      QMessageBox::critical(this, tr("Attention"), tr("Impossible d'enregistrer la requête !"));
      return;
    }
    queriesModel->update();
    emit listOfQueriesChanged(model->getAuditId());
  }
}

void FormQueries::on_pushButtonSaveXLSX_clicked() {
  if (doc->isEmpty() || xlsDoc == nullptr) {
    return;
  }
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer le résultat de la requête en XLSX"), "",
                                                tr("Fichiers XLSX (*.xlsx)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFileInfo fi(fileName);
  if (fi.suffix().compare("xlsx", Qt::CaseInsensitive) != 0) {
    fileName.append(".xlsx");
  }
  fi = QFileInfo(fileName);

  if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
    QMessageBox::critical(this, tr("Attention"), tr("Fichier non accessible en écriture"));
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible"));
    return;
  }

  xlsDoc->saveAs(&file);

  file.close();
}
