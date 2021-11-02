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

#include "dialogduplicateaudit.h"
#include "ui_dialogduplicateaudit.h"
#include <QMessageBox>

DialogDuplicateAudit::DialogDuplicateAudit(unsigned int oldAuditId, QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogDuplicateAudit), oldAuditId(oldAuditId) {
  ui->setupUi(this);
  oldAudit = new PCx_Audit(oldAuditId);
  ui->labelOldName->setText(oldAudit->getAuditName());
  QLocale defaultLocale;
  ui->labelOldDate->setText(defaultLocale.toString(oldAudit->getCreationTimeLocal(), QLocale::LongFormat));
  ui->labelOldTree->setText(QString("%1 (%2 noeuds)")
                                .arg(oldAudit->getAttachedTreeName())
                                .arg(oldAudit->getAttachedTree()->getNumberOfNodes()));
  ui->labelOldYears->setText(oldAudit->getYearsString());
  ui->spinBoxYear1->setValue(static_cast<int>(oldAudit->getYears().constFirst()));
  ui->spinBoxYear2->setValue(static_cast<int>(oldAudit->getYears().constLast()));
  // Remove date constraints
  // QDate date=QDate::currentDate();
  // ui->spinBoxYear1->setMaximum(date.year());
  // ui->spinBoxYear2->setMaximum(date.year()+1);
}

DialogDuplicateAudit::~DialogDuplicateAudit() {
  delete oldAudit;
  delete ui;
}

void DialogDuplicateAudit::on_pushButton_clicked() {
  QString newName = ui->lineEditNewName->text().simplified();
  if (newName.isEmpty()) {
    QMessageBox::information(this, tr("Attention"), tr("Donnez un nom au nouvel audit"));
    return;
  }

  if (PCx_Audit::auditNameExists(newName)) {
    QMessageBox::warning(this, tr("Attention"), tr("Il existe déjà un audit portant ce nom, choisissez un autre nom"));
    return;
  }

  int year1 = ui->spinBoxYear1->value();
  int year2 = ui->spinBoxYear2->value();

  if (year1 >= year2) {
    QMessageBox::warning(this, tr("Attention"), tr("L'audit doit porter sur au moins deux années"));
    return;
  }
  // DUPLICATE AUDIT

  QList<int> years;
  for (int i = year1; i <= year2; i++) {
    years.append(i);
  }

  if (question(tr("Voulez-vous vraiment dupliquer cet audit ? L'opération peut "
                  "nécessiter du temps et ne peut être interrompue.")) == QMessageBox::Yes) {
    int res = oldAudit->duplicateAudit(newName, years, ui->checkBoxDF->isChecked(), ui->checkBoxRF->isChecked(),
                                       ui->checkBoxDI->isChecked(), ui->checkBoxRI->isChecked(),
                                       ui->checkBoxQueries->isChecked());

    if (res > 0) {
      QMessageBox::information(this, "", tr("Audit dupliqué !"));
    }
    done(Accepted);
  }
}

void DialogDuplicateAudit::on_pushButton_2_clicked() { done(Rejected); }
