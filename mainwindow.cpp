#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbutils.h"
#include "dialogdisplaytree.h"
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
    ddtwidget=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAfficher_triggered()
{
    //Only one instance allowed
    if(ddt==NULL && ddtwidget==NULL)
    {
        ddtwidget=new DialogDisplayTree(this);
        this->ddt=ui->mdiArea->addSubWindow(ddtwidget);
        ddtwidget->show();
        connect(ddt,SIGNAL(destroyed()),this,SLOT(onDdtWindowsDestroyed()));
    }
}


void MainWindow::on_actionEffacerTout_triggered()
{
    if(QMessageBox::question(this,tr("Attention"),tr("Vous allez supprimer tous les arbres et tous les audits pour repartir sur une base de données neuve. Cette action ne peut pas être annulée. En êtes-vous sûr ?"))==QMessageBox::Yes)
    {
        ui->mdiArea->closeAllSubWindows();
        emptyDb();
    }
}

void MainWindow::on_actionQuitter_triggered()
{
    this->close();
}

void MainWindow::onDdtWindowsDestroyed()
{
    delete ddtwidget;
    ddtwidget=NULL;
    ddt=NULL;
    qDebug()<<"Closed";
}
