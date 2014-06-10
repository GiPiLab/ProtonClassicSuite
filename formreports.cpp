#include "formreports.h"
#include "ui_formreports.h"

FormReports::FormReports(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReports)
{
    ui->setupUi(this);
    model=NULL;
    updateListOfAudits();
}

FormReports::~FormReports()
{
    delete ui;
}


void FormReports::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormReports::setEnabled(bool state)
{

}


void FormReports::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(FinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void FormReports::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
    }
    model=new PCx_AuditModel(selectedAuditId,this);

    ui->treeView->setModel(model->getAttachedTreeModel());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
}

void FormReports::on_pushButton_clicked()
{
    //ui->treeView->selectionModel()->
}
