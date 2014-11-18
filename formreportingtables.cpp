#include "formreportingtables.h"
#include "ui_formreportingtables.h"

FormReportingTables::FormReportingTables(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingTables)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    tableOverviewModel=nullptr;
    //Root ID
    selectedNodeId=1;
    proxyModel=nullptr;
    updateListOfReportings();
    PCx_Reporting::OREDPCRToComboBox(ui->comboBoxRefColumn);
    ui->comboBoxRefColumn->setCurrentIndex(ui->comboBoxRefColumn->count());
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
    ui->tableView->resizeColumnsToContents();
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNodeId=1;
    on_treeView_clicked(rootIndex);

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

MODES::DFRFDIRI FormReportingTables::getSelectedMode() const
{
    if(ui->radioButtonDF->isChecked())
        return MODES::DF;
    if(ui->radioButtonRF->isChecked())
        return MODES::RF;
    if(ui->radioButtonDI->isChecked())
        return MODES::DI;
    if(ui->radioButtonRI->isChecked())
        return MODES::RI;
    else
    {
        qDebug()<<"Invalid selection";
    }
    return MODES::GLOBAL;
}

void FormReportingTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    selectedNodeId=nodeId;
    ui->groupBoxMode->setTitle(index.data().toString());
    tableOverviewModel->setNodeId(nodeId);
    selectedReporting->fillDateComboBox(ui->comboBoxRefDate,getSelectedMode(),nodeId);

    int dateRef=tableOverviewModel->getDateRef();
    if(dateRef!=-1)
    {
        if(!selectedReporting->dateExistsForNodeAndMode(dateRef,getSelectedMode(),selectedNodeId))
        {
            ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
            tableOverviewModel->setDateRef(-1);
        }
        else
        {
            for(int i=0;i<ui->comboBoxRefDate->count();i++)
            {
                if(ui->comboBoxRefDate->itemData(i)==dateRef)
                {
                    ui->comboBoxRefDate->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
    else
    {
        ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
    }

}

void FormReportingTables::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        tableOverviewModel->setMode(MODES::DF);
        selectedReporting->fillDateComboBox(ui->comboBoxRefDate,MODES::DFRFDIRI::DF,selectedNodeId);

        int dateRef=tableOverviewModel->getDateRef();
        if(dateRef!=-1)
        {
            if(!selectedReporting->dateExistsForNodeAndMode(dateRef,MODES::DF,selectedNodeId))
            {
                ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
                tableOverviewModel->setDateRef(-1);
            }
            else
            {
                for(int i=0;i<ui->comboBoxRefDate->count();i++)
                {
                    if(ui->comboBoxRefDate->itemData(i)==dateRef)
                    {
                        ui->comboBoxRefDate->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        else
        {
            ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
        }
    }
}

void FormReportingTables::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        tableOverviewModel->setMode(MODES::RF);
        selectedReporting->fillDateComboBox(ui->comboBoxRefDate,MODES::DFRFDIRI::RF,selectedNodeId);

        int dateRef=tableOverviewModel->getDateRef();
        if(dateRef!=-1)
        {
            if(!selectedReporting->dateExistsForNodeAndMode(dateRef,MODES::RF,selectedNodeId))
            {
                ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
                tableOverviewModel->setDateRef(-1);
            }
            else
            {
                for(int i=0;i<ui->comboBoxRefDate->count();i++)
                {
                    if(ui->comboBoxRefDate->itemData(i)==dateRef)
                    {
                        ui->comboBoxRefDate->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        else
        {
            ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
        }
    }
}

void FormReportingTables::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        tableOverviewModel->setMode(MODES::RI);
        selectedReporting->fillDateComboBox(ui->comboBoxRefDate,MODES::DFRFDIRI::RI,selectedNodeId);

        int dateRef=tableOverviewModel->getDateRef();
        if(dateRef!=-1)
        {
            if(!selectedReporting->dateExistsForNodeAndMode(dateRef,MODES::RI,selectedNodeId))
            {
                ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
                tableOverviewModel->setDateRef(-1);
            }
            else
            {
                for(int i=0;i<ui->comboBoxRefDate->count();i++)
                {
                    if(ui->comboBoxRefDate->itemData(i)==dateRef)
                    {
                        ui->comboBoxRefDate->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        else
        {
            ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
        }
    }
}

void FormReportingTables::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        tableOverviewModel->setMode(MODES::DI);
        selectedReporting->fillDateComboBox(ui->comboBoxRefDate,MODES::DFRFDIRI::DI,selectedNodeId);

        int dateRef=tableOverviewModel->getDateRef();
        if(dateRef!=-1)
        {
            if(!selectedReporting->dateExistsForNodeAndMode(dateRef,MODES::DI,selectedNodeId))
            {
                ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
                tableOverviewModel->setDateRef(-1);
            }
            else
            {
                for(int i=0;i<ui->comboBoxRefDate->count();i++)
                {
                    if(ui->comboBoxRefDate->itemData(i)==dateRef)
                    {
                        ui->comboBoxRefDate->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        else
        {
            ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
        }
    }
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

void FormReportingTables::on_pushButtonExportHtml_clicked()
{
    qDebug()<<qTableViewToHtml(ui->tableView);
}

void FormReportingTables::on_comboBoxRefColumn_activated(int index)
{
    Q_UNUSED(index);
    if(ui->comboBoxRefColumn->count()==0)
        return;
    PCx_Reporting::OREDPCR selectedOred=(PCx_Reporting::OREDPCR)ui->comboBoxRefColumn->currentData().toInt();
    tableOverviewModel->setColRef(selectedOred);
    if(selectedOred!=PCx_Reporting::OREDPCR::NONE)
    {
        ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
    }

}

void FormReportingTables::on_comboBoxRefDate_activated(int index)
{
    Q_UNUSED(index);
    if(ui->comboBoxRefDate->count()==0)
        return;
    int selectedDate=ui->comboBoxRefDate->currentData().toInt();
    tableOverviewModel->setDateRef(selectedDate);
    if(selectedDate!=-1)
    {
        ui->comboBoxRefColumn->setCurrentIndex(ui->comboBoxRefColumn->count());
    }
}
