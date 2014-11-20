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

    ui->comboBoxRefColumn->setCurrentIndex(ui->comboBoxRefColumn->count());
    ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());

    selectedReporting=new PCx_ReportingWithTreeModel(selectedReportingId);
    tableOverviewModel=new PCx_ReportingTableOverviewModel(selectedReporting,1,getSelectedMode());
    proxyModel=new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(tableOverviewModel);

    ui->tableView->setModel(proxyModel);
    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNodeId=1;
    on_treeView_clicked(rootIndex);
    ui->tableView->resizeColumnsToContents();
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

void FormReportingTables::updateComboRefDate(QComboBox *combo)
{
    PCx_ReportingTableOverviewModel::OVERVIEWMODE selectedMode=getSelectedMode();

    combo->clear();
    QSet<QDate> datesForNode;

    if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF)
    {
       datesForNode=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DF);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF)
    {
       datesForNode=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RF);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI)
    {
       datesForNode=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DI);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI)
    {
       datesForNode=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RI);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF)
    {
        QSet<QDate> dates1=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DF);
        QSet<QDate> dates2=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RF);
        datesForNode=dates1.intersect(dates2);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI)
    {
        QSet<QDate> dates1=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DI);
        QSet<QDate> dates2=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RI);
        datesForNode=dates1.intersect(dates2);
    }
    else if(selectedMode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI)
    {
        QSet<QDate> dates1=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DF);
        QSet<QDate> dates2=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RF);
        QSet<QDate> dates3=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::DI);
        QSet<QDate> dates4=selectedReporting->getDatesForNodeAndMode(selectedNodeId,MODES::RI);
        datesForNode=dates1.intersect(dates2).intersect(dates3).intersect(dates4);
    }


    foreach(QDate date,datesForNode)
    {
        QDateTime dt(date);
        combo->addItem(date.toString(Qt::DefaultLocaleShortDate),dt.toTime_t());
    }
    combo->addItem("",-1);


}

void FormReportingTables::changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode)
{
    tableOverviewModel->setMode(mode);
    updateComboRefDate(ui->comboBoxRefDate);

    int dateRef=tableOverviewModel->getDateRef();
    if(dateRef!=-1)
    {
        if(!dateExistsForNodeAndMode(dateRef,selectedNodeId,mode))
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
    ui->tableView->resizeColumnsToContents();
}

bool FormReportingTables::dateExistsForNodeAndMode(unsigned int timeT, unsigned int nodeId, PCx_ReportingTableOverviewModel::OVERVIEWMODE mode) const
{
    if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF)
        return selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DF);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF)
        return selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RF);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI)
        return selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DI);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI)
        return selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RI);

    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF)
        return (selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DF) &  selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RF));

    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI)
        return (selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DI) &  selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RI));

    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI)
        return (selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DF) &  selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RF) &
                selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::DI) &  selectedReporting->dateExistsForNodeAndMode(timeT,nodeId,MODES::DFRFDIRI::RI));
    qDebug()<<"Unsupported case found !";
    return false;
}

PCx_ReportingTableOverviewModel::OVERVIEWMODE FormReportingTables::getSelectedMode() const
{
    if(ui->radioButtonDF->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF;
    if(ui->radioButtonRF->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF;
    if(ui->radioButtonDI->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI;
    if(ui->radioButtonRI->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI;
    if(ui->radioButtonRFDF->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF;
    if(ui->radioButtonRIDI->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI;
    if(ui->radioButtonRFDFRIDI->isChecked())
        return PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI;

    else
    {
        qDebug()<<"Invalid selection";
    }
    return PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF;
}

void FormReportingTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    selectedNodeId=nodeId;
    ui->groupBoxMode->setTitle(index.data().toString());
    tableOverviewModel->setNodeId(nodeId);
    updateComboRefDate(ui->comboBoxRefDate);

    int dateRef=tableOverviewModel->getDateRef();
    if(dateRef!=-1)
    {
        if(!dateExistsForNodeAndMode(dateRef,selectedNodeId,getSelectedMode()))
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
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF);
    }
}

void FormReportingTables::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF);
    }
}

void FormReportingTables::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI);
    }
}

void FormReportingTables::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI);
    }
}

void FormReportingTables::on_radioButtonRFDF_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF);
    }
}

void FormReportingTables::on_radioButtonRIDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI);
    }
}

void FormReportingTables::on_radioButtonRFDFRIDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI);
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
    ui->tableView->resizeColumnsToContents();
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
    ui->tableView->resizeColumnsToContents();
}



