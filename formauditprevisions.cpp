#include "formauditprevisions.h"
#include "ui_formauditprevisions.h"
#include "pcx_prevision.h"
#include "utils.h"
#include <QListWidgetItem>

FormAuditPrevisions::FormAuditPrevisions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditPrevisions)
{
    previsionModel=nullptr;
    auditWithTreeModel=nullptr;
    previsionItemTableModel=nullptr;
    currentMode=MODES::DFRFDIRI::DF;
    currentNodeId=1;
    ui->setupUi(this);
    PCx_PrevisionItemCriteria::fillComboBoxWithOperators(ui->comboBoxOperators);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)+"s",PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)+"s",PCx_Audit::ORED::REALISES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)+"s",PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)+"s",PCx_Audit::ORED::DISPONIBLES);

    updateListOfPrevisions();
}


FormAuditPrevisions::~FormAuditPrevisions()
{
    delete ui;
    if(previsionModel!=nullptr)
    {
        delete previsionModel;
    }
    if(auditWithTreeModel!=nullptr)
    {
        delete auditWithTreeModel;
    }
    if(previsionItemTableModel!=nullptr)
    {
        delete previsionItemTableModel;
    }
}

void FormAuditPrevisions::onListOfPrevisionsChanged()
{
    updateListOfPrevisions();
}

void FormAuditPrevisions::onSettingsChanged()
{
    if(ui->treeView->currentIndex().isValid())
    {
        on_treeView_clicked(ui->treeView->currentIndex());
    }
    else
    {
        QModelIndex rootIndex=auditWithTreeModel->getAttachedTree()->index(0,0);
        on_treeView_clicked(rootIndex);
    }
}

void FormAuditPrevisions::updateListOfPrevisions()
{
    ui->comboListPrevisions->clear();

    QList<QPair<unsigned int,QString> >listOfPrevisions=PCx_Prevision::getListOfPrevisions();
    bool nonEmpty=!listOfPrevisions.isEmpty();
    this->setEnabled(nonEmpty);
    //doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));

    QPair<unsigned int, QString> p;
    foreach(p,listOfPrevisions)
    {
        ui->comboListPrevisions->insertItem(0,p.second,p.first);
    }
    ui->comboListPrevisions->setCurrentIndex(0);
    on_comboListPrevisions_activated(0);
}

void FormAuditPrevisions::updatePrevisionItemListModel()
{
    if(previsionItemTableModel!=nullptr)
    {
        delete previsionItemTableModel;
    }
    //NOTE : lastyear of the audit+1 is set here
    previsionItemTableModel=new PCx_PrevisionItemTableModel(previsionModel,currentMode,currentNodeId,auditWithTreeModel->getYears().last()+1);
    ui->tableViewCriteria->setModel(previsionItemTableModel);


}

void FormAuditPrevisions::updateLabels()
{
    ui->labelNodeName->setText(auditWithTreeModel->getAttachedTree()->getNodeName(currentNodeId));
    ui->labelValueN->setText(NUMBERSFORMAT::formatFixedPoint(auditWithTreeModel->getNodeValue(currentNodeId,currentMode,PCx_Audit::ORED::OUVERTS,auditWithTreeModel->getYears().last())));
    ui->labelValueNplus1->setText(NUMBERSFORMAT::formatFixedPoint(previsionItemTableModel->computeTotal()));
    ui->tableViewCriteria->resizeColumnsToContents();
}


void FormAuditPrevisions::on_comboListPrevisions_activated(int index)
{
    if(index==-1||ui->comboListPrevisions->count()==0)return;
    unsigned int selectedPrevisionId=ui->comboListPrevisions->currentData().toUInt();
    Q_ASSERT(selectedPrevisionId>0);
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(previsionModel!=nullptr)
    {
        delete previsionModel;
        previsionModel=nullptr;
    }
    if(auditWithTreeModel!=nullptr)
    {
        delete auditWithTreeModel;
        auditWithTreeModel=nullptr;
    }
    previsionModel=new PCx_Prevision(selectedPrevisionId);
    auditWithTreeModel=new PCx_AuditWithTreeModel(previsionModel->getAttachedAuditId());

    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(auditWithTreeModel->getAttachedTree());
    delete m;
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=auditWithTreeModel->getAttachedTree()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
}

void FormAuditPrevisions::on_treeView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    currentNodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    updatePrevisionItemListModel();
    updateLabels();
}


void FormAuditPrevisions::on_comboBoxOperators_activated(int index)
{
    Q_UNUSED(index);
    switch(ui->comboBoxOperators->currentData().toInt())
    {
        case PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFORLEAVES:
        case PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUEFORNODE:
        ui->comboBoxORED->setEnabled(false);
        ui->doubleSpinBox->setEnabled(true);
        break;
        case PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT:
        ui->comboBoxORED->setEnabled(true);
        ui->doubleSpinBox->setEnabled(true);
        break;
    default:
        ui->comboBoxORED->setEnabled(true);
        ui->doubleSpinBox->setEnabled(false);
    }
}


void FormAuditPrevisions::on_pushButtonAddCriteriaToAdd_clicked()
{
    PCx_PrevisionItemCriteria::PREVISIONOPERATOR prevop=(PCx_PrevisionItemCriteria::PREVISIONOPERATOR)ui->comboBoxOperators->currentData().toUInt();
    PCx_Audit::ORED ored=(PCx_Audit::ORED)ui->comboBoxORED->currentData().toUInt();
    qint64 operand=0;
    if(ui->doubleSpinBox->isEnabled())
    {
        operand=NUMBERSFORMAT::doubleToFixedPoint(ui->doubleSpinBox->value());
    }
    PCx_PrevisionItemCriteria criteria(prevop,ored,operand);
    previsionItemTableModel->insertCriteriaToAdd(criteria);
    updateLabels();
}

void FormAuditPrevisions::on_pushButtonAddCriteriaToSubstract_clicked()
{
    PCx_PrevisionItemCriteria::PREVISIONOPERATOR prevop=(PCx_PrevisionItemCriteria::PREVISIONOPERATOR)ui->comboBoxOperators->currentData().toUInt();
    PCx_Audit::ORED ored=(PCx_Audit::ORED)ui->comboBoxORED->currentData().toUInt();
    qint64 operand=0;
    if(ui->doubleSpinBox->isEnabled())
    {
        operand=NUMBERSFORMAT::doubleToFixedPoint(ui->doubleSpinBox->value());
    }
    PCx_PrevisionItemCriteria criteria(prevop,ored,operand);
    previsionItemTableModel->insertCriteriaToSub(criteria);
    updateLabels();

}

void FormAuditPrevisions::on_pushButtonDelCriteria_clicked()
{
    QItemSelectionModel *selectionModel=ui->tableViewCriteria->selectionModel();
    previsionItemTableModel->deleteCriteria(selectionModel->selectedRows());
    updateLabels();


}

void FormAuditPrevisions::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::DF;
        updatePrevisionItemListModel();
        updateLabels();
    }
}

void FormAuditPrevisions::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::RF;
        updatePrevisionItemListModel();
        updateLabels();
    }
}

void FormAuditPrevisions::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::DI;
        updatePrevisionItemListModel();
        updateLabels();
    }
}

void FormAuditPrevisions::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::RI;
        updatePrevisionItemListModel();
        updateLabels();
    }
}

void FormAuditPrevisions::on_pushButtonDeleteAll_clicked()
{
    previsionItemTableModel->deleteAllCriteria();
    updateLabels();
}
