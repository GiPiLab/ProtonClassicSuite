#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "formedittree.h"
#include <QFileDialog>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QMdiSubWindow>


//TODO : MANAGE EMPTY DATABASE GRAYED MENUS

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
    formEditTreeWin=NULL;
    formManageAudits=NULL;
    formEditAudit=NULL;

    restoreSettings();
    updateTitle();
    setMenusState();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::restoreSettings(void)
{
    QSettings settings("GiPiLab","ProtonClassicSuite");
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
    QSettings settings("GiPiLab","ProtonClassicSuite");
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
        ui->menuExploitation->setEnabled(false);
        ui->menuDataInput->setEnabled(false);
    }
    else
    {
        ui->menuExploitation->setEnabled(true);
        ui->menuDataInput->setEnabled(true);
    }
}



void MainWindow::on_actionManageTree_triggered()
{
    //Only one instance allowed
    if(formEditTreeWin==NULL)
    {
        formEditTreeWin=new FormEditTree(this,ui->mdiArea);
        formEditTreeWin->setAttribute(Qt::WA_DeleteOnClose);

        ui->mdiArea->addSubWindow(formEditTreeWin);
        formEditTreeWin->show();
        if(formManageAudits!=NULL)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageAudits,SLOT(onLOTchanged()));
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
    formEditTreeWin=NULL;
    //qDebug()<<"Closed";
}

void MainWindow::onFormManageAuditsWindowsDestroyed()
{
    formManageAudits=NULL;
}

void MainWindow::onFormEditAuditWindowsDestroyed()
{
    formEditAudit=NULL;
}

void MainWindow::onDialogTablesWindowsDestroyed(QObject *obj)
{
    //qDebug()<<listOfDialogTables;
    listOfFormTablesGraphics.removeAt(listOfFormTablesGraphics.indexOf((FormTablesGraphics *)obj));
    //qDebug()<<listOfDialogTables;
}

void MainWindow::on_actionManageAudits_triggered()
{
    if(formManageAudits==NULL)
    {
        formManageAudits=new FormManageAudits(this,ui->mdiArea);
        formManageAudits->setAttribute(Qt::WA_DeleteOnClose);

        ui->mdiArea->addSubWindow(formManageAudits);
        formManageAudits->show();
        connect(formManageAudits,SIGNAL(destroyed()),this,SLOT(onFormManageAuditsWindowsDestroyed()));

        if(formEditTreeWin!=NULL)
        {
            connect(formEditTreeWin,SIGNAL(listOfTreeChanged()),formManageAudits,SLOT(onLOTchanged()));
        }

        if(formEditAudit!=NULL)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formEditAudit,SLOT(onListOfAuditsChanged()));
        }
        foreach(FormTablesGraphics *dlg,listOfFormTablesGraphics)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionEditAudit_triggered()
{
    if(formEditAudit==NULL)
    {
        formEditAudit=new FormEditAudit(this);
        formEditAudit->setAttribute(Qt::WA_DeleteOnClose);
        ui->mdiArea->addSubWindow(formEditAudit);
        formEditAudit->show();
        connect(formEditAudit,SIGNAL(destroyed()),this,SLOT(onFormEditAuditWindowsDestroyed()));

        if(formManageAudits!=NULL)
        {
            connect(formManageAudits,SIGNAL(listOfAuditsChanged()),formEditAudit,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionTablesGraphics_triggered()
{
    FormTablesGraphics *dlg=new FormTablesGraphics();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    ui->mdiArea->addSubWindow(dlg);
    dlg->show();
    listOfFormTablesGraphics.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onDialogTablesWindowsDestroyed(QObject *)));

    if(formManageAudits!=NULL)
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
    initCurrentDb();
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
