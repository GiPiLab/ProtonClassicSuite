#include <QMessageBox>
#include <QInputDialog>
#include "formmanageaudits.h"
#include "ui_formmanageaudits.h"
#include "pcx_audit.h"
#include <QDebug>
#include "utils.h"
#include "dialogduplicateaudit.h"
#include "formdisplaytree.h"
#include "formauditinfos.h"
#include <QMdiSubWindow>
#include <QMdiArea>

FormManageAudits::FormManageAudits(QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormManageAudits)
{
    selectedAudit=nullptr;
    ui->setupUi(this);
    updateListOfTrees();
    updateListOfAudits();
    QDate date=QDate::currentDate();
    ui->spinBoxFrom->setMaximum(date.year());
    ui->spinBoxTo->setMaximum(date.year()+1);
}

FormManageAudits::~FormManageAudits()
{
    if(selectedAudit!=nullptr)
        delete selectedAudit;
    delete ui;
}


void FormManageAudits::updateListOfAudits()
{
    ui->comboListOfAudits->clear();

    QList<QPair<unsigned int,QString> > listOfAudits=PCx_Audit::getListOfAudits(PCx_Audit::AllAudits);
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

void FormManageAudits::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormManageAudits::updateListOfTrees()
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


void FormManageAudits::on_addAuditButton_clicked()
{
    if(ui->comboListOfTrees->count()==0)
    {
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

    redo:
    do
    {
        text=QInputDialog::getText(this,tr("Nouvel audit"), tr("Nom de l'audit à ajouter : "),QLineEdit::Normal,"",&ok).simplified();

    }while(ok && text.isEmpty());

    if(ok)
    {
        if(PCx_Audit::auditNameExists(text))
        {
            QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà un audit portant ce nom !"));
            goto redo;
        }
        //qDebug()<<"Adding an audit with name="<<text<<" years = "<<yearsString<<" treeId = "<<selectedTree;
        if(PCx_Audit::addNewAudit(text,years,selectedTree)>0)
        {
            updateListOfAudits();
            emit(listOfAuditsChanged());
            QMessageBox::information(this,tr("Information"),tr("Nouvel audit ajouté, ouvrez la fenêtre 'saisie des données' pour le compléter"));
        }
    }
}

void FormManageAudits::on_comboListOfAudits_activated(int index)
{
    if(index==-1 || ui->comboListOfAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListOfAudits->currentData().toUInt();

    if(selectedAudit!=nullptr)
    {
        delete selectedAudit;
        selectedAudit=nullptr;
    }

    selectedAudit=new PCx_Audit(selectedAuditId);

   // qDebug()<<"Selected audit = "<<selectedAuditId<< " "<<ui->comboListOfAudits->currentText();

    ui->labelDate->setText(selectedAudit->getCreationTimeLocal().toString(Qt::DefaultLocaleLongDate));
    if(selectedAudit->isFinished()==true)
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
    ui->labelTree->setText(QString("%1 (%2 noeuds)").arg(selectedAudit->getAttachedTreeName()).arg(selectedAudit->getAttachedTree()->getNumberOfNodes()));
    ui->labelYears->setText(selectedAudit->getYearsString());
    ui->labelName->setText(selectedAudit->getAuditName());
}

void FormManageAudits::on_deleteAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(question(tr("Voulez-vous vraiment <b>supprimer</b> l'audit <b>%1</b> ? Cette action ne peut être annulée").arg(ui->comboListOfAudits->currentText().toHtmlEscaped()))==QMessageBox::No)
    {
        return;
    }
    if(PCx_Audit::deleteAudit(ui->comboListOfAudits->currentData().toUInt())==false)
        return;

    if(selectedAudit!=nullptr)
    {
        delete selectedAudit;
        selectedAudit=nullptr;
    }
    updateListOfAudits();
    emit(listOfAuditsChanged());
}


void FormManageAudits::on_finishAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(question(tr("Voulez-vous vraiment <b>terminer</b> l'audit <b>%1</b> ? Cela signifie que vous ne pourrez plus en modifier les données").arg(ui->comboListOfAudits->currentText().toHtmlEscaped()))==QMessageBox::No)
    {
        return;
    }
    if(selectedAudit!=nullptr)
    {
        selectedAudit->finishAudit();
    }
    else
    {
        qWarning()<<"Invalid audit selected !";
    }
    updateListOfAudits();
    emit(listOfAuditsChanged());
}

void FormManageAudits::on_unFinishAuditButton_clicked()
{
    if(ui->comboListOfAudits->currentIndex()==-1)
    {
        return;
    }
    if(question(tr("Voulez-vous vraiment dé-terminer l'audit <b>%1</b> ? Celà signifie que vous pourrez en modifier les données, mais plus l'exploiter avant de l'avoir terminé à nouveau").arg(ui->comboListOfAudits->currentText().toHtmlEscaped()))==QMessageBox::No)
    {
        return;
    }
    if(selectedAudit!=nullptr)
    {
        selectedAudit->unFinishAudit();
    }
    else
    {
        qWarning()<<"Invalid audit selected !";
    }

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
    QMdiSubWindow *subWin=mdiArea->addSubWindow(ddt);
    subWin->setWindowIcon(QIcon(":/icons/icons/tree.png"));
    ddt->show();
}

void FormManageAudits::on_statisticsAuditButton_clicked()
{
    FormAuditInfos *infos=new FormAuditInfos(ui->comboListOfAudits->currentData().toUInt(),this);
    infos->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *mdiSubWin=(QMdiSubWindow *)this->parentWidget();
    QMdiArea *mdiArea=mdiSubWin->mdiArea();
    mdiArea->addSubWindow(infos);
    infos->show();
}
