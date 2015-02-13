#include "formreportingoverview.h"
#include "ui_formreportingoverview.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

FormReportingOverview::FormReportingOverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingOverview)
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

FormReportingOverview::~FormReportingOverview()
{
    delete ui;
    if(proxyModel!=nullptr)
        delete proxyModel;
    if(selectedReporting!=nullptr)
        delete selectedReporting;
    if(tableOverviewModel!=nullptr)
        delete tableOverviewModel;
}

void FormReportingOverview::onListOfReportingsChanged()
{
    updateListOfReportings();
}

void FormReportingOverview::onReportingDataChanged(unsigned int reportingId)
{
    if(selectedReporting->getReportingId()==reportingId)
    {
        if(ui->treeView->currentIndex().isValid())
        {
            on_treeView_clicked(ui->treeView->currentIndex());
        }
        else
        {
            QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
            on_treeView_clicked(rootIndex);
        }
    }
}

void FormReportingOverview::on_comboListReportings_activated(int index)
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
    proxyModel->setSortRole(Qt::EditRole);
    proxyModel->setSourceModel(tableOverviewModel);

    ui->tableView->setModel(proxyModel);
    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNodeId=1;
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
    ui->tableView->resizeColumnsToContents();
    ui->textEdit->clear();
}

void FormReportingOverview::updateListOfReportings()
{
    ui->comboListReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> p;
    if(listOfReportings.count()==0)
    {
        setEnabled(false);
        return;
    }
    else
        setEnabled(true);

    foreach(p,listOfReportings)
    {
        ui->comboListReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListReportings->setCurrentIndex(0);
    this->on_comboListReportings_activated(0);
}

void FormReportingOverview::updateComboRefDate(QComboBox *combo)
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

void FormReportingOverview::changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode)
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

bool FormReportingOverview::dateExistsForNodeAndMode(unsigned int timeT, unsigned int nodeId, PCx_ReportingTableOverviewModel::OVERVIEWMODE mode) const
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
    qWarning()<<"Unsupported case found !";
    return false;
}

PCx_ReportingTableOverviewModel::OVERVIEWMODE FormReportingOverview::getSelectedMode() const
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
        qWarning()<<"Invalid selection";
    }
    return PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF;
}

void FormReportingOverview::on_treeView_clicked(const QModelIndex &index)
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
    ui->tableView->resizeColumnsToContents();
}

void FormReportingOverview::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF);
    }
}

void FormReportingOverview::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF);
    }
}

void FormReportingOverview::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI);
    }
}

void FormReportingOverview::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI);
    }
}

void FormReportingOverview::on_radioButtonRFDF_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF);
    }
}

void FormReportingOverview::on_radioButtonRIDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI);
    }
}

void FormReportingOverview::on_radioButtonRFDFRIDI_toggled(bool checked)
{
    if(checked)
    {
        changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI);
    }
}

void FormReportingOverview::on_checkBoxBP_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxReports_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxOCDM_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxVCDM_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxBudgetVote_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxVCInternes_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxRattaches_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxOuverts_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxRealises_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxEngages_toggled(bool checked)
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

void FormReportingOverview::on_checkBoxDisponibles_toggled(bool checked)
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

void FormReportingOverview::on_pushButtonCopyToDocument_clicked()
{
    QTextCursor cursor=ui->textEdit->textCursor();
    cursor.insertHtml(QString("<p>%1 - %2</p>").arg(selectedReporting->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped()).arg(PCx_ReportingTableOverviewModel::OVERVIEWMODEToCompleteString(getSelectedMode())));
    cursor.insertHtml(qTableViewToHtml(ui->tableView));
}

void FormReportingOverview::on_comboBoxRefColumn_activated(int index)
{
    Q_UNUSED(index);
    if(ui->comboBoxRefColumn->count()==0)
        return;
    PCx_Reporting::OREDPCR selectedOred=(PCx_Reporting::OREDPCR)ui->comboBoxRefColumn->currentData().toInt();
    tableOverviewModel->setColRef(selectedOred);
    if(selectedOred!=PCx_Reporting::OREDPCR::NONELAST)
    {
        ui->comboBoxRefDate->setCurrentIndex(ui->comboBoxRefDate->count());
    }
    ui->tableView->resizeColumnsToContents();
}

void FormReportingOverview::on_comboBoxRefDate_activated(int index)
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



void FormReportingOverview::on_pushButtonClear_clicked()
{
    ui->textEdit->clear();
}

void FormReportingOverview::on_pushButtonSelectAll_clicked()
{
    ui->checkBoxBP->setChecked(true);
    ui->checkBoxBudgetVote->setChecked(true);
    ui->checkBoxDisponibles->setChecked(true);
    ui->checkBoxRealises->setChecked(true);
    ui->checkBoxEngages->setChecked(true);
    ui->checkBoxOCDM->setChecked(true);
    ui->checkBoxOuverts->setChecked(true);
    ui->checkBoxRattaches->setChecked(true);
    ui->checkBoxReports->setChecked(true);
    ui->checkBoxVCDM->setChecked(true);
    ui->checkBoxVCInternes->setChecked(true);
    ui->checkBoxVCInternes->setChecked(true);
}

void FormReportingOverview::on_pushButtonSelectNone_clicked()
{
    ui->checkBoxBP->setChecked(false);
    ui->checkBoxBudgetVote->setChecked(false);
    ui->checkBoxDisponibles->setChecked(false);
    ui->checkBoxRealises->setChecked(false);
    ui->checkBoxEngages->setChecked(false);
    ui->checkBoxOCDM->setChecked(false);
    ui->checkBoxOuverts->setChecked(false);
    ui->checkBoxRattaches->setChecked(false);
    ui->checkBoxReports->setChecked(false);
    ui->checkBoxVCDM->setChecked(false);
    ui->checkBoxVCInternes->setChecked(false);
    ui->checkBoxVCInternes->setChecked(false);

}

void FormReportingOverview::on_pushButtonExportHTML_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        return;
    }

    QTextDocument doc;
    doc.setHtml(selectedReporting->generateHTMLReportingTitle()+ui->textEdit->toHtml());

    QString output=doc.toHtml("utf-8");
    output.replace(" -qt-block-indent:0;","");

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<output;
    stream.flush();
    file.close();
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré.").arg(fi.fileName().toHtmlEscaped()));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));

}

void FormReportingOverview::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormReportingOverview::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
    ui->treeView->expandToDepth(0);
}
