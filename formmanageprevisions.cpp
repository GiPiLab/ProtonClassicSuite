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

#include "formmanageprevisions.h"
#include "pcx_prevision.h"
#include "ui_formmanageprevisions.h"
#include <QInputDialog>
#include <QMessageBox>

FormManagePrevisions::FormManagePrevisions(QWidget *parent) : QWidget(parent), ui(new Ui::FormManagePrevisions) {
  ui->setupUi(this);
  selectedPrevision = nullptr;
  updateListOfPrevisions();
  updateListOfAudits();

  // TODO(unknown): Import/export des prévisions
}

void FormManagePrevisions::onListOfAuditsChanged() { updateListOfAudits(); }

void FormManagePrevisions::updateListOfPrevisions() {
  ui->comboListPrevisions->clear();

  QList<QPair<unsigned int, QString>> listOfPrevisions = PCx_Prevision::getListOfPrevisions();

  QList<QPair<unsigned int, QString>> listOfAudits =
      PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);

  if (listOfPrevisions.isEmpty() && !listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Vous pouvez maintenant utiliser un audit pour créer une "
                                "<b>prévision</b>. Une prévision consiste en un ensemble de "
                                "critères appliqués à chaque noeud de l'arbre afin de fixer les "
                                "valeurs des crédits prévus pour l'année N+1"));
  }

  ui->groupBoxPrevisions->setEnabled(!listOfPrevisions.isEmpty());
  QPair<unsigned int, QString> p;
  foreach (p, listOfPrevisions) { ui->comboListPrevisions->insertItem(0, p.second, p.first); }
  ui->comboListPrevisions->setCurrentIndex(0);
  on_comboListPrevisions_activated(0);
}

FormManagePrevisions::~FormManagePrevisions() {
  delete ui;
  delete selectedPrevision;
}

void FormManagePrevisions::onSelectedPrevisionUpdated() {
  if (selectedPrevision != nullptr) {
    ui->pushButtonToAudit->setEnabled(!selectedPrevision->isPrevisionEmpty());
  }
}

void FormManagePrevisions::on_comboListPrevisions_activated(int index) {
  if (index == -1 || ui->comboListPrevisions->count() == 0) {
    return;
  }
  unsigned int selectedPrevisionId = ui->comboListPrevisions->currentData().toUInt();

  if (selectedPrevision != nullptr) {
    delete selectedPrevision;
    selectedPrevision = nullptr;
  }

  selectedPrevision = new PCx_Prevision(selectedPrevisionId);
  onSelectedPrevisionUpdated();
  PCx_Audit selectedAttachedAudit(selectedPrevision->getAttachedAuditId());

  // qDebug()<<"Selected audit = "<<selectedAuditId<< "
  // "<<ui->comboListOfAudits->currentText();

  QLocale defaultLocale;
  ui->labelDate->setText(defaultLocale.toString(selectedPrevision->getCreationTimeLocal(), QLocale::LongFormat));
  ui->labelName->setText(selectedPrevision->getPrevisionName());
  ui->labelAttachedAudit->setText(selectedAttachedAudit.getAuditName());
  ui->labelAttachedAuditYears->setText(selectedAttachedAudit.getYearsString());
  ui->labelAttachedTree->setText(selectedAttachedAudit.getAttachedTreeName());
}

void FormManagePrevisions::updateListOfAudits() {
  ui->comboListAudits->clear();

  QList<QPair<unsigned int, QString>> listOfAudits =
      PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);
  QPair<unsigned int, QString> p;
  if (listOfAudits.count() == 0) {
    QMessageBox::information(this, tr("Information"),
                             tr("Les prévisions s'appuient sur les données historiques des audits. "
                                "Commencez par créer, remplir et terminer un audit dans la fenêtre "
                                "de gestion des audits"));
    ui->groupBox_2->setEnabled(false);
  } else {
    ui->groupBox_2->setEnabled(true);
  }
  foreach (p, listOfAudits) { ui->comboListAudits->insertItem(0, p.second, p.first); }
  ui->comboListAudits->setCurrentIndex(0);
}

void FormManagePrevisions::on_pushButtonAddPrevision_clicked() {
  if (ui->comboListAudits->count() == 0) {
    return;
  }
  int selectedIndex = ui->comboListAudits->currentIndex();

  if (selectedIndex == -1) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez l'audit sur lequel sera adossé la prévision"));
    return;
  }

  unsigned int selectedAudit = ui->comboListAudits->currentData().toUInt();

  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Nouvelle prévision"), tr("Nom de la nouvelle prévision à ajouter : "),
                                 QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    if (PCx_Prevision::previsionNameExists(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà une prévision portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }

    if (PCx_Prevision::addNewPrevision(selectedAudit, text)) {
      updateListOfPrevisions();
      emit listOfPrevisionsChanged();
      QMessageBox::information(this, tr("Information"),
                               tr("Nouvelle prévision ajoutée ! Utilisez la fenêtre <b>élaboration "
                                  "du budget</b> afin de fixer des critères de prévision pour "
                                  "l'année N+1"));
    }
  }
}

void FormManagePrevisions::on_pushButtonDelete_clicked() {
  if (ui->comboListPrevisions->currentIndex() == -1) {
    return;
  }
  if (question(tr("Voulez-vous vraiment <b>supprimer</b> la prévision <b>%1</b> ? "
                  "Cette action ne peut être annulée")
                   .arg(ui->comboListPrevisions->currentText().toHtmlEscaped())) == QMessageBox::No) {
    return;
  }
  if (PCx_Prevision::deletePrevision(ui->comboListPrevisions->currentData().toUInt()) == false) {
    die();
  }
  if (selectedPrevision != nullptr) {
    delete selectedPrevision;
    selectedPrevision = nullptr;
  }
  updateListOfPrevisions();
  emit listOfPrevisionsChanged();
}

void FormManagePrevisions::on_pushButtonToAudit_clicked() {
  if (ui->comboListPrevisions->currentIndex() == -1 || selectedPrevision == nullptr) {
    return;
  }

  if (question(tr("Voulez-vous incorporer les valeurs des prévisions dans un "
                  "audit étendu d'un an à partir de l'audit courant ?")) == QMessageBox::No) {
    return;
  }
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Nouvel audit"),
                                 tr("Nom de l'audit étendu (l'audit actuel est \"%1\")")
                                     .arg(selectedPrevision->getAttachedAudit()->getAuditName().toHtmlEscaped()),
                                 QLineEdit::Normal, "", &ok)
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
    int res = selectedPrevision->toPrevisionalExtendedAudit(text);

    if (res > 0) {
      QMessageBox::information(this, tr("Information"),
                               tr("Audit étendu ajouté, utilisez les fenêtres usuelles sur les "
                                  "audits pour le terminer et l'exploiter"));
      // NOTE : not needed because the new audit is not finished
      updateListOfAudits();
      // To notify formManageAudits and formEditAudit
      emit listOfAuditsChanged();
    }
    // qDebug()<<"Adding an audit with name="<<text<<" years = "<<yearsString<<"
    // treeId = "<<selectedTree;
  }
}

void FormManagePrevisions::on_pushButtonDuplicate_clicked() {
  bool ok;
  QString text;

redo:
  do {
    text = QInputDialog::getText(this, tr("Dupliquer prévision"), tr("Nom de la prévision dupliquée"),
                                 QLineEdit::Normal, "", &ok)
               .simplified();

  } while (ok && text.isEmpty());

  if (ok) {
    if (PCx_Prevision::previsionNameExists(text)) {
      QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà une prévision portant ce nom !"));
      goto redo;
    }
    if (text.size() > MAXOBJECTNAMELENGTH) {
      QMessageBox::warning(this, tr("Attention"), tr("Nom trop long !"));
      goto redo;
    }
    unsigned int res = selectedPrevision->duplicatePrevision(text);

    if (res > 0) {
      QMessageBox::information(this, tr("Information"), tr("Prévision dupliquée !"));
      updateListOfPrevisions();
      emit listOfPrevisionsChanged();
    }
  }
}
