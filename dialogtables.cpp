#include "dialogtables.h"
#include "ui_dialogtables.h"

DialogTables::DialogTables(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogTables)
{
    ui->setupUi(this);
    tables=NULL;
    ui->splitter->setStretchFactor(1,1);

    updateListOfAudits();

}

DialogTables::~DialogTables()
{
    delete ui;
    if(tables!=NULL)
        delete tables;
}

void DialogTables::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(FinishedAuditsOnly);
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void DialogTables::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(tables!=NULL)
    {
        delete tables;
    }
    tables=new PCx_tables(selectedAuditId,this);

    ui->treeView->setModel(tables->getModel()->getAttachedTreeModel());
    ui->treeView->expandToDepth(1);
}

void DialogTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(selectedNode>0);

    ui->label->setText(index.data().toString());


    QSqlTableModel *modelDF=tables->getModel()->getModelDF();
    QSqlTableModel *modelRF=tables->getModel()->getModelRF();
    QSqlTableModel *modelDI=tables->getModel()->getModelDI();
    QSqlTableModel *modelRI=tables->getModel()->getModelRI();


    QString T1DF=tables->getT1(selectedNode,modelDF);
    QString T1RF=tables->getT1(selectedNode,modelRF);
    QString T1DI=tables->getT1(selectedNode,modelDI);
    QString T1RI=tables->getT1(selectedNode,modelRI);
    ui->textBrowserRecapDF->setText(T1DF);
    ui->textBrowserRecapRF->setText(T1RF);
    ui->textBrowserRecapDI->setText(T1DI);
    ui->textBrowserRecapRI->setText(T1RI);



}
