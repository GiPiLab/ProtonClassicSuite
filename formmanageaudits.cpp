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

#include "formmanageaudits.h"
#include "dialogduplicateaudit.h"
#include "formauditinfos.h"
#include "formdisplaytree.h"
#include "pcx_audit.h"
#include "ui_formmanageaudits.h"
#include "utils.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

FormManageAudits::FormManageAudits(QWidget *parent) : QWidget(parent), ui(new Ui::FormManageAudits) {
  selectedAudit = nullptr;
  ui->setupUi(this);
  updateListOfTrees();
  updateListOfAudits();
  // Remove date constraints
  QDate date = QDate::currentDate();
  // ui->spinBoxFrom->setMaximum(date.year());
  // ui->spinBoxTo->setMaximum(date.year()+1);

  ui->spinBoxTo->setValue(date.year() - 1);
  updateRandomButtonVisibility();
}

FormManageAudits::~FormManageAudits() {
  delete selectedAudit;
  delete ui;
}

void FormManageAudits::updateButtonsVisibility() {
  if (selectedAudit == nullptr) {
    return;
  }
  bool finished = selectedAudit->isFinished();
  ui->pushButtonClearDF->setEnabled(!finished);
  ui->pushButtonClearRF->setEnabled(!finished);
  ui->pushButtonClearDI->setEnabled(!finished);
  ui->pushButtonClearRI->setEnabled(!finished);
  ui->pushButtonLoadDF->setEnabled(!finished);
  ui->pushButtonLoadRF->setEnabled(!finished);
  ui->pushButtonLoadDI->setEnabled(!finished);
  ui->pushButtonLoadRI->setEnabled(!finished);
  updateRandomButtonVisibility();
}

void FormManageAudits::updateRandomButtonVisibility() {
  QSettings settings;
  bool randomAllowed = settings.value("misc/randomAllowed", true).toBool();
  bool finished = false;
  if (selectedAudit != nullptr) {
    finished = selectedAudit->isFinished();
  }

  if (!finished) {
    ui->pushButtonRandomDF->setEnabled(randomAllowed);
  } else {
    ui->pushButtonRandomDF->setEnabled(false);
  }
  if (!finished) {
    ui->pushButtonRandomRF->setEnabled(randomAllowed);
  } else {
    ui->pushButtonRandomRF->setEnabled(false);
  }
  if (!finished) {
    ui->pushButtonRandomDI->setEnabled(randomAllowed);
  } else {
    ui->pushButtonRandomDI->setEnabled(false);
  }
  if (!finished) {
    ui->pushButtonRandomRI->setEnabled(randomAllowed);
  } else {
    ui->pushButtonRandomRI->setEnabled(false);
  }
}

void FormManageAudits::updateListOfAudits() {
  ui->comboListOfAudits->clear();

  QList<QPair<unsigned int, QString>> listOfAudits = PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::AllAudits);

  QList<QPair<unsigned int, QString>> lot = PCx_Tree::getListOfTrees(true);
  if (!lot.isEmpty() && listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Vous pouvez maintenant créer un nouvel audit. Une fois créé, "
                                "utilisez la fenêtre <b>saisie des données</b> pour le remplir "
                                "manuellement, ou alors le bouton <b>Générer le fichier "
                                "squelette</b> pour fabriquer un ficher excel que vous dupliquerez "
                                "(un pour les DF, un pour les RF, ainsi de suite le cas échéant) et "
                                "compléterez avec vos données, avant de les charger avec les "
                                "boutons <b>Charger [x]</b>. Vous pouvez aussi remplir l'audit de "
                                "données aléatoires à des fins de test."));
  }

  ui->groupBoxAudits->setEnabled(!listOfAudits.isEmpty());
  QPair<unsigned int, QString> p;
  foreach (p, listOfAudits) { ui->comboListOfAudits->insertItem(0, p.second, p.first); }
  ui->comboListOfAudits->setCurrentIndex(0);
  this->on_comboListOfAudits_activated(0);
}

void FormManageAudits::onLOTchanged() { updateListOfTrees(); }

void FormManageAudits::onListOfAuditsChanged() { updateListOfAudits(); }

void FormManageAudits::updateListOfTrees() {
  ui->comboListOfTrees->clear();

  QList<QPair<unsigned int, QString>> lot = PCx_Tree::getListOfTrees(true);
  setEnabled(!lot.isEmpty());

  if (lot.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("<b>Terminez</b> d'abord un arbre afin de pouvoir créer un audit"));
  }

  QPair<unsigned int, QString> p;
  foreach (p, lot) { ui->comboListOfTrees->insertItem(0, p.second, p.first); }
  ui->comboListOfTrees->setCurrentIndex(0);
}

void FormManageAudits::on_addAuditButton_clicked() {
  if (ui->comboListOfTrees->count() == 0) {
    return;
  }
  int selectedIndex = ui->comboListOfTrees->currentIndex();

  if (selectedIndex == -1) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez l'arbre sur lequel sera adossé l'audit"));
    return;
  }

  unsigned int selectedTree = ui->comboListOfTrees->currentData().toUInt();

  int anneeFrom = ui->spinBoxFrom->value();
  int anneeTo = ui->spinBoxTo->value();

  if (anneeFrom >= anneeTo) {
    QMessageBox::warning(this, tr("Attention"), tr("L'audit doit porter sur au moins deux années"));
    return;
  }

  QList<int> years;
  QString yearsString;
  yearsString = QString("De %1 à %2").arg(anneeFrom).arg(anneeTo);
  for (int i = anneeFrom; i <= anneeTo; i++) {
    years.append(i);
  }

  bool ok;
  QString text;

redo:
  do {
    text =
        QInputDialog::getText(this, tr("Nouvel audit"), tr("Nom de l'audit à ajouter : "), QLineEdit::Normal, "", &ok)
            .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    if (PCx_Audit::auditNameExists(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un audit portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }

    // qDebug()<<"Adding an audit with name="<<text<<" years = "<<yearsString<<"
    // treeId = "<<selectedTree;
    if (PCx_Audit::addNewAudit(text, years, selectedTree) > 0) {
      updateListOfAudits();
      emit listOfAuditsChanged();
      QMessageBox::information(this, tr("Information"),
                               tr("Nouvel audit ajouté, utilisez la fenêtre <b>saisie des "
                                  "données</b> pour le remplir manuellement, ou alors le bouton "
                                  "<b>générer un fichier squelette</b> afin de créer un fichier "
                                  "avec les feuilles de l'arbre et les colonnes à compléter. Ce "
                                  "fichier doit être dupliqué pour les DF, RF, DI, RI le cas "
                                  "échéant puis complété sous excel avec vos données. Une fois les "
                                  "fichiers complétés, utilisez les boutons <b>Charger [x]</b> pour "
                                  "les importer dans l'audit. Vous pouvez également remplir l'audit "
                                  "de données aléatoires à des fins de test"));
    }
  }
}

void FormManageAudits::on_comboListOfAudits_activated(int index) {
  if (index == -1 || ui->comboListOfAudits->count() == 0) {
    return;
  }
  unsigned int selectedAuditId = ui->comboListOfAudits->currentData().toUInt();

  if (selectedAudit != nullptr) {
    delete selectedAudit;
    selectedAudit = nullptr;
  }

  selectedAudit = new PCx_Audit(selectedAuditId);

  // qDebug()<<"Selected audit = "<<selectedAuditId<< "
  // "<<ui->comboListOfAudits->currentText();
  QLocale defaultLocale;
  ui->labelDate->setText(defaultLocale.toString(selectedAudit->getCreationTimeLocal(), QLocale::LongFormat));
  if (selectedAudit->isFinished() == true) {
    ui->labelFinished->setText(tr("oui"));
    ui->finishAuditButton->setEnabled(false);
  } else {
    ui->labelFinished->setText(tr("non"));
    ui->finishAuditButton->setEnabled(true);
  }
  ui->labelTree->setText(QString("%1 (%2 noeuds)")
                             .arg(selectedAudit->getAttachedTreeName())
                             .arg(selectedAudit->getAttachedTree()->getNumberOfNodes()));
  ui->labelYears->setText(selectedAudit->getYearsString());
  ui->labelName->setText(selectedAudit->getAuditName());
  updateButtonsVisibility();
}

void FormManageAudits::on_deleteAuditButton_clicked() {
  if (ui->comboListOfAudits->currentIndex() == -1) {
    return;
  }
  if (question(tr("Voulez-vous vraiment <b>supprimer</b> l'audit <b>%1</b> ? Cette "
                  "action ne peut être annulée")
                   .arg(ui->comboListOfAudits->currentText().toHtmlEscaped())) == QMessageBox::No) {
    return;
  }
  if (PCx_Audit::deleteAudit(ui->comboListOfAudits->currentData().toUInt()) == false) {
    return;
  }

  if (selectedAudit != nullptr) {
    delete selectedAudit;
    selectedAudit = nullptr;
  }
  updateListOfAudits();
  emit listOfAuditsChanged();
}

void FormManageAudits::on_finishAuditButton_clicked() {
  if (ui->comboListOfAudits->currentIndex() == -1) {
    return;
  }
  if (question(tr("Voulez-vous vraiment <b>terminer</b> l'audit <b>%1</b> ? Cela "
                  "signifie que vous ne pourrez plus en modifier les données")
                   .arg(ui->comboListOfAudits->currentText().toHtmlEscaped())) == QMessageBox::No) {
    return;
  }
  if (selectedAudit != nullptr) {
    selectedAudit->finishAudit();
  } else {
    qWarning() << "Invalid audit selected !";
  }
  updateListOfAudits();
  emit listOfAuditsChanged();
}

void FormManageAudits::on_cloneAuditButton_clicked() {
  DialogDuplicateAudit d(ui->comboListOfAudits->currentData().toUInt(), this);
  int res = d.exec();
  if (res == QDialog::Accepted) {
    updateListOfAudits();
    emit listOfAuditsChanged();
  }
}

void FormManageAudits::on_pushButtonDisplayTree_clicked() {
  if (ui->comboListOfTrees->count() == 0) {
    return;
  }
  int selectedIndex = ui->comboListOfTrees->currentIndex();

  if (selectedIndex == -1) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez l'arbre sur lequel sera adossé l'audit"));
    return;
  }

  unsigned int selectedTree = ui->comboListOfTrees->currentData().toUInt();

  auto *ddt = new FormDisplayTree(selectedTree, this);
  ddt->setAttribute(Qt::WA_DeleteOnClose);
  auto *mdiSubWin = qobject_cast<QMdiSubWindow *>(this->parentWidget());
  QMdiArea *mdiArea = mdiSubWin->mdiArea();
  QMdiSubWindow *subWin = mdiArea->addSubWindow(ddt);
  subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
  ddt->show();
}

void FormManageAudits::on_statisticsAuditButton_clicked() {
  FormAuditInfos *infos = new FormAuditInfos(ui->comboListOfAudits->currentData().toUInt(), this);
  infos->setAttribute(Qt::WA_DeleteOnClose);
  auto *mdiSubWin = qobject_cast<QMdiSubWindow *>(this->parentWidget());
  QMdiArea *mdiArea = mdiSubWin->mdiArea();
  mdiArea->addSubWindow(infos);
  infos->show();
}

void FormManageAudits::on_pushButtonRandomDF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::DF;

  if (question(tr("Remplir les <b>%1</b> de l'audit de données aléatoires ?")
                   .arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) == QMessageBox::No) {
    return;
  }

  // QElapsedTimer timer;
  // timer.start();

  selectedAudit->fillWithRandomData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
  // qDebug()<<"Done in "<<timer.elapsed()<<"ms";
}

void FormManageAudits::on_pushButtonRandomRF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::RF;

  if (question(tr("Remplir les <b>%1</b> de l'audit de données aléatoires ?")
                   .arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) == QMessageBox::No) {
    return;
  }

  // QElapsedTimer timer;
  // timer.start();

  selectedAudit->fillWithRandomData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
  // qDebug()<<"Done in "<<timer.elapsed()<<"ms";
}

void FormManageAudits::on_pushButtonRandomDI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::DI;

  if (question(tr("Remplir les <b>%1</b> de l'audit de données aléatoires ?")
                   .arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) == QMessageBox::No) {
    return;
  }

  // QElapsedTimer timer;
  // timer.start();

  selectedAudit->fillWithRandomData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
  // qDebug()<<"Done in "<<timer.elapsed()<<"ms";
}

void FormManageAudits::on_pushButtonRandomRI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::RI;

  if (question(tr("Remplir les <b>%1</b> de l'audit de données aléatoires ?")
                   .arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) == QMessageBox::No) {
    return;
  }

  // QElapsedTimer timer;
  // timer.start();

  selectedAudit->fillWithRandomData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
  // qDebug()<<"Done in "<<timer.elapsed()<<"ms";
}

void FormManageAudits::on_pushButtonClearDF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::DF;

  if (question(tr("Effacer toutes les <b>%1</b> ?").arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) ==
      QMessageBox::No) {
    return;
  }

  selectedAudit->clearAllData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
}

void FormManageAudits::on_pushButtonClearRF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::RF;

  if (question(tr("Effacer toutes les <b>%1</b> ?").arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) ==
      QMessageBox::No) {
    return;
  }

  selectedAudit->clearAllData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
}

void FormManageAudits::on_pushButtonClearDI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::DI;

  if (question(tr("Effacer toutes les <b>%1</b> ?").arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) ==
      QMessageBox::No) {
    return;
  }

  selectedAudit->clearAllData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
}

void FormManageAudits::on_pushButtonClearRI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  MODES::DFRFDIRI mode = MODES::DFRFDIRI::RI;

  if (question(tr("Effacer toutes les <b>%1</b> ?").arg(MODES::modeToCompleteString(mode).toHtmlEscaped())) ==
      QMessageBox::No) {
    return;
  }

  selectedAudit->clearAllData(mode);
  emit auditDataUpdated(selectedAudit->getAuditId());
}

void FormManageAudits::on_pushButtonLoadDF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  if (importLeaves(MODES::DFRFDIRI::DF)) {
    emit auditDataUpdated(selectedAudit->getAuditId());
  }
}

void FormManageAudits::on_pushButtonLoadRF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  if (importLeaves(MODES::DFRFDIRI::RF)) {
    emit auditDataUpdated(selectedAudit->getAuditId());
  }
}

void FormManageAudits::on_pushButtonLoadDI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  if (importLeaves(MODES::DFRFDIRI::DI)) {
    emit auditDataUpdated(selectedAudit->getAuditId());
  }
}

void FormManageAudits::on_pushButtonLoadRI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  if (importLeaves(MODES::DFRFDIRI::RI)) {
    emit auditDataUpdated(selectedAudit->getAuditId());
  }
}

bool FormManageAudits::importLeaves(MODES::DFRFDIRI mode) {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getOpenFileName(this, tr("Charger les données des feuilles"), "", tr("Fichiers XLSX (*.xlsx)"));
  if (fileName.isEmpty()) {
    return false;
  }

  bool res = selectedAudit->importDataFromXLSX(fileName, mode);

  if (res == true) {
    QMessageBox::information(this, tr("Succès"),
                             tr("%1 chargées !").arg(MODES::modeToCompleteString(mode).toHtmlEscaped()));
  } else {
    QMessageBox::critical(this, tr("Erreur"),
                          tr("%1 non chargées !").arg(MODES::modeToCompleteString(mode).toHtmlEscaped()));
  }
  return res;
}

bool FormManageAudits::exportLeaves(MODES::DFRFDIRI mode) {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getSaveFileName(this, tr("Enregistrer les données des feuilles"), "", tr("Fichiers XLSX (*.xlsx)"));
  if (fileName.isEmpty()) {
    return false;
  }

  QFileInfo fi(fileName);
  if (fi.suffix().compare("xlsx", Qt::CaseInsensitive) != 0) {
    fileName.append(".xlsx");
  }
  fi = QFileInfo(fileName);

  if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
    QMessageBox::critical(this, tr("Attention"), tr("Fichier non accessible en écriture"));
    return false;
  }

  bool res = selectedAudit->exportLeavesDataXLSX(mode, fileName);

  if (res == true) {
    QMessageBox::information(this, tr("Succès"),
                             tr("<b>%1</b> enregistrées.").arg(MODES::modeToCompleteString(mode).toHtmlEscaped()));
  } else {
    QMessageBox::critical(this, tr("Erreur"), tr("Enregistrement échoué"));
  }
  return res;
}

void FormManageAudits::on_pushButtonExportDF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  exportLeaves(MODES::DFRFDIRI::DF);
}

void FormManageAudits::on_pushButtonExportRF_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  exportLeaves(MODES::DFRFDIRI::RF);
}

void FormManageAudits::on_pushButtonExportDI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  exportLeaves(MODES::DFRFDIRI::DI);
}

void FormManageAudits::on_pushButtonExportRI_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }
  exportLeaves(MODES::DFRFDIRI::RI);
}

void FormManageAudits::on_pushButtonSkel_clicked() {
  if (selectedAudit == nullptr) {
    return;
  }

  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getSaveFileName(this, tr("Enregistrer le squelette de reporting"), "", tr("Fichiers XLSX (*.xlsx)"));
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

  bool res = selectedAudit->exportLeavesSkeleton(fileName);

  if (res == true) {
    QMessageBox::information(this, tr("Succès"), tr("Squelette de l'audit enregistré !"));
  } else {
    QMessageBox::critical(this, tr("Erreur"), tr("Enregistrement échoué"));
  }
}
