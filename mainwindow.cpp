#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbutils.h"
#include "dialogedittree.h"
#include <QFileDialog>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
    dialogEditTreeWin=NULL;
    dialogManageAudits=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionManageTree_triggered()
{
    //Only one instance allowed
    if(dialogEditTreeWin==NULL)
    {
        dialogEditTreeWin=new DialogEditTree(this,ui->mdiArea);
        ui->mdiArea->addSubWindow(dialogEditTreeWin);
        dialogEditTreeWin->show();
        if(dialogManageAudits!=NULL)
        {
            connect(dialogEditTreeWin,SIGNAL(listOfTreeChanged()),dialogManageAudits,SLOT(onLOTchanged()));
        }
        connect(dialogEditTreeWin,SIGNAL(destroyed()),this,SLOT(onDialogEditTreeWindowsDestroyed()));
    }
}


void MainWindow::on_actionReset_triggered()
{
    if(QMessageBox::question(this,tr("Attention"),tr("Vous allez supprimer tous les arbres et tous les audits pour repartir sur une base de données neuve. Cette action ne peut pas être annulée. En êtes-vous sûr ?"))==QMessageBox::Yes)
    {
        dialogEditTreeWin->close();
        ui->mdiArea->closeAllSubWindows();
        emptyDb();
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::onDialogEditTreeWindowsDestroyed()
{
    dialogEditTreeWin=NULL;
    qDebug()<<"Closed";
}

void MainWindow::onDialogManageAuditsWindowsDestroyed()
{
    dialogManageAudits=NULL;
}

void MainWindow::on_actionGerer_les_audits_triggered()
{
    if(dialogManageAudits==NULL)
    {
        dialogManageAudits=new DialogManageAudits(this,ui->mdiArea);
        ui->mdiArea->addSubWindow(dialogManageAudits);
        dialogManageAudits->show();
        connect(dialogManageAudits,SIGNAL(destroyed()),this,SLOT(onDialogManageAuditsWindowsDestroyed()));
    }

}
