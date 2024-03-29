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

#include "formreportingreports.h"
#include "ui_formreportingreports.h"

FormReportingReports::FormReportingReports(QWidget *parent) : QWidget(parent), ui(new Ui::FormReportingReports) {
  ui->setupUi(this);
  model = nullptr;
  updateListOfReportings();
}

FormReportingReports::~FormReportingReports() {
  delete ui;
  if (model != nullptr) {
    delete model;
    delete report;
  }
}

void FormReportingReports::onListOfReportingsChanged() { updateListOfReportings(); }

void FormReportingReports::updateListOfReportings() {
  ui->comboListReportings->clear();

  QList<QPair<unsigned int, QString>> listOfReportings = PCx_Reporting::getListOfReportings();
  if (listOfReportings.isEmpty()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Commencez par créer et ajouter des données à un reporting dans la "
                                "fenêtre de gestion des reportings"));
  }
  bool nonEmpty = !listOfReportings.isEmpty();
  this->setEnabled(nonEmpty);
  QPair<unsigned int, QString> p;
  foreach (p, listOfReportings) { ui->comboListReportings->insertItem(0, p.second, p.first); }
  ui->comboListReportings->setCurrentIndex(0);
  on_comboListReportings_activated(0);
}

void FormReportingReports::updateTooltips() {
  QList<unsigned int> leaves = model->getAttachedTree()->getLeavesId();
  unsigned int nodeId = leaves.at(QRandomGenerator::global()->bounded(leaves.count()));
  ui->checkBoxS->setToolTip("<span style='font-size:8pt'>" + report->getTables().getPCRRFDF(nodeId) + "</span>");
  ui->checkBoxA->setToolTip("<span style='font-size:8pt'>" +
                            report->getTables().getPCRProvenance(nodeId, MODES::DFRFDIRI::DF) + "</span>");
  ui->checkBoxB->setToolTip("<span style='font-size:8pt'>" +
                            report->getTables().getPCRVariation(nodeId, MODES::DFRFDIRI::DF) + "</span>");
  ui->checkBoxC->setToolTip("<span style='font-size:8pt'>" +
                            report->getTables().getPCRUtilisation(nodeId, MODES::DFRFDIRI::DF) + "</span>");
  ui->checkBoxD->setToolTip("<span style='font-size:8pt'>" +
                            report->getTables().getPCRCycles(nodeId, MODES::DFRFDIRI::DF) + "</span>");
}

void FormReportingReports::on_comboListReportings_activated(int index) {
  if (index == -1 || ui->comboListReportings->count() == 0) {
    return;
  }
  unsigned int selectedReportingId = ui->comboListReportings->currentData().toUInt();

  if (model != nullptr) {
    delete model;
    delete report;
  }
  model = new PCx_ReportingWithTreeModel(selectedReportingId);
  report = new PCx_Report(model);
  QItemSelectionModel *m = ui->treeView->selectionModel();
  ui->treeView->setModel(model->getAttachedTree());
  delete m;
  ui->treeView->expandToDepth(1);

  ui->comboListTypes->clear();

  QList<QPair<unsigned int, QString>> listOfTypes = model->getAttachedTree()->getAllTypes();
  if (listOfTypes.isEmpty()) {
    qFatal("Assertion failed");
  }
  QPair<unsigned int, QString> p;
  foreach (p, listOfTypes) { ui->comboListTypes->addItem(p.second, p.first); }
  ui->comboListTypes->setCurrentIndex(0);
  updateTooltips();
}

void FormReportingReports::on_saveButton_clicked() {
  // TODO(unknown): for V2 : Order tables and graphics
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
   // NOTE : useless since graphviz tree TOC
  /*
  QList<unsigned int> sortedSelectedNodes;
  if (ui->radioButtonBFS->isChecked()) {
    sortedSelectedNodes = model->getAttachedTree()->sortNodesBFS(selectedNodes);
  } else if (ui->radioButtonDFS->isChecked()) {
    sortedSelectedNodes = model->getAttachedTree()->sortNodesDFS(selectedNodes);
  } else {
    QMessageBox::warning(this, tr("Attention"), tr("Choisissez l'ordre d'affichage des noeuds "
                                                   "sélectionnés dans le rapport !"));
    return;
  }
  */

  QList<PCx_Report::PCRPRESETS> selectedPresets;

  if (ui->checkBoxA->isChecked()) {
    selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_A);
  }

  if (ui->checkBoxB->isChecked()) {
    selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_B);
  }

  if (ui->checkBoxC->isChecked()) {
    selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_C);
  }

  if (ui->checkBoxD->isChecked()) {
    selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_D);
  }

  if (ui->checkBoxS->isChecked()) {
    selectedPresets.append(PCx_Report::PCRPRESETS::PCRPRESET_S);
  }

  if (selectedPresets.isEmpty()) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez au moins un élément à inclure"));
    return;
  }

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

  if (listModes.isEmpty() && !selectedPresets.isEmpty()) {
    QMessageBox::warning(this, tr("Attention"), tr("Sélectionnez au moins un mode (dépenses/recettes "
                                                   "de fonctionnement/d'investissement)"));
    return;
  }


  QString mainFileName=chooseHTMLFileNameWithDialog();
  if(mainFileName==nullptr)return;
  QFileInfo fi(mainFileName);

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

  // QElapsedTimer timer;
  // timer.start();

  QList<PCx_Report::PCRPRESETS> presetsSansS = selectedPresets;
  presetsSansS.removeAll(PCx_Report::PCRPRESETS::PCRPRESET_S);


  QMap<unsigned int,QUrl> nodeToFileName;

//foreach (unsigned int selectedNode, sortedSelectedNodes) {
  foreach (unsigned int selectedNode, selectedNodes) {
   QString output = report->generateNodeHTMLHeader(selectedNode);

    output.append(QString("\n\n<h1 id='node%2'>%1</h1>")
                      .arg(model->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped())
                      .arg(selectedNode));

    if (selectedPresets.contains(PCx_Report::PCRPRESETS::PCRPRESET_S)) {
      QList<PCx_Report::PCRPRESETS> tmpPreset = {PCx_Report::PCRPRESETS::PCRPRESET_S};
      output.append(report->generateHTMLReportingReportForNode(tmpPreset, selectedNode, MODES::DFRFDIRI::GLOBAL,
                                                               ui->checkBoxIncludeGraphics->isChecked(), nullptr, nullptr));
    }
    foreach (MODES::DFRFDIRI mode, listModes) {
      output.append(report->generateHTMLReportingReportForNode(presetsSansS, selectedNode, mode,
                                                               ui->checkBoxIncludeGraphics->isChecked(), nullptr, nullptr));
      if (progress.wasCanceled()) {
        goto cleanup;
      }
      output.append("\n<br><br><br>");
    }

    if (!progress.wasCanceled()) {
      progress.setValue(++i);
    } else {
    cleanup:
      QDir dir(absoluteNodesPath);
      dir.removeRecursively();
      return;
    }
    output.append("\n\n<br><br><br><br>\n</body>\n</html>");

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
  output.clear();
  // END WRITE NODE FILE

  }


  if (!progress.wasCanceled()) {
      progress.setValue(maximumProgressValue - 1);
  } else {
      QDir dir(absoluteNodesPath);
      dir.removeRecursively();
      return;
  }



  //Main HTML file

  QFile mainFile(mainFileName);
  if (!mainFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Attention"), tr("Ouverture du fichier impossible : %1").arg(mainFile.errorString()));
    QDir dir(absoluteNodesPath);
    dir.removeRecursively();
    return;
  }


  QString mainOutput = PCx_Report::generateMainHTMLHeader();
  mainOutput.append(model->generateHTMLReportingTitle());
  mainOutput.append(report->generateSVGTOC(nodeToFileName));


  QTextStream stream(&mainFile);
  stream << mainOutput;
  stream.flush();
  mainFile.close();


  progress.setValue(maximumProgressValue);
  if (stream.status() == QTextStream::Ok) {
    QMessageBox::information(this,
                             tr("Informations"),
                             tr("Le rapport a bien été enregistré, vous pouvez l'ouvrir avec votre "
                                "navigateur internet"));

  } else {
    QMessageBox::critical(this, tr("Attention"), tr("Le rapport n'a pas pu être enregistré !"));
  }
}

void FormReportingReports::on_pushButtonExpandAll_clicked() { ui->treeView->expandAll(); }

void FormReportingReports::on_pushButtonCollapseAll_clicked() {
  ui->treeView->collapseAll();
  ui->treeView->expandToDepth(0);
}

void FormReportingReports::on_pushButtonSelectType_clicked() {
  PCx_TreeModel *treeModel = model->getAttachedTree();
  unsigned int selectedType = ui->comboListTypes->currentData().toUInt();
  QModelIndexList indexOfTypes = treeModel->getIndexesOfNodesWithThisType(selectedType);

  foreach (const QModelIndex &index, indexOfTypes) {
    ui->treeView->selectionModel()->select(index, QItemSelectionModel::Select);
  }
}

void FormReportingReports::on_pushButtonSelectAllNodes_clicked() {
  ui->treeView->expandAll();
  ui->treeView->selectAll();
}

void FormReportingReports::on_pushButtonUnSelectAllNodes_clicked() { ui->treeView->clearSelection(); }

/*QSize FormReportingReports::sizeHint() const
{
    return QSize(900,600);
}*/

void FormReportingReports::on_pushButtonSelectAll_clicked() {
  ui->checkBoxA->setChecked(true);
  ui->checkBoxB->setChecked(true);
  ui->checkBoxC->setChecked(true);
  ui->checkBoxD->setChecked(true);
  ui->checkBoxS->setChecked(true);
}

void FormReportingReports::on_pushButtonSelectNone_clicked() {
  ui->checkBoxA->setChecked(false);
  ui->checkBoxB->setChecked(false);
  ui->checkBoxC->setChecked(false);
  ui->checkBoxD->setChecked(false);
  ui->checkBoxS->setChecked(false);
}
