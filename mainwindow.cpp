#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogabout.h"

using namespace NUMBERSFORMAT;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
    formEditTreeWin=nullptr;
    formManageAudits=nullptr;
    formEditAudit=nullptr;
    formReports=nullptr;
    formManageReportings=nullptr;
    formReportingReports=nullptr;

    restoreSettings();
    updateTitle();
    setMenusState();
    updateFormatModeAndDecimals();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::restoreSettings(void)
{
    QCoreApplication::setOrganizationName("GiPiLab");
    QCoreApplication::setApplicationName("ProtonClassicSuite");
    QSettings settings;
    if(settings.value("mainwindow/maximised",false).toBool()==true)
        setWindowState(Qt::WindowMaximized);
    this->move(settings.value("mainwindow/position",QVariant(this->pos())).toPoint());
    this->resize(settings.value("mainwindow/size",QVariant(this->size())).toSize());

    recentDb=settings.value("database/recentdb").toString();
    if(!recentDb.isEmpty())
    {
        QFileInfo fi(recentDb);
        if(fi.exists() && fi.isFile() && fi.isReadable() && fi.isWritable())
            loadDb(recentDb);
    }

}

void MainWindow::saveSettings(void)
{
    QSettings settings;
    settings.setValue("mainwindow/maximised",this->isMaximized());
    settings.setValue("mainwindow/position",this->pos());
    settings.setValue("mainwindow/size",this->size());

    if(!recentDb.isEmpty())
        settings.setValue("database/recentdb",recentDb);

}

void MainWindow::setMenusState()
{
    if(QSqlDatabase::database().databaseName().isEmpty())
    {
        ui->menuDataInput->setEnabled(false);
        ui->menuReporting->setEnabled(false);
        ui->actionEditAudit->setEnabled(false);
        ui->actionManageAudits->setEnabled(false);
        ui->actionManageTree->setEnabled(false);
        ui->actionQueries->setEnabled(false);
        ui->actionReport->setEnabled(false);
        ui->actionTablesGraphics->setEnabled(false);
    }
    else
    {
        ui->menuDataInput->setEnabled(true);
        ui->menuReporting->setEnabled(true);

        ui->actionEditAudit->setEnabled(true);
        ui->actionManageAudits->setEnabled(true);
        ui->actionManageTree->setEnabled(true);
        ui->actionQueries->setEnabled(true);
        ui->actionReport->setEnabled(true);
        ui->actionTablesGraphics->setEnabled(true);
    }
}


void MainWindow::on_actionManageTree_triggered()
{
    //Only one instance allowed
    if(formEditTreeWin==nullptr)
    {
        formEditTreeWin=new FormEditTree(this);
        formEditTreeWin->setAttribute(Qt::WA_DeleteOnClose);

        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formEditTreeWin);
        subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
        formEditTreeWin->show();
        if(formManageAudits!=nullptr)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageAudits,SLOT(onLOTchanged()));
        }
        if(formManageReportings!=nullptr)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageReportings,SLOT(onLOTchanged()));
        }
        connect(formEditTreeWin,SIGNAL(destroyed()),this,SLOT(onFormEditTreeWindowsDestroyed()));
    }
}


void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::onFormEditTreeWindowsDestroyed()
{
    formEditTreeWin=nullptr;
  //  qDebug()<<"FormEditTree window closed";
}

void MainWindow::onFormManageAuditsWindowsDestroyed()
{
    formManageAudits=nullptr;
    //qDebug()<<"FormManageAudits window closed";
}

void MainWindow::onFormManageReportingsWindowsDestroyed()
{
    formManageReportings=nullptr;
    //qDebug()<<"FormManageReporting window closed";
}

void MainWindow::onFormEditAuditWindowsDestroyed()
{
    formEditAudit=nullptr;
    //qDebug()<<"FormEditAudit window closed";
}

void MainWindow::onFormReportsWindowsDestroyed()
{
    formReports=nullptr;
    //qDebug()<<"FormReports window closed";
}

void MainWindow::onFormReportingReportsWindowsDestroyed()
{
    formReportingReports=nullptr;
    //qDebug()<<"FormReportingReports window closed";
}


void MainWindow::onFormTablesWindowsDestroyed(QObject *obj)
{
    listOfFormTablesGraphics.removeAt(listOfFormTablesGraphics.indexOf(static_cast<FormTablesGraphics *>(obj)));
    //qDebug()<<"FormTables window closed, remaining :"<<listOfFormTablesGraphics;
}

void MainWindow::onFormQueriesWindowsDestroyed(QObject *obj)
{
    listOfFormQueries.removeAt(listOfFormQueries.indexOf(static_cast<FormQueries *>(obj)));
    //qDebug()<<"FormQueries window closed, remaining :"<<listOfFormQueries;
}

void MainWindow::on_actionManageAudits_triggered()
{
    if(formManageAudits==nullptr)
    {
        formManageAudits=new FormManageAudits(this);
        formManageAudits->setAttribute(Qt::WA_DeleteOnClose);

        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formManageAudits);
        subWin->setWindowIcon(QIcon(":/icons/icons/manage_sources.png"));
        formManageAudits->show();
        connect(formManageAudits,SIGNAL(destroyed()),this,SLOT(onFormManageAuditsWindowsDestroyed()));

        if(formEditTreeWin!=nullptr)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageAudits,SLOT(onLOTchanged()));
        }

        if(formEditAudit!=nullptr)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formEditAudit,SLOT(onListOfAuditsChanged()));
        }

        if(formReports!=nullptr)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formReports,SLOT(onListOfAuditsChanged()));
        }

        foreach(FormTablesGraphics *dlg,listOfFormTablesGraphics)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
        }
        foreach(FormQueries *dlg,listOfFormQueries)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
        }

    }
}

void MainWindow::on_actionEditAudit_triggered()
{
    if(formEditAudit==nullptr)
    {
        formEditAudit=new FormEditAudit(this);
        formEditAudit->setAttribute(Qt::WA_DeleteOnClose);
        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formEditAudit);
        subWin->setWindowIcon(QIcon(":/icons/icons/table_edit.png"));

        formEditAudit->show();
        connect(formEditAudit,SIGNAL(destroyed()),this,SLOT(onFormEditAuditWindowsDestroyed()));

        if(formManageAudits!=nullptr)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formEditAudit,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionTablesGraphics_triggered()
{
    FormTablesGraphics *dlg=new FormTablesGraphics();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/exploreAudit.png"));

    dlg->show();
    listOfFormTablesGraphics.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormTablesWindowsDestroyed(QObject *)));

    if(formManageAudits!=nullptr)
    {
        connect(formManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
    }
}

void MainWindow::updateTitle()
{
    QFileInfo fi(QSqlDatabase::database().databaseName());
    setWindowTitle("ProtonClassicSuite - "+fi.baseName());
}

void MainWindow::on_actionNewDb_triggered()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if(!recentDb.isEmpty())
    {
        QFileInfo fi(recentDb);
        QDir dir=fi.dir();
        if(dir.isReadable())
        {
            fileDialog.setDirectory(dir);
        }
    }

    QString fileName = fileDialog.getSaveFileName(this, tr("Nouvelle base de données"), "",tr("Bases de données ProtonClassicSuite (*.pcxdb)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("pcxdb",Qt::CaseInsensitive))
        fileName.append(".pcxdb");
    fi=QFileInfo(fileName);

    QFile newFile(fileName);

    if(newFile.exists())
    {
        if(!newFile.remove())
        {
            QMessageBox::critical(this,tr("Attention"),newFile.errorString());
            return;
        }
    }
    else
    {
        if(!newFile.open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Le fichier n'est pas accessible en lecture/écriture"));
            return;
        }
        newFile.close();
    }

    ui->mdiArea->closeAllSubWindows();
    loadDb(fileName);
    initializeNewDb();
    recentDb=fileName;
    updateTitle();
    setMenusState();
}

void MainWindow::on_actionOpenDb_triggered()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if(!recentDb.isEmpty())
    {
        QFileInfo fi(recentDb);
        QDir dir=fi.dir();
        if(dir.isReadable())
        {
            fileDialog.setDirectory(dir);
        }
    }

    QString fileName = fileDialog.getOpenFileName(this, tr("Ouvrir une base de données"), "",tr("Bases de données ProtonClassicSuite (*.pcxdb)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(!fi.isWritable()||!fi.isFile()||!fi.isReadable())
    {
        QMessageBox::critical(this,tr("Attention"),tr("Le fichier n'est pas accessible en lecture/écriture"));
        return;
    }

    ui->mdiArea->closeAllSubWindows();
    loadDb(fileName);
    recentDb=fileName;
    updateTitle();
    setMenusState();
}

void MainWindow::on_actionReport_triggered()
{
    if(formReports==nullptr)
    {
        formReports=new FormReports(this);
        formReports->setAttribute(Qt::WA_DeleteOnClose);
        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formReports);
        subWin->setWindowIcon(QIcon(":/icons/icons/report.png"));
        formReports->show();
        connect(formReports,SIGNAL(destroyed()),this,SLOT(onFormReportsWindowsDestroyed()));

        if(formManageAudits!=nullptr)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formReports,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionO_ptions_triggered()
{
    DialogOptions dialogOptions(this);
    if(dialogOptions.exec()==QDialog::Accepted)
    {
        foreach(FormQueries *q, listOfFormQueries)
        {
            q->onColorChanged();
        }
    }
}

void MainWindow::on_actionQueries_triggered()
{
    FormQueries *dlg=new FormQueries(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/queries.png"));
    dlg->show();
    listOfFormQueries.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormQueriesWindowsDestroyed(QObject *)));

    if(formManageAudits!=nullptr)
    {
        connect(formManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
    }

}

void MainWindow::on_actionA_propos_triggered()
{
    DialogAbout dlg(this);
    dlg.exec();
}

void MainWindow::on_actionGestion_des_reportings_triggered()
{
    if(formManageReportings==nullptr)
    {
        formManageReportings=new FormManageReportings(this);
        formManageReportings->setAttribute(Qt::WA_DeleteOnClose);

        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formManageReportings);
        subWin->setWindowIcon(QIcon(":/icons/icons/reportingManage.png"));
        formManageReportings->show();
        connect(formManageReportings,SIGNAL(destroyed()),this,SLOT(onFormManageReportingsWindowsDestroyed()));

        if(formEditTreeWin!=nullptr)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageReportings,SLOT(onLOTchanged()));
        }

        if(formReportingReports!=nullptr)
        {
            connect(formReportingReports,SIGNAL(listOfReportingsChanged()),formReportingReports,SLOT(onListOfReportingsChanged()));
        }

        foreach(FormReportingOverview *dlg,listOfFormReportingOverview)
        {
            connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
        }

        foreach(FormReportingSupervision *dlg,listOfFormReportingSupervision)
        {
            connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
        }
        foreach(FormReportingGraphics *dlg,listOfFormReportingGraphics)
        {
            connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
        }
        foreach(FormReportingExplore *dlg,listOfFormReportingExplore)
        {
            connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
        }
        //FIXME : connect !
    }
}

void MainWindow::on_actionReportingOverview_triggered()
{
    FormReportingOverview *dlg=new FormReportingOverview();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingOverview.png"));
    dlg->show();
    listOfFormReportingOverview.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormReportingTablesWindowsDestroyed(QObject *)));

    if(formManageReportings!=nullptr)
    {
        connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
    }

}

void MainWindow::onFormReportingTablesWindowsDestroyed(QObject *obj)
{
    listOfFormReportingOverview.removeAt(listOfFormReportingOverview.indexOf(static_cast<FormReportingOverview *>(obj)));
}

void MainWindow::onFormReportingSupervisionWindowsDestroyed(QObject *obj)
{
    listOfFormReportingSupervision.removeAt(listOfFormReportingSupervision.indexOf(static_cast<FormReportingSupervision *>(obj)));
}

void MainWindow::onFormReportingGraphicsWindowsDestroyed(QObject *obj)
{
    listOfFormReportingGraphics.removeAt(listOfFormReportingGraphics.indexOf(static_cast<FormReportingGraphics *>(obj)));
}

void MainWindow::onFormReportingExploreWindowsDestroyed(QObject *obj)
{
    listOfFormReportingExplore.removeAt(listOfFormReportingExplore.indexOf(static_cast<FormReportingExplore *>(obj)));
}



void MainWindow::on_actionSurveillance_des_reportings_triggered()
{
    FormReportingSupervision *dlg=new FormReportingSupervision();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingForecast.png"));
    dlg->show();
    listOfFormReportingSupervision.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormReportingSupervisionWindowsDestroyed(QObject *)));

    if(formManageReportings!=nullptr)
    {
        connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
    }
}

void MainWindow::on_actionGraphiques_triggered()
{
    FormReportingGraphics *dlg=new FormReportingGraphics();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingGraphics.png"));
    dlg->show();
    listOfFormReportingGraphics.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormReportingGraphicsWindowsDestroyed(QObject *)));

    if(formManageReportings!=nullptr)
    {
        connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
    }
}

void MainWindow::on_actionExploreReportings_triggered()
{
    FormReportingExplore *dlg=new FormReportingExplore();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(dlg);
    subWin->setWindowIcon(QIcon(":/icons/icons/reportingExplore.png"));
    dlg->show();
    listOfFormReportingExplore.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onFormReportingExploreWindowsDestroyed(QObject *)));

    if(formManageReportings!=nullptr)
    {
        connect(formManageReportings,SIGNAL(listOfReportingsChanged()),dlg,SLOT(onListOfReportingsChanged()));
    }

}

void MainWindow::on_actionReportingGenerateur_de_rapports_triggered()
{
    if(formReportingReports==nullptr)
    {
        formReportingReports=new FormReportingReports(this);
        formReportingReports->setAttribute(Qt::WA_DeleteOnClose);
        QMdiSubWindow *subWin=ui->mdiArea->addSubWindow(formReportingReports);
        subWin->setWindowIcon(QIcon(":/icons/icons/reportingReport.png"));
        formReportingReports->show();
        connect(formReportingReports,SIGNAL(destroyed()),this,SLOT(onFormReportingReportsWindowsDestroyed()));

        if(formManageReportings!=nullptr)
        {
            connect(formManageReportings,SIGNAL(listOfReportingsChanged()),formReportingReports,SLOT(onListOfReportingsChanged()));
        }
    }

}
