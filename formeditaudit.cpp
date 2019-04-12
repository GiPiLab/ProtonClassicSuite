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

#include "formeditaudit.h"
#include "formauditinfos.h"
#include "pcx_tables.h"
#include "ui_formeditaudit.h"
#include "utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QTextDocument>
#include <QtGlobal>

FormEditAudit::FormEditAudit(QWidget *parent) : QWidget(parent), ui(new Ui::FormEditAudit) {
  ui->setupUi(this);
  selectedNode = 0;
  ui->splitter->setStretchFactor(1, 1);
  ui->tableViewDF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableViewRF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableViewDI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableViewRI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  auditModel = nullptr;

  delegateDF = new auditDataDelegate(ui->tableViewDF);
  delegateRF = new auditDataDelegate(ui->tableViewRF);
  delegateDI = new auditDataDelegate(ui->tableViewDI);
  delegateRI = new auditDataDelegate(ui->tableViewRI);

  ui->tableViewDF->setItemDelegate(delegateDF);
  ui->tableViewRF->setItemDelegate(delegateRF);
  ui->tableViewDI->setItemDelegate(delegateDI);
  ui->tableViewRI->setItemDelegate(delegateRI);

  updateListOfAudits();
}

FormEditAudit::~FormEditAudit() {
  delete ui;
  delete auditModel;
  delete (delegateDF);
  delete (delegateRF);
  delete (delegateDI);
  delete (delegateRI);
}

void FormEditAudit::onListOfAuditsChanged() { updateListOfAudits(); }

void FormEditAudit::updateListOfAudits() {
  ui->comboListAudits->clear();

  // QList<QPair<unsigned int,QString>
  // >listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::UnFinishedAuditsOnly);
  QList<QPair<unsigned int, QString>> listOfAudits = PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::AllAudits);
  bool nonEmpty = !listOfAudits.isEmpty();

  if (listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Créez d'abord un audit dans la fenêtre de gestion des audits"));
  } else {
    QMessageBox::information(this, tr("Information"),
                             tr("Les données sont modifiables pour les audits non terminés. Les "
                                "valeurs se saisissent uniquement aux <b>feuilles</b> de l'arbre et "
                                "les sommes sont mises à jour dynamiquement vers la racine"));
  }

  ui->splitter->setEnabled(nonEmpty);

  QPair<unsigned int, QString> p;
  foreach (p, listOfAudits) { ui->comboListAudits->insertItem(0, p.second, p.first); }
  ui->comboListAudits->setCurrentIndex(0);
  on_comboListAudits_activated(0);
}

QSize FormEditAudit::sizeHint() const { return {900, 400}; }

void FormEditAudit::on_comboListAudits_activated(int index) {
  if (index == -1 || ui->comboListAudits->count() == 0) {
    return;
  }
  unsigned int selectedAuditId = ui->comboListAudits->currentData().toUInt();
  // qDebug()<<"Selected audit ID = "<<selectedAuditId;
  delete auditModel;

  QItemSelectionModel *m = ui->treeView->selectionModel();
  // Read-write audit model
  auditModel = new PCx_EditableAuditModel(selectedAuditId, nullptr);
  ui->treeView->setModel(auditModel->getAttachedTree());
  delete m;
  ui->treeView->expandToDepth(1);

  ui->tableViewDF->setModel(auditModel->getTableModelDF());
  ui->tableViewDF->hideColumn(0);
  ui->tableViewDF->hideColumn(1);
  ui->tableViewRF->setModel(auditModel->getTableModelRF());
  ui->tableViewRF->hideColumn(0);
  ui->tableViewRF->hideColumn(1);
  ui->tableViewDI->setModel(auditModel->getTableModelDI());
  ui->tableViewDI->hideColumn(0);
  ui->tableViewDI->hideColumn(1);
  ui->tableViewRI->setModel(auditModel->getTableModelRI());
  ui->tableViewRI->hideColumn(0);
  ui->tableViewRI->hideColumn(1);

  // Roots
  auditModel->getTableModelDF()->setFilter(QString("id_node=1"));
  auditModel->getTableModelRF()->setFilter(QString("id_node=1"));
  auditModel->getTableModelDI()->setFilter(QString("id_node=1"));
  auditModel->getTableModelRI()->setFilter(QString("id_node=1"));
  ui->label->setText(auditModel->getAttachedTree()->index(0, 0).data().toString());

  ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui->tableViewDF->horizontalHeader()->setSectionResizeMode(
      static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE), QHeaderView::Fixed);
  ui->tableViewRF->horizontalHeader()->setSectionResizeMode(
      static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE), QHeaderView::Fixed);
  ui->tableViewDI->horizontalHeader()->setSectionResizeMode(
      static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE), QHeaderView::Fixed);
  ui->tableViewRI->horizontalHeader()->setSectionResizeMode(
      static_cast<int>(PCx_EditableAuditModel::COLINDEXES::COL_ANNEE), QHeaderView::Fixed);

  QModelIndex rootIndex = auditModel->getAttachedTree()->index(0, 0);
  ui->treeView->setCurrentIndex(rootIndex);
  on_treeView_clicked(rootIndex);
}

void FormEditAudit::on_treeView_clicked(const QModelIndex &index) {
  selectedNode = index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
  if (selectedNode == 0) {
    qFatal("Assertion failed");
  }

  auditModel->getTableModelDF()->setFilter(QString("id_node=%1").arg(selectedNode));
  auditModel->getTableModelRF()->setFilter(QString("id_node=%1").arg(selectedNode));
  auditModel->getTableModelDI()->setFilter(QString("id_node=%1").arg(selectedNode));
  auditModel->getTableModelRI()->setFilter(QString("id_node=%1").arg(selectedNode));
  bool isLeaf = auditModel->getAttachedTree()->isLeaf(selectedNode);
  bool isFinished = auditModel->isFinished();

  if (isLeaf) {
    if (!isFinished) {
      ui->tableViewDF->setEditTriggers(QAbstractItemView::AllEditTriggers);
      ui->tableViewRF->setEditTriggers(QAbstractItemView::AllEditTriggers);
      ui->tableViewDI->setEditTriggers(QAbstractItemView::AllEditTriggers);
      ui->tableViewRI->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
  } else {
    ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }

  ui->label->setText(index.data().toString());
}

void FormEditAudit::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormEditAudit::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormEditAudit::on_statsButton_clicked() {
  auto *infos = new FormAuditInfos(auditModel->getAuditId(), this);
  infos->setAttribute(Qt::WA_DeleteOnClose);
  auto *mdiSubWin = dynamic_cast<QMdiSubWindow *>(this->parentWidget());
  QMdiArea *mdiArea = mdiSubWin->mdiArea();
  mdiArea->addSubWindow(infos);
  infos->show();
}

void FormEditAudit::onAuditDataUpdated(unsigned int auditId) {
  if (auditModel == nullptr) {
    return;
  }
  if (auditModel->getAuditId() == auditId) {
    if (ui->treeView->currentIndex().isValid()) {
      on_treeView_clicked(ui->treeView->currentIndex());
    } else {
      QModelIndex rootIndex = auditModel->getAttachedTree()->index(0, 0);
      on_treeView_clicked(rootIndex);
    }
  }
}

void FormEditAudit::on_pushButtonExportHTML_clicked() {
  QString out = auditModel->generateHTMLHeader();
  out.append(auditModel->generateHTMLAuditTitle());

  MODES::DFRFDIRI mode = MODES::DFRFDIRI::DF;

  if (ui->tabWidget->currentWidget() == ui->tabDF) {
    mode = MODES::DFRFDIRI::DF;
  } else if (ui->tabWidget->currentWidget() == ui->tabRF) {
    mode = MODES::DFRFDIRI::RF;
  } else if (ui->tabWidget->currentWidget() == ui->tabDI) {
    mode = MODES::DFRFDIRI::DI;
  } else if (ui->tabWidget->currentWidget() == ui->tabRI) {
    mode = MODES::DFRFDIRI::RI;
  }

  PCx_Tables tables(auditModel);

  out.append(tables.getPCARawData(selectedNode, mode));
  out.append("</body></html>");

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

  if (fi.exists() && (!fi.isFile() || !fi.isWritable())) {
    QMessageBox::critical(this, tr("Attention"), tr("Fichier non accessible en écriture"));
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    return;
  }

  QSettings settings;
  QString settingStyle = settings.value("output/style", "CSS").toString();
  if (settingStyle == "INLINE") {
    QTextDocument doc;
    doc.setHtml(out);

    out = doc.toHtml("utf-8");
    out.replace(" -qt-block-indent:0;", "");
  }

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << out;
  stream.flush();
  file.close();
  if (stream.status() == QTextStream::Ok) {
    QMessageBox::information(this, tr("Information"),
                             tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName().toHtmlEscaped()));
  } else {
    QMessageBox::critical(this, tr("Attention"), tr("Le document n'a pas pu être enregistré !"));
  }
}
