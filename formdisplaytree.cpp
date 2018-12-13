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

#include "formdisplaytree.h"
#include "ui_formdisplaytree.h"
#include "utils.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QtPrintSupport/QtPrintSupport>

FormDisplayTree::FormDisplayTree(unsigned int treeId, QWidget *parent) : QWidget(parent), ui(new Ui::FormDisplayTree) {
  ui->setupUi(this);
  model = new PCx_TreeModel(treeId);
  ui->treeView->setModel(model);
  QDateTime dt = QDateTime::currentDateTime();
  ui->label->setText(
      tr("Arbre %1 le %2 à %3").arg(model->getName(), dt.date().toString("dd/MM/yyyy"), dt.time().toString()));
  ui->treeView->expandToDepth(1);
}

FormDisplayTree::~FormDisplayTree() {
  delete model;
  delete ui;
}

void FormDisplayTree::on_printViewButton_clicked() {
#ifndef Q_OS_ANDROID
  QPrinter printer;

  auto *dialog = new QPrintDialog(&printer, this);
  dialog->setWindowTitle(tr("Imprimer"));
  if (dialog->exec() != QDialog::Accepted)
    return;
  QPainter painter;
  painter.begin(&printer);
  double xscale = printer.pageRect().width() / double(ui->treeView->width());
  double yscale = printer.pageRect().height() / double(ui->treeView->height());
  double scale = qMin(xscale, yscale) / 2;
  painter.translate(printer.paperRect().x() + printer.pageRect().width() / 2,
                    printer.paperRect().y() + printer.pageRect().height() / 2);
  painter.scale(scale, scale);
  painter.translate(-width() / 2, -height() / 2);

  ui->treeView->render(&painter);
#else
  QMessageBox::warning(this, tr("Non supporté"), tr("Fonction non supportée sur cette plateforme"));
#endif
}

void FormDisplayTree::on_expandButton_clicked() { ui->treeView->expandAll(); }

void FormDisplayTree::on_collapseButton_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormDisplayTree::on_exportButton_clicked() {
#ifdef Q_OS_ANDROID
  QMessageBox::warning(this, tr("Non supporté"), tr("Fonction non supportée sur cette plateforme"));
  return;
#endif

  QList<unsigned int> listOfTrees = PCx_Tree::getListOfTreesId();
  if (!listOfTrees.contains(model->getTreeId())) {
    QMessageBox::warning(this, tr("Attention"), tr("Cet arbre a été supprimé !"));
    return;
  }
  QString dot = model->toDot();

  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getSaveFileName(this, tr("Enregistrer l'arbre au format PDF"), "", tr("Fichiers PDF (*.pdf)"));
  if (fileName.isEmpty())
    return;

  QFileInfo fi(fileName);
  if (fi.suffix().compare("pdf", Qt::CaseInsensitive) != 0)
    fileName.append(".pdf");
  fi = QFileInfo(fileName);

  // Process dot graph
  if (dotToPdf(dot.toUtf8(), fileName) == false) {
    QMessageBox::warning(this, tr("Echec"), tr("Enregistrement du PDF impossible"));
    return;
  }
  QMessageBox::information(this, tr("Succès"), tr("Arbre enregistré au format PDF"));

  if (question(tr("Enregistrer aussi la source au format GraphViz ?")) == QMessageBox::Yes) {
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer la source de l'arbre au format DOT"), "",
                                                  tr("Fichiers Graphviz (*.dot)"));
    if (fileName.isEmpty())
      return;

    QFileInfo fi(fileName);
    if (fi.suffix().compare("dot", Qt::CaseInsensitive) != 0)
      fileName.append(".dot");
    fi = QFileInfo(fileName);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
      return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << dot;
    file.close();
    QMessageBox::information(this, tr("Succès"), tr("Source de l'arbre enregistrée au format DOT"));
  }
}

void FormDisplayTree::on_exportXLSXButton_clicked() {
  QList<unsigned int> listOfTrees = PCx_Tree::getListOfTreesId();
  if (!listOfTrees.contains(model->getTreeId())) {
    QMessageBox::warning(this, tr("Attention"), tr("Cet arbre a été supprimé !"));
    return;
  }
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  QString fileName =
      fileDialog.getSaveFileName(this, tr("Enregistrer l'arbre au format XLSX"), "", tr("Fichiers XLSX (*.xlsx)"));
  if (fileName.isEmpty())
    return;

  QFileInfo fi(fileName);
  if (fi.suffix().compare("xlsx", Qt::CaseInsensitive) != 0)
    fileName.append(".xlsx");
  fi = QFileInfo(fileName);

  bool res = model->toXLSX(fileName);

  if (res == true) {
    QMessageBox::information(this, tr("Succès"), tr("Arbre enregistré au format XLSX"));
  } else {
    QMessageBox::critical(this, tr("Erreur"), tr("Enregistrement impossible"));
  }
}
