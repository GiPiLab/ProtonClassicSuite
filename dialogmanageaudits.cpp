#include "dialogmanageaudits.h"
#include "ui_dialogmanageaudits.h"
#include "pcx_treemodel.h"
#include "pcx_auditmodel.h"

DialogManageAudits::DialogManageAudits(QWidget *parent,QMdiArea *mdiarea) :
    QDialog(parent),
    ui(new Ui::DialogManageAudits)
{
    ui->setupUi(this);
    updateListOfTrees();
    this->mdiarea=mdiarea;

}

DialogManageAudits::~DialogManageAudits()
{

    delete ui;
}


void DialogManageAudits::updateListOfAudits()
{
    ui->comboListOfAudits->clear();

    QHash<int,QString> listOfAudits=PCx_AuditModel::getListOfAudits(false);
    foreach(int auditId,listOfAudits.keys())
    {
        ui->comboListOfAudits->insertItem(0,listOfAudits[auditId],auditId);
    }
    ui->comboListOfAudits->setCurrentIndex(0);
}



void DialogManageAudits::onLOTchanged()
{
    updateListOfTrees();
}

void DialogManageAudits::updateListOfTrees()
{
    ui->comboListOfTrees->clear();

    QHash<int,QString> lot=PCx_TreeModel::getListOfTrees(true);
    foreach(int treeId,lot.keys())
    {
        ui->comboListOfTrees->insertItem(0,lot[treeId],treeId);
    }
    ui->comboListOfTrees->setCurrentIndex(0);
}

void DialogManageAudits::on_addAuditButton_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"Pas d'arbre";
        return;
    }



    QSet<unsigned int> annees;
    annees.insert(2002);
    annees.insert(2003);
    annees.insert(2004);
    PCx_AuditModel::addNewAudit("Toto",annees,1);
}
