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

#include "formauditreports.h"
#include "pcx_auditwithtreemodel.h"
#include "ui_formauditreports.h"
#include "utils.h"

FormAuditReports::FormAuditReports(QWidget *parent) : QWidget(parent), ui(new Ui::FormAuditReports) {
  ui->setupUi(this);
  ui->splitter->setStretchFactor(1, 1);
  model = nullptr;
  referenceNode = 1;
  updateListOfAudits();
}

FormAuditReports::~FormAuditReports() {
  delete ui;
  if (model != nullptr) {
    delete model;
    delete report;
  }
}

void FormAuditReports::populateLists() {
    ui->listTables->setModel(report->getTables().getListModelOfAvailableAuditTables(true));
    ui->listGraphics->setModel(report->getGraphics().getListModelOfAvailablePCAGRAPHICS());
}

void FormAuditReports::onListOfAuditsChanged() { updateListOfAudits(); }

void FormAuditReports::updateListOfAudits() {
  ui->comboListAudits->clear();

  QList<QPair<unsigned int, QString>> listOfAudits =
      PCx_Audit::getListOfAudits(PCx_Audit::ListAuditsMode::FinishedAuditsOnly);

  if (listOfAudits.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Créez et terminez un audit dans la fenêtre de gestion des audits"));
  }

  bool nonEmpty = !listOfAudits.isEmpty();
  this->setEnabled(nonEmpty);
  QPair<unsigned int, QString> p;
  foreach (p, listOfAudits) { ui->comboListAudits->insertItem(0, p.second, p.first); }
  ui->comboListAudits->setCurrentIndex(0);
  on_comboListAudits_activated(0);
}

void FormAuditReports::on_comboListAudits_activated(int index) {
  if (index == -1 || ui->comboListAudits->count() == 0) {
    return;
  }
  unsigned int selectedAuditId = ui->comboListAudits->currentData().toUInt();
  if (!(selectedAuditId > 0)) {
    qFatal("Assertion failed");
  }
  // qDebug()<<"Selected audit ID = "<<selectedAuditId;

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
  // QModelIndex rootIndex=model->getAttachedTree()->index(0,0);
  // ui->treeView->setCurrentIndex(rootIndex);

  ui->comboListTypes->clear();

  QList<QPair<unsigned int, QString>> listOfTypes = model->getAttachedTree()->getAllTypes();
  if (listOfTypes.isEmpty()) {
    qFatal("Assertion failed");
  }
  QPair<unsigned int, QString> p;
  foreach (p, listOfTypes) { ui->comboListTypes->addItem(p.second, p.first); }
  ui->comboListTypes->setCurrentIndex(0);
  populateLists();
}

void FormAuditReports::on_saveButton_clicked() {
  QItemSelectionModel *sel = ui->treeView->selectionModel();
  QModelIndexList selIndexes = sel->selectedIndexes();

  if (selIndexes.isEmpty()) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez au moins un noeud dans l'arborescence"));
    return;
  }

  QList<unsigned int> selectedNodes;
  foreach (const QModelIndex &idx, selIndexes) {
    selectedNodes.append(idx.data(PCx_TreeModel::NodeIdUserRole).toUInt());
  }

  // qDebug()<<"Selected nodes : "<<selectedNodes;
  //NOTE : useless since graphviz tree toc
   /*
   * QList<unsigned int> sortedSelectedNodes;
  if (ui->radioButtonBFS->isChecked()) {
    sortedSelectedNodes = model->getAttachedTree()->sortNodesBFS(selectedNodes);
  } else if (ui->radioButtonDFS->isChecked()) {
    sortedSelectedNodes = model->getAttachedTree()->sortNodesDFS(selectedNodes);
  } else {
    QMessageBox::warning(this, tr("Attention"), tr("Choisissez l'ordre d'affichage des noeuds "
                                                   "sélectionnés dans le rapport !"));
    return;
  }
  // qDebug()<<"Selected nodes (sorted) : "<<sortedSelectedNodes;
  */

  QModelIndexList selectedItemsTables = ui->listTables->selectionModel()->selectedIndexes();
  QModelIndexList selectedItemsGraphics = ui->listGraphics->selectionModel()->selectedIndexes();

  QList<PCx_Tables::PCATABLES> selectedTables;
  QList<PCx_Graphics::PCAGRAPHICS> selectedGraphics;

  foreach (const QModelIndex &item, selectedItemsTables) {
      selectedTables.append(static_cast<PCx_Tables::PCATABLES>(item.data(PCx_Tables::TableIdUserRole).toUInt()));
  }

  foreach (const QModelIndex &item, selectedItemsGraphics) {
      selectedGraphics.append(
          static_cast<PCx_Graphics::PCAGRAPHICS>(item.data(PCx_Graphics::GraphicIdUserRole).toUInt()));
  }

  if (selectedTables.isEmpty() && selectedGraphics.isEmpty()) {
      QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez au moins un tableau ou un graphique"));
      return;
  }

  QList<PCx_Tables::PCATABLES> modeIndependantTables;
  QList<PCx_Graphics::PCAGRAPHICS> modeIndependantGraphics;

  // Isolate mode-independant tables and graphics
  if (selectedTables.contains(PCx_Tables::PCATABLES::PCAT10)) {
    modeIndependantTables.append(PCx_Tables::PCATABLES::PCAT10);
    selectedTables.removeAll(PCx_Tables::PCATABLES::PCAT10);
  }
  if (selectedTables.contains(PCx_Tables::PCATABLES::PCAT11)) {
    modeIndependantTables.append(PCx_Tables::PCATABLES::PCAT11);
    selectedTables.removeAll(PCx_Tables::PCATABLES::PCAT11);
  }
  if (selectedTables.contains(PCx_Tables::PCATABLES::PCAT12)) {
    modeIndependantTables.append(PCx_Tables::PCATABLES::PCAT12);
    selectedTables.removeAll(PCx_Tables::PCATABLES::PCAT12);
  }
  if (selectedGraphics.contains(PCx_Graphics::PCAGRAPHICS::PCAG9)) {
    modeIndependantGraphics.append(PCx_Graphics::PCAGRAPHICS::PCAG9);
    selectedGraphics.removeAll(PCx_Graphics::PCAGRAPHICS::PCAG9);
  }

  /*qDebug()<<"Mode-independant selected tables = "<<modeIndependantTables;
qDebug()<<"Mode-independant selected graphics = "<<modeIndependantGraphics;

qDebug()<<"Mode-dependant selected tables = "<<selectedTables;
qDebug()<<"Mode-dependant selected graphics = "<<selectedGraphics;*/

  QList<MODES::DFRFDIRI> listModes;
  if (ui->checkBoxDF->isChecked()) {
    listModes.append(MODES::DFRFDIRI::DF);
  }
  if (ui->checkBoxRF->isChecked()) {
    listModes.append(MODES::DFRFDIRI::RF);
  }
  if (ui->checkBoxDI->isChecked()) {
    listModes.append(MODES::DFRFDIRI::DI);
  }
  if (ui->checkBoxRI->isChecked()) {
    listModes.append(MODES::DFRFDIRI::RI);
  }

  if (listModes.isEmpty() && (!selectedGraphics.isEmpty() || !selectedTables.isEmpty())) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez au moins un mode (dépenses/recettes "
                                                   "de fonctionnement/d'investissement)"));
    return;
  }

  QString fileName=chooseHTMLFileNameWithDialog();
  if(fileName==nullptr)return;
  QFileInfo fi(fileName);

  QString relativeNodesPath = fi.fileName() + "_files";
  QString absoluteNodesPath = fi.absoluteFilePath() + "_files";

  QFileInfo nodesDirInfo(absoluteNodesPath);

  if (!nodesDirInfo.exists()) {
    if (!fi.absoluteDir().mkdir(relativeNodesPath)) {
      QMessageBox::critical(this, tr("Attention"), tr("Création du dossier des noeuds impossible"));
      return;
    }
  } else {
    if (!nodesDirInfo.isWritable()) {
      QMessageBox::critical(this, tr("Attention"), tr("Écriture impossible dans le dossier des noeuds"));
      return;
    }
  }

  int maximumProgressValue = selIndexes.count() + 1;

  QProgressDialog progress(tr("Enregistrement du rapport en cours..."), tr("Annuler"), 0, maximumProgressValue, this);
  progress.setMinimumDuration(1000);

  progress.setWindowModality(Qt::ApplicationModal);

  progress.setValue(0);
  int i = 0;
  QSettings settings;

  report->getGraphics().setGraphicsWidth(settings.value("graphics/width", PCx_Graphics::DEFAULTWIDTH).toInt());
  report->getGraphics().setGraphicsHeight(settings.value("graphics/height", PCx_Graphics::DEFAULTHEIGHT).toInt());

  QElapsedTimer timer;
  timer.start();

  QMap<unsigned int,QUrl> nodeToFileName;

  //foreach (unsigned int selectedNode, sortedSelectedNodes) {
  foreach (unsigned int selectedNode, selectedNodes) {
    QString output = report->generateNodeHTMLHeader(selectedNode);
    output.append(QString("\n\n<h1 id='node%2'>%1</h1>")
                      .arg(model->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped())
                      .arg(selectedNode));

    if (!modeIndependantGraphics.isEmpty() || !modeIndependantTables.isEmpty()) {
      // Mode-independant
      output.append(report->generateHTMLAuditReportForNode(
          QList<PCx_Tables::PCAPRESETS>(), modeIndependantTables, modeIndependantGraphics, selectedNode,
          MODES::DFRFDIRI::DF, referenceNode, nullptr, nullptr));
    }
    foreach (MODES::DFRFDIRI mode, listModes) {
      output.append(report->generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS>(), selectedTables,
                                                           selectedGraphics, selectedNode, mode, referenceNode, nullptr, nullptr));
      if (progress.wasCanceled()) {
        goto cleanup;
      }
      output.append(QStringLiteral("\n<br><br><br>"));
    }
    if (!progress.wasCanceled()) {
      progress.setValue(++i);
    } else {
    cleanup:
      QDir dir(absoluteNodesPath);
      dir.removeRecursively();
      return;
    }
    output.append(QStringLiteral("\n\n</body></html>"));

    // BEGIN WRITE NODE FILE

    QString nodeUniqueName=generateUniqueFileName(".html");
    QString nodeAbsoluteFileName = absoluteNodesPath + "/" + nodeUniqueName;
    QString nodeRelativeFileName = relativeNodesPath + "/" + nodeUniqueName;
    nodeToFileName.insert(selectedNode,nodeRelativeFileName);
    QFile nodeFile(nodeAbsoluteFileName);

    if (!nodeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QMessageBox::critical(this, tr("Attention"),
                            tr("Ouverture du fichier impossible : %1").arg(nodeFile.errorString()));
      return;
    }

    QTextStream nodeStream(&nodeFile);
    nodeStream << output;
    nodeStream.flush();
    nodeFile.close();

    if (nodeStream.status() != QTextStream::Ok) {
      QMessageBox::critical(this, tr("Attention"), tr("Erreur d'enregistrement d'un noeud du rapport"));
      return;
    }
    // END WRITE NODE FILE
  }

  // Main html file
  QString mainOutput = PCx_Report::generateMainHTMLHeader();
  mainOutput.append(model->generateHTMLAuditTitle());
  mainOutput.append(report->generateSVGTOC(nodeToFileName));
  mainOutput.append(QStringLiteral("\n</body></html>"));

  if (!progress.wasCanceled()) {
    progress.setValue(maximumProgressValue - 1);
  } else {
    QDir dir(absoluteNodesPath);
    dir.removeRecursively();
    return;
  }

  QFile file(fileName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
    QDir dir(absoluteNodesPath);
    dir.removeRecursively();
    return;
  }

  QTextStream stream(&file);
  stream << mainOutput;
  stream.flush();
  file.close();

  progress.setValue(maximumProgressValue);
  if (stream.status() == QTextStream::Ok) {
      if (question(tr("Le rapport <b>%1</b> a bien été enregistré. Les noeuds sont stockées dans le dossier <b>%2</b>. "
                      "Voulez-vous ouvrir le rapport dans le navigateur ?")
                       .arg(fi.fileName().toHtmlEscaped(), relativeNodesPath.toHtmlEscaped())) == QMessageBox::Yes) {
          if (QDesktopServices::openUrl(QUrl("file://" + fi.absoluteFilePath(), QUrl::TolerantMode)) == false) {
              QMessageBox::warning(this, tr("Attention"), tr("Ouverture impossible"));
          }
      }

  } else {
      QMessageBox::critical(this, tr("Attention"), tr("Le rapport n'a pas pu être enregistré !"));
  }
}

void FormAuditReports::on_pushButtonPoidsRelatifs_clicked() {

    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT1), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT4), 1,
                                          Qt::MatchExactly));

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT8), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }

    QItemSelectionModel *listGraphicsSelectionModel = ui->listGraphics->selectionModel();
    QAbstractItemModel *listGraphicsModel = ui->listGraphics->model();

    indexes =
        listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                 static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG9), 1, Qt::MatchExactly);

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listGraphicsSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonBase100_clicked() {
    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT5), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT6), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonEvolution_clicked() {
    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT2BIS), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT3BIS), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }

    QItemSelectionModel *listGraphicsSelectionModel = ui->listGraphics->selectionModel();
    QAbstractItemModel *listGraphicsModel = ui->listGraphics->model();

    indexes =
        listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                 static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG1), 1, Qt::MatchExactly);

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG3), 1,
                                            Qt::MatchExactly));

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG5), 1,
                                            Qt::MatchExactly));

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG7), 1,
                                            Qt::MatchExactly));

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAHISTORY), 1,
                                            Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listGraphicsSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonEvolutionCumul_clicked() {
    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT2), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT3), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }

    QItemSelectionModel *listGraphicsSelectionModel = ui->listGraphics->selectionModel();
    QAbstractItemModel *listGraphicsModel = ui->listGraphics->model();

    indexes =
        listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                 static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG2), 1, Qt::MatchExactly);

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG4), 1,
                                            Qt::MatchExactly));

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG6), 1,
                                            Qt::MatchExactly));

    indexes.append(listGraphicsModel->match(listGraphicsModel->index(0, 0), PCx_Graphics::GraphicIdUserRole,
                                            static_cast<unsigned int>(PCx_Graphics::PCAGRAPHICS::PCAG8), 1,
                                            Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listGraphicsSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonJoursAct_clicked() {
    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT7), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT9), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonResultats_clicked() {
    QItemSelectionModel *listTablesSelectionModel = ui->listTables->selectionModel();
    QAbstractItemModel *listTablesModel = ui->listTables->model();

    QModelIndexList indexes =
        listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                               static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT10), 1, Qt::MatchExactly);

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT11), 1,
                                          Qt::MatchExactly));

    indexes.append(listTablesModel->match(listTablesModel->index(0, 0), PCx_Tables::TableIdUserRole,
                                          static_cast<unsigned int>(PCx_Tables::PCATABLES::PCAT12), 1,
                                          Qt::MatchExactly));

    if (indexes.isEmpty()) {
        qWarning() << "Problème de selection";
    } else {
        foreach (const QModelIndex &idx, indexes) {
            listTablesSelectionModel->select(idx, QItemSelectionModel::Toggle);
        }
    }
}

void FormAuditReports::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormAuditReports::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormAuditReports::on_pushButtonSelectAll_clicked() {
  ui->listTables->selectAll();
  ui->listGraphics->selectAll();
}

void FormAuditReports::on_pushButtonSelectNone_clicked() {
  ui->listTables->clearSelection();
  ui->listGraphics->clearSelection();
}

void FormAuditReports::on_pushButtonSelectType_clicked() {
  PCx_TreeModel *treeModel = model->getAttachedTree();
  unsigned int selectedType = ui->comboListTypes->currentData().toUInt();
  QModelIndexList indexOfTypes = treeModel->getIndexesOfNodesWithThisType(selectedType);

  foreach (const QModelIndex &index, indexOfTypes) {
      ui->treeView->selectionModel()->select(index, QItemSelectionModel::Toggle);
  }
}

void FormAuditReports::on_pushButtonSelectAllNodes_clicked() {
  ui->treeView->expandAll();
  ui->treeView->selectAll();
}

void FormAuditReports::on_pushButtonUnSelectAllNodes_clicked() { ui->treeView->clearSelection(); }

QSize FormAuditReports::sizeHint() const { return {900, 600}; }

void FormAuditReports::on_treeView_doubleClicked(const QModelIndex &index) {
    unsigned int potentialNewReferenceNode = index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    QString potentialName = model->getAttachedTree()->getNodeName(potentialNewReferenceNode).toHtmlEscaped();
    if (question(tr("Nouveau noeud de référence pour les calculs : <b>%1</b>. Confirmer ?").arg(potentialName)) ==
        QMessageBox::Yes) {
        referenceNode = potentialNewReferenceNode;
        ui->labelReference->setText("Ref : " + potentialName);
    }
}
