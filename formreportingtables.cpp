#include "formreportingtables.h"
#include "ui_formreportingtables.h"

FormReportingTables::FormReportingTables(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingTables)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    tableOverviewModel=nullptr;
    proxyModel=nullptr;
    updateListOfReportings();
    ui->splitter->setStretchFactor(1,1);
}

FormReportingTables::~FormReportingTables()
{
    delete ui;
    if(proxyModel!=nullptr)
        delete proxyModel;
    if(selectedReporting!=nullptr)
        delete selectedReporting;
    if(tableOverviewModel!=nullptr)
        delete tableOverviewModel;
}

void FormReportingTables::onListOfReportingsChanged()
{
    updateListOfReportings();
}

void FormReportingTables::on_comboListReportings_activated(int index)
{
    if(index==-1 || ui->comboListReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }

    if(tableOverviewModel!=nullptr)
    {
        delete tableOverviewModel;
        tableOverviewModel=nullptr;
    }

    if(proxyModel!=nullptr)
    {
        delete proxyModel;
        proxyModel=nullptr;
    }

    selectedReporting=new PCx_ReportingWithTreeModel(selectedReportingId);
    tableOverviewModel=new PCx_ReportingTableOverviewModel(selectedReporting,1,MODES::DF);
    proxyModel=new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(tableOverviewModel);

    ui->tableView->setModel(proxyModel);
    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
}

void FormReportingTables::updateListOfReportings()
{
    ui->comboListReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> p;
    foreach(p,listOfReportings)
    {
        ui->comboListReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListReportings->setCurrentIndex(0);
    this->on_comboListReportings_activated(0);
}

void FormReportingTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    tableOverviewModel->setNodeId(nodeId);
}

void FormReportingTables::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        tableOverviewModel->setMode(MODES::DF);
}

void FormReportingTables::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        tableOverviewModel->setMode(MODES::RF);
}

void FormReportingTables::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        tableOverviewModel->setMode(MODES::RI);
}

void FormReportingTables::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        tableOverviewModel->setMode(MODES::DI);
}


void FormReportingTables::on_checkBoxBP_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxReports_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REPORTS))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }

}

void FormReportingTables::on_checkBoxOCDM_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OCDM))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxVCDM_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VCDM))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxBudgetVote_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BUDGETVOTE))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxVCInternes_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::VIREMENTSINTERNES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxRattaches_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::RATTACHENMOINS1))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxOuverts_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxRealises_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxEngages_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}

void FormReportingTables::on_checkBoxDisponibles_toggled(bool checked)
{
    for(int i=0;i<proxyModel->columnCount();i++)
    {
        if(proxyModel->headerData(i,Qt::Horizontal)==PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::DISPONIBLES))
        {
            ui->tableView->setColumnHidden(i,!checked);
            break;
        }
    }
}
