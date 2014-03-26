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
    ddt=NULL;
    dialogEditTreeWin=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionManageTree_triggered()
{
    //Only one instance allowed
    if(ddt==NULL && dialogEditTreeWin==NULL)
    {
        dialogEditTreeWin=new DialogEditTree(this);
        this->ddt=ui->mdiArea->addSubWindow(dialogEditTreeWin);
        dialogEditTreeWin->show();
        connect(ddt,SIGNAL(destroyed()),this,SLOT(onDialogEditTreeWindowsDestroyed()));
    }
}


void MainWindow::on_actionReset_triggered()
{
    if(QMessageBox::question(this,tr("Attention"),tr("Vous allez supprimer tous les arbres et tous les audits pour repartir sur une base de données neuve. Cette action ne peut pas être annulée. En êtes-vous sûr ?"))==QMessageBox::Yes)
    {
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
    delete dialogEditTreeWin;
    dialogEditTreeWin=NULL;
    ddt=NULL;
    qDebug()<<"Closed";
}
