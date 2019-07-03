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

#include "mainwindow.h"
#include "dialogabout.h"
#include "ui_mainwindow.h"
#include <QWhatsThis>

using namespace NUMBERSFORMAT;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setCentralWidget(ui->mdiArea);
  formEditTreeWin = nullptr;
  formManageAudits = nullptr;
  formEditAudit = nullptr;
  formAuditReports = nullptr;
  formManageReportings = nullptr;
  formReportingReports = nullptr;
  formAuditPrevisions = nullptr;
  formManagePrevisions = nullptr;
  whatThisAction = QWhatsThis::createAction();
  ui->toolBar->insertAction(nullptr, whatThisAction);

  QCoreApplication::setOrganizationName("GiPiLab");
  QCoreApplication::setApplicationName("ProtonClassicSuite");

  QSettings settings;
  if (settings.value("firstRun", true).toBool() == true) {
    ui->actionAbout->trigger();
    settings.setValue("firstRun", false);
  }
  restoreSettings();
  updateTitle();

  setMenusState();
  updateFormatModeAndDecimals();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete whatThisAction;
  delete ui;
}

void MainWindow::desactivateActions() {
  ui->actionNewDb->setEnabled(false);
  ui->actionOpenDb->setEnabled(false);
  ui->actionOptions->setEnabled(false);

  ui->menuAudits->setEnabled(false);
  ui->menuReporting->setEnabled(false);
  ui->menuBudgets->setEnabled(false);

  ui->actionEditAudit->setEnabled(false);
  ui->actionManageAudits->setEnabled(false);
  ui->actionManageTree->setEnabled(false);
  ui->actionQueries->setEnabled(false);
  ui->actionAuditReport->setEnabled(false);
  ui->actionExploreAudits->setEnabled(false);
  ui->actionBudgetElaboration->setEnabled(false);
  ui->actionExploreReportings->setEnabled(false);
  ui->actionManageReportings->setEnabled(false);
  ui->actionReportingGraphics->setEnabled(false);
  ui->actionReportingReport->setEnabled(false);
  ui->actionReportingOverview->setEnabled(false);
  ui->actionReportingSupervision->setEnabled(false);
  ui->actionManagePrevisions->setEnabled(false);

  ui->actionTreemap->setEnabled(false);
}

void MainWindow::restoreSettings() {

    QSettings settings;

    if (settings.value("mainwindow/maximised", false).toBool() == true) {
        setWindowState(Qt::WindowMaximized);
    }
    this->move(settings.value("mainwindow/position", QVariant(this->pos())).toPoint());
    this->resize(settings.value("mainwindow/size", QVariant(this->size())).toSize());

    recentDb = settings.value("database/recentdb").toString();
    if (!recentDb.isEmpty()) {
        QFileInfo fi(recentDb);
        if (fi.exists() && fi.isFile() && fi.isReadable() && fi.isWritable()) {
            loadDb(recentDb);
        }
    }
}

void MainWindow::saveSettings() {
  QSettings settings;
  settings.setValue("mainwindow/maximised", this->isMaximized());
  settings.setValue("mainwindow/position", this->pos());
  settings.setValue("mainwindow/size", this->size());

  if (!recentDb.isEmpty()) {
    settings.setValue("database/recentdb", recentDb);
  }
}

void MainWindow::setMenusState() {
  desactivateActions();

  ui->actionNewDb->setEnabled(true);
  ui->actionOpenDb->setEnabled(true);
  ui->actionOptions->setEnabled(true);

  if (!QSqlDatabase::database().databaseName().isEmpty()) {
    ui->menuAudits->setEnabled(true);
    ui->actionManageTree->setEnabled(true);
    ui->actionEditAudit->setEnabled(true);
    ui->actionManageAudits->setEnabled(true);
    ui->actionQueries->setEnabled(true);
    ui->actionAuditReport->setEnabled(true);
    ui->actionExploreAudits->setEnabled(true);
    ui->actionTreemap->setEnabled(true);
  }

  ui->actionNewDb->setEnabled(true);
  ui->actionOpenDb->setEnabled(true);
  ui->actionOptions->setEnabled(true);

  if (!QSqlDatabase::database().databaseName().isEmpty()) {
    ui->menuBudgets->setEnabled(true);
    ui->actionManageTree->setEnabled(true);
    ui->actionEditAudit->setEnabled(true);
    ui->actionManageAudits->setEnabled(true);
    ui->actionManagePrevisions->setEnabled(true);
    ui->actionBudgetElaboration->setEnabled(true);
  }

  ui->actionNewDb->setEnabled(true);
  ui->actionOpenDb->setEnabled(true);
  ui->actionOptions->setEnabled(true);

  if (!QSqlDatabase::database().databaseName().isEmpty()) {
    ui->menuReporting->setEnabled(true);
    ui->actionManageTree->setEnabled(true);
    ui->actionExploreReportings->setEnabled(true);
    ui->actionManageReportings->setEnabled(true);
    ui->actionReportingGraphics->setEnabled(true);
    ui->actionReportingReport->setEnabled(true);
    ui->actionReportingOverview->setEnabled(true);
    ui->actionReportingSupervision->setEnabled(true);
  }
}

void MainWindow::on_actionManageTree_triggered() {
  // Only one instance allowed
  if (formEditTreeWin == nullptr) {
    formEditTreeWin = new FormEditTree(this);
    formEditTreeWin->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formEditTreeWin);
    subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
    formEditTreeWin->show();
    if (formManageAudits != nullptr) {
      connect(formEditTreeWin, &FormEditTree::listOfTreeChanged, formManageAudits, &FormManageAudits::onLOTchanged);
    }
    if (formManageReportings != nullptr) {
      connect(formEditTreeWin, &FormEditTree::listOfTreeChanged, formManageReportings,
              &FormManageReportings::onLOTchanged);
    }
    connect(formEditTreeWin, &QObject::destroyed, this, &MainWindow::onFormEditTreeWindowsDestroyed);
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formEditTreeWin->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (question(tr("Voulez-vous vraiment quitter ?")) == QMessageBox::Yes) {
    // To avoid segfault with removeAt
    ui->mdiArea->closeAllSubWindows();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::on_actionExit_triggered() { this->close(); }

void MainWindow::onFormEditTreeWindowsDestroyed() {
  formEditTreeWin = nullptr;
  //  qDebug()<<"FormEditTree window closed";
}

void MainWindow::onFormManageAuditsWindowsDestroyed() {
  formManageAudits = nullptr;
  // qDebug()<<"FormManageAudits window closed";
}

void MainWindow::onFormManagePrevisionsWindowsDestroyed() { formManagePrevisions = nullptr; }

void MainWindow::onFormManageReportingsWindowsDestroyed() {
  formManageReportings = nullptr;
  // qDebug()<<"FormManageReporting window closed";
}

void MainWindow::onFormEditAuditWindowsDestroyed() {
  formEditAudit = nullptr;
  // qDebug()<<"FormEditAudit window closed";
}

void MainWindow::onFormReportsWindowsDestroyed() {
  formAuditReports = nullptr;
  // qDebug()<<"FormReports window closed";
}

void MainWindow::onFormAuditPrevisionsWindowsDestroyed() { formAuditPrevisions = nullptr; }

void MainWindow::onFormReportingReportsWindowsDestroyed() {
  formReportingReports = nullptr;
  // qDebug()<<"FormReportingReports window closed";
}

void MainWindow::onFormAuditExploreWindowsDestroyed(QObject *obj) {

  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  int index = listOfFormAuditExplore.indexOf(static_cast<FormAuditExplore *>(obj));
  Q_ASSERT(index != -1);
  int size = listOfFormAuditExplore.size();
  Q_ASSERT(size > 0);
  listOfFormAuditExplore.removeAt(index);
  Q_ASSERT(listOfFormAuditExplore.size() == size - 1);
}

void MainWindow::onFormAuditTreemapWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  int index = listOfFormAuditTreemap.indexOf(static_cast<FormAuditTreemap *>(obj));
  listOfFormAuditTreemap.removeAt(index);
}

void MainWindow::onFormQueriesWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  listOfFormQueries.removeAt(listOfFormQueries.indexOf(static_cast<FormQueries *>(obj)));
  // qDebug()<<"FormQueries window closed, remaining :"<<listOfFormQueries;
}

void MainWindow::on_actionManageAudits_triggered() {
  if (formManageAudits == nullptr) {
    formManageAudits = new FormManageAudits(this);
    formManageAudits->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formManageAudits);
    subWin->setWindowIcon(QIcon(":/icons/icons/manage_sources.png"));
    formManageAudits->show();
    connect(formManageAudits, &QObject::destroyed, this, &MainWindow::onFormManageAuditsWindowsDestroyed);

    if (formEditTreeWin != nullptr) {
      connect(formEditTreeWin, &FormEditTree::listOfTreeChanged, formManageAudits, &FormManageAudits::onLOTchanged);
    }

    if (formEditAudit != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formEditAudit,
              &FormEditAudit::onListOfAuditsChanged);
      connect(formManageAudits, &FormManageAudits::auditDataUpdated, formEditAudit, &FormEditAudit::onAuditDataUpdated);
    }

    if (formAuditReports != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formAuditReports,
              &FormAuditReports::onListOfAuditsChanged);
    }

    if (formManageReportings != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formManageReportings,
              &FormManageReportings::onListOfAuditsChanged);
    }
    if (formAuditPrevisions != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formAuditPrevisions,
              &FormAuditPrevisions::onListOfPrevisionsChanged);
    }
    if (formManagePrevisions != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formManagePrevisions,
              &FormManagePrevisions::onListOfAuditsChanged);
      connect(formManagePrevisions, &FormManagePrevisions::listOfAuditsChanged, formManageAudits,
              &FormManageAudits::onListOfAuditsChanged);
    }
    foreach (FormAuditExplore *dlg, listOfFormAuditExplore) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormAuditExplore::onListOfAuditsChanged);
    }
    foreach (FormQueries *dlg, listOfFormQueries) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormQueries::onListOfAuditsChanged);
    }
    foreach (FormAuditTreemap *dlg, listOfFormAuditTreemap) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormAuditTreemap::onListOfAuditsChanged);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formManageAudits->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionEditAudit_triggered() {
  if (formEditAudit == nullptr) {
    formEditAudit = new FormEditAudit(this);
    formEditAudit->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formEditAudit);
    subWin->setWindowIcon(QIcon(":/icons/icons/table_edit.png"));

    formEditAudit->show();
    connect(formEditAudit, &QObject::destroyed, this, &MainWindow::onFormEditAuditWindowsDestroyed);

    if (formManageAudits != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formEditAudit,
              &FormEditAudit::onListOfAuditsChanged);
      connect(formManageAudits, &FormManageAudits::auditDataUpdated, formEditAudit, &FormEditAudit::onAuditDataUpdated);
    }
    if (formManageReportings != nullptr) {
      connect(formManageReportings, &FormManageReportings::auditDataUpdated, formEditAudit,
              &FormEditAudit::onAuditDataUpdated);
    }
    if (formManagePrevisions != nullptr) {
      connect(formManagePrevisions, &FormManagePrevisions::listOfAuditsChanged, formEditAudit,
              &FormEditAudit::onListOfAuditsChanged);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formEditAudit->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::updateTitle() {
  QFileInfo fi(QSqlDatabase::database().databaseName());
  setWindowTitle("ProtonClassicSuite - " + fi.baseName());
}

void MainWindow::on_actionNewDb_triggered() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  if (!recentDb.isEmpty()) {
    QFileInfo fi(recentDb);
    QDir dir = fi.dir();
    if (dir.isReadable()) {
      fileDialog.setDirectory(dir);
    }
  }

  QString fileName = fileDialog.getSaveFileName(this, tr("Nouvelle base de données"), "",
                                                tr("Bases de données ProtonClassicSuite (*.pcxdb)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFileInfo fi(fileName);
  if (fi.suffix().compare("pcxdb", Qt::CaseInsensitive)) {
    fileName.append(".pcxdb");
  }
  fi = QFileInfo(fileName);

  QFile newFile(fileName);

  if (newFile.exists()) {
    if (!newFile.remove()) {
      QMessageBox::critical(this, tr("Attention"), newFile.errorString());
      return;
    }
  } else {
    if (!newFile.open(QIODevice::ReadWrite)) {
      QMessageBox::critical(this, tr("Attention"), tr("Le fichier n'est pas accessible en lecture/écriture"));
      return;
    }
    newFile.close();
  }

  ui->mdiArea->closeAllSubWindows();
  loadDb(fileName);
  initializeNewDb();
  recentDb = fileName;
  updateTitle();
  setMenusState();

  QMessageBox::information(this, tr("Pour continuer"),
                           tr("Vous devez maintenant créer ou importer un arbre représentant la "
                              "structure que vous souhaitez analyser, dans la fenêtre <b>gestion "
                              "des arbres</b>"));
}

void MainWindow::on_actionOpenDb_triggered() {
  QFileDialog fileDialog;
  fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  if (!recentDb.isEmpty()) {
    QFileInfo fi(recentDb);
    QDir dir = fi.dir();
    if (dir.isReadable()) {
      fileDialog.setDirectory(dir);
    }
  }

  QString fileName = fileDialog.getOpenFileName(this, tr("Ouvrir une base de données"), "",
                                                tr("Bases de données ProtonClassicSuite (*.pcxdb)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFileInfo fi(fileName);
  if (!fi.isWritable() || !fi.isFile() || !fi.isReadable()) {
    QMessageBox::critical(this, tr("Attention"), tr("Le fichier n'est pas accessible en lecture/écriture"));
    return;
  }

  ui->mdiArea->closeAllSubWindows();
  loadDb(fileName);
  recentDb = fileName;
  updateTitle();
  setMenusState();
}

void MainWindow::on_actionAuditReport_triggered() {
  if (formAuditReports == nullptr) {
    formAuditReports = new FormAuditReports(this);
    formAuditReports->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formAuditReports);
    subWin->setWindowIcon(QIcon(":/icons/icons/report.png"));
    formAuditReports->show();
    connect(formAuditReports, &QObject::destroyed, this, &MainWindow::onFormReportsWindowsDestroyed);

    if (formManageAudits != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formAuditReports,
              &FormAuditReports::onListOfAuditsChanged);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formAuditReports->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionOptions_triggered() {
  DialogOptions dialogOptions(this);
  if (dialogOptions.exec() == QDialog::Accepted) {
    if (formEditTreeWin != nullptr) {
      formEditTreeWin->updateRandomButtonVisibility();
    }
    if (formManageAudits != nullptr) {
      formManageAudits->updateRandomButtonVisibility();
    }
    if (formManageReportings != nullptr) {
      formManageReportings->updateRandomVisibility();
    }
    if (formAuditPrevisions != nullptr) {
      formAuditPrevisions->onSettingsChanged();
    }
    foreach (FormQueries *q, listOfFormQueries) { q->onColorChanged(); }
    foreach (FormAuditExplore *form, listOfFormAuditExplore) { form->onSettingsChanged(); }
    foreach (FormReportingExplore *form, listOfFormReportingExplore) { form->onSettingsChanged(); }
  }
}

void MainWindow::on_actionQueries_triggered() {
  FormQueries *dlg = new FormQueries(this);
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/queries.png"));
  dlg->show();

  foreach (const FormQueries *f, listOfFormQueries) {
    connect(dlg, &FormQueries::listOfQueriesChanged, f, &FormQueries::onListOfQueriesChanged);
    connect(f, &FormQueries::listOfQueriesChanged, dlg, &FormQueries::onListOfQueriesChanged);
  }

  listOfFormQueries.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormQueriesWindowsDestroyed);

  if (formManageAudits != nullptr) {
    connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormQueries::onListOfAuditsChanged);
  }
}

void MainWindow::on_actionAbout_triggered() {
    DialogAbout dlg(this);
    dlg.exec();
}

void MainWindow::on_actionManageReportings_triggered() {
  if (formManageReportings == nullptr) {
    formManageReportings = new FormManageReportings(this);
    formManageReportings->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formManageReportings);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingManage.png"));
    formManageReportings->show();
    connect(formManageReportings, &QObject::destroyed, this, &MainWindow::onFormManageReportingsWindowsDestroyed);

    if (formEditTreeWin != nullptr) {
      connect(formEditTreeWin, &FormEditTree::listOfTreeChanged, formManageReportings,
              &FormManageReportings::onLOTchanged);
    }
    if (formManageAudits != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formManageReportings,
              &FormManageReportings::onListOfAuditsChanged);
    }
    if (formEditAudit != nullptr) {
      connect(formManageReportings, &FormManageReportings::auditDataUpdated, formEditAudit,
              &FormEditAudit::onAuditDataUpdated);
    }

    if (formReportingReports != nullptr) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, formReportingReports,
              &FormReportingReports::onListOfReportingsChanged);
    }

    foreach (FormReportingOverview *dlg, listOfFormReportingOverview) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
              &FormReportingOverview::onListOfReportingsChanged);
      connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
              &FormReportingOverview::onReportingDataChanged);
    }

    foreach (FormReportingSupervision *dlg, listOfFormReportingSupervision) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
              &FormReportingSupervision::onListOfReportingsChanged);
      connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
              &FormReportingSupervision::onReportingDataChanged);
    }
    foreach (FormReportingGraphics *dlg, listOfFormReportingGraphics) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
              &FormReportingGraphics::onListOfReportingsChanged);
      connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
              &FormReportingGraphics::onReportingDataChanged);
    }
    foreach (FormReportingExplore *dlg, listOfFormReportingExplore) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
              &FormReportingExplore::onListOfReportingsChanged);
      connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
              &FormReportingExplore::onReportingDataChanged);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formManageReportings->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionReportingOverview_triggered() {
  FormReportingOverview *dlg = new FormReportingOverview();
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/reportingOverview.png"));
  dlg->show();
  listOfFormReportingOverview.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormReportingTablesWindowsDestroyed);

  if (formManageReportings != nullptr) {
    connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
            &FormReportingOverview::onListOfReportingsChanged);
    connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
            &FormReportingOverview::onReportingDataChanged);
  }
}

void MainWindow::onFormReportingTablesWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  listOfFormReportingOverview.removeAt(listOfFormReportingOverview.indexOf(static_cast<FormReportingOverview *>(obj)));
}

void MainWindow::onFormReportingSupervisionWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  listOfFormReportingSupervision.removeAt(
      listOfFormReportingSupervision.indexOf(static_cast<FormReportingSupervision *>(obj)));
}

void MainWindow::onFormReportingGraphicsWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  listOfFormReportingGraphics.removeAt(listOfFormReportingGraphics.indexOf(static_cast<FormReportingGraphics *>(obj)));
}

void MainWindow::onFormReportingExploreWindowsDestroyed(QObject *obj) {
  // WARNING : do not use dynamic_cast or qobject_cast, as the pointer is already destroyed here
  listOfFormReportingExplore.removeAt(listOfFormReportingExplore.indexOf(static_cast<FormReportingExplore *>(obj)));
}

void MainWindow::on_actionReportingSupervision_triggered() {
  FormReportingSupervision *dlg = new FormReportingSupervision();
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/reportingForecast.png"));
  dlg->show();
  listOfFormReportingSupervision.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormReportingSupervisionWindowsDestroyed);

  if (formManageReportings != nullptr) {
    connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
            &FormReportingSupervision::onListOfReportingsChanged);
    connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
            &FormReportingSupervision::onReportingDataChanged);
  }
}

void MainWindow::on_actionReportingGraphics_triggered() {
  FormReportingGraphics *dlg = new FormReportingGraphics();
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/reportingGraphics.png"));
  dlg->show();
  listOfFormReportingGraphics.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormReportingGraphicsWindowsDestroyed);

  if (formManageReportings != nullptr) {
    connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
            &FormReportingGraphics::onListOfReportingsChanged);
    connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
            &FormReportingGraphics::onReportingDataChanged);
  }
}

void MainWindow::on_actionExploreReportings_triggered() {
  FormReportingExplore *dlg = new FormReportingExplore();
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/reportingExplore.png"));
  dlg->show();
  listOfFormReportingExplore.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormReportingExploreWindowsDestroyed);

  if (formManageReportings != nullptr) {
    connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, dlg,
            &FormReportingExplore::onListOfReportingsChanged);
    connect(formManageReportings, &FormManageReportings::reportingDataUpdated, dlg,
            &FormReportingExplore::onReportingDataChanged);
  }
}

void MainWindow::on_actionReportingReport_triggered() {
  if (formReportingReports == nullptr) {
    formReportingReports = new FormReportingReports(this);
    formReportingReports->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formReportingReports);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingReport.png"));
    formReportingReports->show();
    connect(formReportingReports, &QObject::destroyed, this, &MainWindow::onFormReportingReportsWindowsDestroyed);

    if (formManageReportings != nullptr) {
      connect(formManageReportings, &FormManageReportings::listOfReportingsChanged, formReportingReports,
              &FormReportingReports::onListOfReportingsChanged);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formReportingReports->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionExploreAudits_triggered() {
  FormAuditExplore *dlg = new FormAuditExplore();
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/exploreAudit.png"));

  dlg->show();
  listOfFormAuditExplore.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormAuditExploreWindowsDestroyed);

  if (formManageAudits != nullptr) {
    connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormAuditExplore::onListOfAuditsChanged);
  }
}

void MainWindow::on_actionCascade_triggered() { ui->mdiArea->cascadeSubWindows(); }

void MainWindow::on_actionCloseAllSubwin_triggered() { ui->mdiArea->closeAllSubWindows(); }

void MainWindow::on_actionBudgetElaboration_triggered() {
  if (formAuditPrevisions == nullptr) {
    formAuditPrevisions = new FormAuditPrevisions(this);
    formAuditPrevisions->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formAuditPrevisions);
    subWin->setWindowIcon(QIcon(":/icons/icons/editPrevisions.png"));
    formAuditPrevisions->show();
    connect(formAuditPrevisions, &QObject::destroyed, this, &MainWindow::onFormAuditPrevisionsWindowsDestroyed);

    if (formManagePrevisions != nullptr) {
      connect(formManagePrevisions, &FormManagePrevisions::listOfPrevisionsChanged, formAuditPrevisions,
              &FormAuditPrevisions::onListOfPrevisionsChanged);
      connect(formAuditPrevisions, &FormAuditPrevisions::previsionUpdated, formManagePrevisions,
              &FormManagePrevisions::onSelectedPrevisionUpdated);
    }
  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formAuditPrevisions->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionManagePrevisions_triggered() {
  if (formManagePrevisions == nullptr) {
    formManagePrevisions = new FormManagePrevisions(this);
    formManagePrevisions->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(formManagePrevisions);
    subWin->setWindowIcon(QIcon(":/icons/icons/previsionManage.png"));
    formManagePrevisions->show();
    connect(formManagePrevisions, &QObject::destroyed, this, &MainWindow::onFormManagePrevisionsWindowsDestroyed);

    if (formManageAudits != nullptr) {
      connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, formManagePrevisions,
              &FormManagePrevisions::onListOfAuditsChanged);
      connect(formManagePrevisions, &FormManagePrevisions::listOfAuditsChanged, formManageAudits,
              &FormManageAudits::onListOfAuditsChanged);
    }
    if (formEditAudit != nullptr) {
      connect(formManagePrevisions, &FormManagePrevisions::listOfAuditsChanged, formEditAudit,
              &FormEditAudit::onListOfAuditsChanged);
    }

    if (formAuditPrevisions != nullptr) {
      connect(formManagePrevisions, &FormManagePrevisions::listOfPrevisionsChanged, formAuditPrevisions,
              &FormAuditPrevisions::onListOfPrevisionsChanged);
      connect(formAuditPrevisions, &FormAuditPrevisions::previsionUpdated, formManagePrevisions,
              &FormManagePrevisions::onSelectedPrevisionUpdated);
    }

  } else {
    QList<QMdiSubWindow *> listSubWin = ui->mdiArea->subWindowList();
    foreach (QMdiSubWindow *win, listSubWin) {
      if (win->windowTitle() == formManagePrevisions->windowTitle()) {
        win->setFocus();
        break;
      }
    }
  }
}

void MainWindow::on_actionTreemap_triggered() {
  FormAuditTreemap *dlg = new FormAuditTreemap(this);
  dlg->setAttribute(Qt::WA_DeleteOnClose);

  QMdiSubWindow *subWin = ui->mdiArea->addSubWindow(dlg);
  subWin->setWindowIcon(QIcon(":/icons/icons/treemap.png"));

  dlg->show();
  listOfFormAuditTreemap.append(dlg);

  connect(dlg, &QObject::destroyed, this, &MainWindow::onFormAuditTreemapWindowsDestroyed);

  if (formManageAudits != nullptr) {
    connect(formManageAudits, &FormManageAudits::listOfAuditsChanged, dlg, &FormAuditTreemap::onListOfAuditsChanged);
  }
}
