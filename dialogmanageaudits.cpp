#include <QMessageBox>
#include <QInputDialog>
#include "dialogmanageaudits.h"
#include "ui_dialogmanageaudits.h"
#include "pcx_treemodel.h"
#include "pcx_auditmodel.h"
#include "pcx_auditinfos.h"

DialogManageAudits::DialogManageAudits(QWidget *parent,QMdiArea *mdiarea) :
    QWidget(parent),
    ui(new Ui::DialogManageAudits)
{
    ui->setupUi(this);
    updateListOfTrees();
    updateListOfAudits();
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
    this->on_comboListOfAudits_activated(0);
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
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé l'audit"));
        return;
    }

    unsigned int selectedTree=ui->comboListOfTrees->currentData().toUInt();

    int anneeFrom=ui->spinBoxFrom->value();
    int anneeTo=ui->spinBoxTo->value();

    if(anneeFrom>=anneeTo)
    {
        QMessageBox::warning(this,tr("Attention"),tr("L'audit doit porter sur au moins deux années"));
        return;
    }

    QSet<unsigned int> years;
    QString yearsString;
    yearsString=QString("De %1 à %2").arg(anneeFrom).arg(anneeTo);
    for(int i=anneeFrom;i<=anneeTo;i++)
    {
        years.insert(i);
    }

    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouvel audit"), tr("Nom de l'audit à ajouter : "),QLineEdit::Normal,"",&ok);

    }while(ok && text.isEmpty());

    if(ok)
    {
        qDebug()<<"Add audit with name="<<text<<" years = "<<yearsString<<" treeId = "<<selectedTree;
        PCx_AuditModel::addNewAudit(text,years,selectedTree);
        updateListOfAudits();
    }
}

void DialogManageAudits::on_comboListOfAudits_activated(int index)
{
    if(index==-1 || ui->comboListOfAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListOfAudits->currentData().toUInt();
    qDebug()<<"Selected audit = "<<selectedAuditId<< " "<<ui->comboListOfAudits->currentText();
    PCx_AuditInfos infos(selectedAuditId);
    if(infos.valid==true)
    {
        ui->labelDate->setText(infos.creationTimeLocal.toString(Qt::SystemLocaleLongDate));
        if(infos.finished==true)
        {
            ui->labelFinished->setText(tr("oui"));
        }
        else
        {
            ui->labelFinished->setText(tr("non"));
        }
        ui->labelTree->setText(infos.attachedTreeName);
        ui->labelYears->setText(infos.yearsString);
        ui->labelName->setText(infos.name);
    }
}

void DialogManageAudits::on_deleteAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment supprimer l'audit <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfAudits->currentText()))==QMessageBox::No)
    {
        return;
    }
    PCx_AuditModel::deleteAudit(ui->comboListOfAudits->currentData().toUInt());
    updateListOfAudits();
}





