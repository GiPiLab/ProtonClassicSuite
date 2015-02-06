#include "formauditprevisions.h"
#include "ui_formauditprevisions.h"
#include "pcx_prevision.h"
#include "utils.h"
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <QMessageBox>

FormAuditPrevisions::FormAuditPrevisions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditPrevisions)
{
    changed=false;
    previsionModel=nullptr;
    auditWithTreeModel=nullptr;
    previsionItemTableModel=nullptr;
    currentPrevisionItem=nullptr;

    currentMode=MODES::DFRFDIRI::DF;
    currentNodeId=1;
    ui->setupUi(this);
    if(!ui->checkBoxDisplayLeafCriteria->isChecked())
    {
        ui->textBrowser->setHidden(true);
    }
    PCx_PrevisionItemCriteria::fillComboBoxWithOperators(ui->comboBoxOperators);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)+"s",PCx_Audit::ORED::OUVERTS);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)+"s",PCx_Audit::ORED::REALISES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)+"s",PCx_Audit::ORED::ENGAGES);
    ui->comboBoxORED->addItem(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)+"s",PCx_Audit::ORED::DISPONIBLES);

    updateListOfPrevisions();
    connect(ui->textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(onAnchorClicked(QUrl)));
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
    if(currentPrevisionItem!=nullptr)
    {
        delete currentPrevisionItem;
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

    QPair<unsigned int, QString> p;
    foreach(p,listOfPrevisions)
    {
        ui->comboListPrevisions->insertItem(0,p.second,p.first);
    }
    ui->comboListPrevisions->setCurrentIndex(0);
    on_comboListPrevisions_activated(0);
}

void FormAuditPrevisions::updatePrevisionItemTableModel()
{
    if(currentPrevisionItem!=nullptr)
    {
        delete currentPrevisionItem;
    }
    currentPrevisionItem=new PCx_PrevisionItem(previsionModel,currentMode,currentNodeId,auditWithTreeModel->getYears().last()+1);
    currentPrevisionItem->loadFromDb();
    if(previsionItemTableModel!=nullptr)
    {
        previsionItemTableModel->setPrevisionItem(currentPrevisionItem);
    }
    else
    {
        previsionItemTableModel=new PCx_PrevisionItemTableModel(currentPrevisionItem,this);
    }
    //NOTE : lastyear of the audit+1 is set here

    ui->tableViewCriteria->setModel(previsionItemTableModel);
}

void FormAuditPrevisions::updateLabels()
{
    ui->labelNodeName->setText(auditWithTreeModel->getAttachedTree()->getNodeName(currentNodeId));
    ui->labelValueN->setText(NUMBERSFORMAT::formatFixedPoint(auditWithTreeModel->getNodeValue(currentNodeId,currentMode,PCx_Audit::ORED::OUVERTS,auditWithTreeModel->getYears().last())));
    ui->labelValueNplus1->setText(NUMBERSFORMAT::formatFixedPoint(currentPrevisionItem->getSummedPrevisionItemValue()));
    ui->labelValuePrevisionItem->setText(NUMBERSFORMAT::formatFixedPoint(currentPrevisionItem->getPrevisionItemValue()));
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
  //  qDebug()<<auditWithTreeModel->getAttachedTree()->getLeavesId(currentNodeId);
    updatePrevisionItemTableModel();
    updateLabels();
    if(ui->textBrowser->isVisible())
        ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());
}


void FormAuditPrevisions::on_comboBoxOperators_activated(int index)
{
    Q_UNUSED(index);
    switch(ui->comboBoxOperators->currentData().toInt())
    {
        case PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUE:
        ui->comboBoxORED->setEnabled(false);
        ui->doubleSpinBox->setEnabled(true);
        ui->doubleSpinBox->setSuffix("€");
        break;
        case PCx_PrevisionItemCriteria::PREVISIONOPERATOR::PERCENT:
        ui->comboBoxORED->setEnabled(true);
        ui->doubleSpinBox->setEnabled(true);
        ui->doubleSpinBox->setSuffix("%");
        break;
    default:
        ui->comboBoxORED->setEnabled(true);
        ui->doubleSpinBox->setEnabled(false);
    }
}

void FormAuditPrevisions::onAnchorClicked(QUrl url)
{
    QStringList nodeString=url.toString().split("_");
    if(nodeString.count()!=2)
    {
        qWarning()<<"Error parsing anchor";
        return;
    }
    unsigned int nodeId=nodeString.at(1).toUInt();

    QModelIndexList indexOfNode=auditWithTreeModel->getAttachedTree()->getIndexOfNodeId(nodeId);
    if(indexOfNode.count()==1)
    {
        const QModelIndex &index=indexOfNode.at(0);
        ui->treeView->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
        ui->treeView->scrollTo(index);
        on_treeView_clicked(index);
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
    currentPrevisionItem->insertCriteriaToAdd(criteria);
    previsionItemTableModel->resetModel();
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
    currentPrevisionItem->insertCriteriaToSub(criteria);
    previsionItemTableModel->resetModel();
    updateLabels();

}

void FormAuditPrevisions::on_pushButtonDelCriteria_clicked()
{
    QItemSelectionModel *selectionModel=ui->tableViewCriteria->selectionModel();
    if(selectionModel->selectedRows().isEmpty())
        return;
    currentPrevisionItem->deleteCriteria(selectionModel->selectedRows());
    previsionItemTableModel->resetModel();
    updateLabels();

}

void FormAuditPrevisions::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::DF;
        updatePrevisionItemTableModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());

    }
}

void FormAuditPrevisions::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::RF;
        updatePrevisionItemTableModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());

    }
}

void FormAuditPrevisions::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::DI;
        updatePrevisionItemTableModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());

    }
}

void FormAuditPrevisions::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        currentMode=MODES::DFRFDIRI::RI;
        updatePrevisionItemTableModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());

    }
}

void FormAuditPrevisions::on_pushButtonDeleteAll_clicked()
{
    changed=true;
    currentPrevisionItem->deleteAllCriteria();
    previsionItemTableModel->resetModel();
    updateLabels();
}

void FormAuditPrevisions::on_pushButtonApplyToNode_clicked()
{
    if(currentPrevisionItem!=nullptr)
    {
        changed=false;
        currentPrevisionItem->saveDataToDb();
        currentPrevisionItem->dispatchComputedValueToChildrenLeaves();
        previsionItemTableModel->resetModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());

    }
}

void FormAuditPrevisions::on_pushButtonApplyToLeaves_clicked()
{
    if(currentPrevisionItem!=nullptr)
    {
        changed=false;
        currentPrevisionItem->saveDataToDb();
        currentPrevisionItem->dispatchCriteriaItemsToChildrenLeaves();
        previsionItemTableModel->resetModel();
        updateLabels();
        if(ui->textBrowser->isVisible())
            ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());
    }
}

void FormAuditPrevisions::on_checkBoxDisplayLeafCriteria_toggled(bool checked)
{
    if(checked)
    {
        ui->textBrowser->setHidden(false);
        ui->textBrowser->setHtml(currentPrevisionItem->getNodePrevisionHTMLReport());
    }
    else
    {
        ui->textBrowser->clear();
        ui->textBrowser->setHidden(true);
    }

}

bool FormAuditPrevisions::displayChangeConfirmationMessage() const
{
    if(question(tr("Les critères de calcul ne seront pas enregistrés s'ils ne sont pas appliqués au noeud"
                " courant ou à ses feuilles, en êtes-vous sûr ?"))==QMessageBox::Yes)
        return true;
    else
        return false;
}
