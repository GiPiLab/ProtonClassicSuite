#include <QMessageBox>
#include <QInputDialog>
#include "formmanageaudits.h"
#include "ui_formmanageaudits.h"
#include "pcx_auditmodel.h"
#include "pcx_auditinfos.h"
#include <QDebug>
#include "utils.h"
#include "dialogduplicateaudit.h"
#include "formdisplaytree.h"
#include <QMdiSubWindow>
#include <QMdiArea>

FormManageAudits::FormManageAudits(QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormManageAudits)
{
    ui->setupUi(this);
    updateListOfTrees();
    updateListOfAudits();
    QDate date=QDate::currentDate();
    ui->spinBoxFrom->setMaximum(date.year());
    ui->spinBoxTo->setMaximum(date.year()+1);
}

FormManageAudits::~FormManageAudits()
{
    delete ui;
}


void FormManageAudits::updateListOfAudits()
{
    ui->comboListOfAudits->clear();

    QList<QPair<unsigned int,QString> > listOfAudits=PCx_AuditModel::getListOfAudits(PCx_AuditModel::AllAudits);
    ui->groupBoxAudits->setEnabled(!listOfAudits.isEmpty());
    QPair<unsigned int,QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListOfAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListOfAudits->setCurrentIndex(0);
    this->on_comboListOfAudits_activated(0);
}

void FormManageAudits::onLOTchanged()
{
    updateListOfTrees();
}

void FormManageAudits::updateListOfTrees()
{
    ui->comboListOfTrees->clear();

    QList<QPair<unsigned int,QString> > lot=PCx_TreeModel::getListOfTrees(true);
    setEnabled(!lot.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,lot)
    {
        ui->comboListOfTrees->insertItem(0,p.second,p.first);
    }
    ui->comboListOfTrees->setCurrentIndex(0);
}


void FormManageAudits::on_addAuditButton_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
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
        qDebug()<<"Adding an audit with name="<<text<<" years = "<<yearsString<<" treeId = "<<selectedTree;
        PCx_AuditModel::addNewAudit(text,years,selectedTree);
        updateListOfAudits();
        emit(listOfAuditsChanged());
    }
}

void FormManageAudits::on_comboListOfAudits_activated(int index)
{
    if(index==-1 || ui->comboListOfAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListOfAudits->currentData().toUInt();
    qDebug()<<"Selected audit = "<<selectedAuditId<< " "<<ui->comboListOfAudits->currentText();
    PCx_AuditInfos infos(selectedAuditId);
    ui->labelDate->setText(infos.creationTimeLocal.toString(Qt::SystemLocaleLongDate));
    if(infos.finished==true)
    {
        ui->labelFinished->setText(tr("oui"));
        ui->finishAuditButton->setEnabled(false);
        ui->unFinishAuditButton->setEnabled(true);
    }
    else
    {
        ui->labelFinished->setText(tr("non"));
        ui->finishAuditButton->setEnabled(true);
        ui->unFinishAuditButton->setEnabled(false);
    }
    ui->labelTree->setText(QString("%1 (%2 noeuds)").arg(infos.attachedTreeName).arg(PCx_TreeModel::getNumberOfNodes(infos.attachedTreeId)));
    ui->labelYears->setText(infos.yearsString);
    ui->labelName->setText(infos.name);
}

void FormManageAudits::on_deleteAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment <b>supprimer</b> l'audit <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfAudits->currentText()))==QMessageBox::No)
    {
        return;
    }
    if(PCx_AuditModel::deleteAudit(ui->comboListOfAudits->currentData().toUInt())==false)
        die();
    updateListOfAudits();
    emit(listOfAuditsChanged());
}


void FormManageAudits::on_finishAuditButton_clicked()
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

void FormManageAudits::on_unFinishAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(QMessageBox::question(this,tr("Attention"),tr("Voulez-vous vraiment dé-terminer l'audit <b>%1</b> ? Celà signifie que vous pourrez en modifier les données, mais plus l'exploiter avant de l'avoir terminé à nouveau").arg(ui->comboListOfAudits->currentText()))==QMessageBox::No)
    {
        return;
    }
    PCx_AuditModel::unFinishAudit(ui->comboListOfAudits->currentData().toUInt());
    updateListOfAudits();
    emit(listOfAuditsChanged());

}

void FormManageAudits::on_cloneAuditButton_clicked()
{
    DialogDuplicateAudit d(ui->comboListOfAudits->currentData().toUInt(),this);
    int res=d.exec();
    if(res==QDialog::Accepted)
    {
        updateListOfAudits();
        emit(listOfAuditsChanged());
    }
}

void FormManageAudits::on_pushButtonDisplayTree_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
        qDebug()<<"No finished tree";
        return;
    }
    int selectedIndex=ui->comboListOfTrees->currentIndex();

    if(selectedIndex==-1)
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez l'arbre sur lequel sera adossé l'audit"));
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
