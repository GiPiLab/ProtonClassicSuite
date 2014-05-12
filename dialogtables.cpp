#include "dialogtables.h"
#include "pcx_tables.h"
#include "ui_dialogtables.h"
#include "utils.h"

//TODO : Slot for list of audits changed


DialogTables::DialogTables(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogTables)
{
    model=NULL;
    ui->setupUi(this);
    //ui->splitter->setStretchFactor(1,1);
    updateListOfAudits();

}

DialogTables::~DialogTables()
{
    delete ui;
    if(model!=NULL)
        delete model;
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

void DialogTables::updateTextBrowser()
{
    QSqlTableModel *tableModel;
    if(ui->radioButtonDF->isChecked())
    {
        tableModel=model->getTableModelDF();
    }
    else if(ui->radioButtonRF->isChecked())
    {
        tableModel=model->getTableModelRF();
    }
    else if(ui->radioButtonDI->isChecked())
    {
        tableModel=model->getTableModelDI();
    }
    else if(ui->radioButtonRI->isChecked())
    {
        tableModel=model->getTableModelRI();
    }
    else if(ui->radioButtonGlobal->isChecked())
    {
        tableModel=NULL;
    }
    else
    {
        qCritical()<<"Unsupported case of radio button checking";
        die();
    }

    ui->textBrowser->clear();
    QString output;

    unsigned int selectedNode=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();


    //Mode DF,RF,DI,RI
    if(tableModel!=NULL)
    {
        if(ui->checkBoxRecap->isChecked())
        {
                output.append(PCx_Tables::getTabRecap(selectedNode,tableModel));
        }
        if(ui->checkBoxEvolution->isChecked())
        {
                output.append(PCx_Tables::getTabEvolution(selectedNode,tableModel));
        }
        if(ui->checkBoxEvolutionCumul->isChecked())
        {
                output.append(PCx_Tables::getTabEvolutionCumul(selectedNode,tableModel));
        }
        if(ui->checkBoxBase100->isChecked())
        {
                output.append(PCx_Tables::getTabBase100(selectedNode,tableModel));
        }
        if(ui->checkBoxJoursAct->isChecked())
        {
                output.append(PCx_Tables::getTabJoursAct(selectedNode,tableModel));
        }
    }
    //Global mode
    else
    {
        output.append("Toto");
    }

    ui->textBrowser->setHtml(output);
}


void DialogTables::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
    }
    model=new PCx_AuditModel(selectedAuditId,this);

    ui->treeView->setModel(model->getAttachedTreeModel());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
}

void DialogTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(selectedNode>0);

    ui->groupBoxMode->setTitle(index.data().toString());

    updateTextBrowser();

    /*displayTablesRecap(selectedNode,tbl);
        displayTablesEvolution(selectedNode,tbl);
        displayTablesEvolutionCumul(selectedNode,tbl);
        displayTablesBase100(selectedNode,tbl);
        displayTablesJoursAct(selectedNode,tbl);*/
}

void DialogTables::on_radioButtonGlobal_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_checkBoxRecap_toggled(bool checked)
{
        updateTextBrowser();
}

void DialogTables::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_radioButtonRF_toggled(bool checked)
{
   if(checked)
       updateTextBrowser();
}

void DialogTables::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_checkBoxEvolution_toggled(bool checked)
{
    updateTextBrowser();
}

void DialogTables::on_checkBoxEvolutionCumul_toggled(bool checked)
{
    updateTextBrowser();
}

void DialogTables::on_checkBoxBase100_toggled(bool checked)
{
    updateTextBrowser();
}

void DialogTables::on_checkBoxJoursAct_toggled(bool checked)
{
    updateTextBrowser();
}
