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

    QList<QPair<unsigned int,QString> > listOfAudits=PCx_AuditModel::getListOfAudits(AllAudits);
    QPair<unsigned int,QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListOfAudits->insertItem(0,p.second,p.first);
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

    QList<QPair<unsigned int,QString> > lot=PCx_TreeModel::getListOfTrees(true);
    QPair<unsigned int, QString> p;
    foreach(p,lot)
    {
        ui->comboListOfTrees->insertItem(0,p.second,p.first);
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

    QList<unsigned int> years;
    QString yearsString;
    yearsString=QString("De %1 à %2").arg(anneeFrom).arg(anneeTo);
    for(int i=anneeFrom;i<=anneeTo;i++)
    {
        years.append(i);
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
        emit(listOfAuditsChanged());
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
            ui->finishAuditButton->setEnabled(false);
        }
        else
        {
            ui->labelFinished->setText(tr("non"));
            ui->finishAuditButton->setEnabled(true);
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
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>supprimer</b> l'audit <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfAudits->currentText()))==QMessageBox::No)
    {
        return;
    }
    PCx_AuditModel::deleteAudit(ui->comboListOfAudits->currentData().toUInt());
    updateListOfAudits();
    emit(listOfAuditsChanged());

}


void DialogManageAudits::on_finishAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>terminer</b> l'audit <b>%1</b> ? Celà signifie que vous ne pourrez plus en modifier les données").arg(ui->comboListOfAudits->currentText()))==QMessageBox::No)
    {
        return;
    }
    PCx_AuditModel::finishAudit(ui->comboListOfAudits->currentData().toUInt());
    updateListOfAudits();
    emit(listOfAuditsChanged());
}
