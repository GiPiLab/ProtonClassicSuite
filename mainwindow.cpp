#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "dialogedittree.h"
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
    dialogEditTreeWin=NULL;
    dialogManageAudits=NULL;
    dialogEditAudit=NULL;

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
    if(dialogEditTreeWin==NULL)
    {
        dialogEditTreeWin=new DialogEditTree(this,ui->mdiArea);
        dialogEditTreeWin->setAttribute(Qt::WA_DeleteOnClose);

        ui->mdiArea->addSubWindow(dialogEditTreeWin);
        dialogEditTreeWin->show();
        if(dialogManageAudits!=NULL)
        {
            connect(dialogEditTreeWin,SIGNAL(listOfTreeChanged()),dialogManageAudits,SLOT(onLOTchanged()));
        }
        connect(dialogEditTreeWin,SIGNAL(destroyed()),this,SLOT(onDialogEditTreeWindowsDestroyed()));     
    }
}


void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::onDialogEditTreeWindowsDestroyed()
{
    dialogEditTreeWin=NULL;
    //qDebug()<<"Closed";
}

void MainWindow::onDialogManageAuditsWindowsDestroyed()
{
    dialogManageAudits=NULL;
}

void MainWindow::onDialogEditAuditWindowsDestroyed()
{
    dialogEditAudit=NULL;
}

void MainWindow::onDialogTablesWindowsDestroyed(QObject *obj)
{
    //qDebug()<<listOfDialogTables;
    listOfDialogTablesGraphics.removeAt(listOfDialogTablesGraphics.indexOf((DialogTablesGraphics *)obj));
    //qDebug()<<listOfDialogTables;
}

void MainWindow::on_actionManageAudits_triggered()
{
    if(dialogManageAudits==NULL)
    {
        dialogManageAudits=new DialogManageAudits(this,ui->mdiArea);
        dialogManageAudits->setAttribute(Qt::WA_DeleteOnClose);

        ui->mdiArea->addSubWindow(dialogManageAudits);
        dialogManageAudits->show();
        connect(dialogManageAudits,SIGNAL(destroyed()),this,SLOT(onDialogManageAuditsWindowsDestroyed()));

        if(dialogEditTreeWin!=NULL)
        {
            connect(dialogEditTreeWin,SIGNAL(listOfTreeChanged()),dialogManageAudits,SLOT(onLOTchanged()));
        }

        if(dialogEditAudit!=NULL)
        {
            connect(dialogManageAudits,SIGNAL(listOfAuditsChanged()),dialogEditAudit,SLOT(onListOfAuditsChanged()));
        }
        foreach(DialogTablesGraphics *dlg,listOfDialogTablesGraphics)
        {
            connect(dialogManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionEditAudit_triggered()
{
    if(dialogEditAudit==NULL)
    {
        dialogEditAudit=new DialogEditAudit(this);
        dialogEditAudit->setAttribute(Qt::WA_DeleteOnClose);
        ui->mdiArea->addSubWindow(dialogEditAudit);
        dialogEditAudit->show();
        connect(dialogEditAudit,SIGNAL(destroyed()),this,SLOT(onDialogEditAuditWindowsDestroyed()));

        if(dialogManageAudits!=NULL)
        {
            connect(dialogManageAudits,SIGNAL(listOfAuditsChanged()),dialogEditAudit,SLOT(onListOfAuditsChanged()));
        }
    }
}

void MainWindow::on_actionTablesGraphics_triggered()
{
    DialogTablesGraphics *dlg=new DialogTablesGraphics();
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    ui->mdiArea->addSubWindow(dlg);
    dlg->show();
    listOfDialogTablesGraphics.append(dlg);

    connect(dlg,SIGNAL(destroyed(QObject *)),this,SLOT(onDialogTablesWindowsDestroyed(QObject *)));

    if(dialogManageAudits!=NULL)
    {
        connect(dialogManageAudits,SIGNAL(listOfAuditsChanged()),dlg,SLOT(onListOfAuditsChanged()));
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
