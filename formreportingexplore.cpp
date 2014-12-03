#include "formreportingexplore.h"
#include "pcx_report.h"
#include "ui_formreportingexplore.h"


FormReportingExplore::FormReportingExplore(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingExplore)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    document=new QTextDocument();
    report=nullptr;
    updateListOfReportings();
    ui->splitter->setStretchFactor(1,1);
}

FormReportingExplore::~FormReportingExplore()
{
    if(selectedReporting!=nullptr)
        delete selectedReporting;

    if(report!=nullptr)
        delete report;

    if(document!=nullptr)
        delete document;

    delete ui;
}

void FormReportingExplore::onListOfReportingsChanged()
{
    updateListOfReportings();
}

void FormReportingExplore::updateListOfReportings()
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

void FormReportingExplore::on_comboListReportings_activated(int index)
{
    if(index==-1 || ui->comboListReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }


    selectedReporting=new PCx_ReportingWithTreeModel(selectedReportingId);

    if(report!=nullptr)
    {
        delete report;
    }
    report=new PCx_Report(selectedReporting);

    document->clear();


    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNode=1;
    on_treeView_clicked(rootIndex);
}

QList<PCx_Report::PCRPRESETS> FormReportingExplore::getPresets() const
{
    QList<PCx_Report::PCRPRESETS> presets;
    if(ui->checkBoxA->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_A);
    if(ui->checkBoxB->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_B);
    if(ui->checkBoxC->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_C);
    if(ui->checkBoxD->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_D);
    if(ui->checkBoxS->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_S);
    return presets;
}

MODES::DFRFDIRI FormReportingExplore::getSelectedMode() const
{
    if(ui->radioButtonDF->isChecked())
        return MODES::DFRFDIRI::DF;
    if(ui->radioButtonRF->isChecked())
        return MODES::DFRFDIRI::RF;
    if(ui->radioButtonDI->isChecked())
        return MODES::DFRFDIRI::DI;
    if(ui->radioButtonRI->isChecked())
        return MODES::DFRFDIRI::RI;

    else
    {
        qDebug()<<"Invalid selection";
    }
    return MODES::DFRFDIRI::DF;
}


void FormReportingExplore::updateTextEdit()
{
    QString output=selectedReporting->generateHTMLHeader()+selectedReporting->generateHTMLReportingTitle();

    output.append(report->generateHTMLReportingReportForNode(getPresets(),selectedNode,getSelectedMode()));

    ui->textEdit->setHtml(output);
}


void FormReportingExplore::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    selectedNode=nodeId;
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxA_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxB_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxC_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxD_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxS_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}
