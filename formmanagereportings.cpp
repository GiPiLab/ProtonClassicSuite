#include "pcx_reporting.h"
#include "formmanagereportings.h"
#include "ui_formmanagereportings.h"
#include "formdisplaytree.h"
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QInputDialog>

FormManageReportings::FormManageReportings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormManageReportings)
{
    ui->setupUi(this);
    updateListOfTrees();
    updateListOfReportings();
}

FormManageReportings::~FormManageReportings()
{
    delete ui;
}



void FormManageReportings::onLOTchanged()
{
    updateListOfTrees();
}

void FormManageReportings::updateListOfTrees()
{
    ui->comboListOfTrees->clear();

    QList<QPair<unsigned int,QString> > lot=PCx_Tree::getListOfTrees(true);
    setEnabled(!lot.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,lot)
    {
        ui->comboListOfTrees->insertItem(0,p.second,p.first);
    }
    ui->comboListOfTrees->setCurrentIndex(0);
}

void FormManageReportings::on_pushButtonDisplayTree_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
        return;
    }
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé le reporting"));
        return;
    }

    unsigned int selectedTree=ui->comboListOfTrees->currentData().toUInt();

    FormDisplayTree *ddt=new FormDisplayTree(selectedTree,this);
    ddt->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *mdiSubWin=(QMdiSubWindow *)this->parentWidget();
    QMdiArea *mdiArea=mdiSubWin->mdiArea();
    mdiArea->addSubWindow(ddt);
    ddt->show();
}

void FormManageReportings::on_pushButtonAddReporting_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
        return;
    }
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé le reporting"));
        return;
    }

    unsigned int selectedTree=ui->comboListOfTrees->currentData().toUInt();

    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouveau reporting"), tr("Nom du reporting à ajouter : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        if(PCx_Reporting::addNewReporting(text,selectedTree)>0)
        {
            updateListOfReportings();
            emit(listOfReportingsChanged());
            QMessageBox::information(this,tr("Information"),tr("Nouveau reporting ajouté, ouvrez la fenêtre 'saisie des données' pour le compléter"));
        }
    }
}

void FormManageReportings::updateListOfReportings()
{
    ui->comboListOfReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    ui->groupBoxReportings->setEnabled(!listOfReportings.isEmpty());
    QPair<unsigned int,QString> p;
    foreach(p,listOfReportings)
    {
        ui->comboListOfReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListOfReportings->setCurrentIndex(0);
    this->on_comboListOfReportings_activated(0);
}


void FormManageReportings::on_pushButtonDeleteReporting_clicked()
{
    if(ui->comboListOfReportings->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>supprimer</b> le reporting <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfReportings->currentText()))==QMessageBox::No)
    {
        return;
    }
    if(PCx_Reporting::deleteReporting(ui->comboListOfReportings->currentData().toUInt())==false)
        die();

    updateListOfReportings();
    emit(listOfReportingsChanged());

}



void FormManageReportings::on_comboListOfReportings_activated(int index)
{

}
