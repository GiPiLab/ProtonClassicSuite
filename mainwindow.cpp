#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAfficher_triggered()
{
    this->ddt=new DialogDisplayTree(this);
    this->ui->mdiArea->addSubWindow(ddt);
    ddt->show();
}
