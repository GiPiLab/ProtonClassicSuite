#include "formeditaudit.h"
#include "ui_formeditaudit.h"
#include <QDebug>
#include <QMessageBox>
#include <QtGlobal>
#include <QMdiArea>
#include <QMdiSubWindow>
#include "formauditinfos.h"
#include "pcx_auditmanage.h"

FormEditAudit::FormEditAudit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEditAudit)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    ui->tableViewDF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auditModel=NULL;

    delegateDF=new auditDataDelegate(ui->tableViewDF);
    delegateRF=new auditDataDelegate(ui->tableViewRF);
    delegateDI=new auditDataDelegate(ui->tableViewDI);
    delegateRI=new auditDataDelegate(ui->tableViewRI);

    ui->tableViewDF->setItemDelegate(delegateDF);
    ui->tableViewRF->setItemDelegate(delegateRF);
    ui->tableViewDI->setItemDelegate(delegateDI);
    ui->tableViewRI->setItemDelegate(delegateRI);

    updateListOfAudits();
}

FormEditAudit::~FormEditAudit()
{
    delete ui;
    if(auditModel!=NULL)
    {
        delete auditModel;
    }
    delete(delegateDF);
    delete(delegateRF);
    delete(delegateDI);
    delete(delegateRI);
}

void FormEditAudit::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void FormEditAudit::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditManage::getListOfAudits(PCx_AuditManage::UnFinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    ui->splitter->setEnabled(nonEmpty);

    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void FormEditAudit::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    qDebug()<<"Selected audit ID = "<<selectedAuditId;
    if(auditModel!=NULL)
    {
        delete auditModel;
    }

    QItemSelectionModel *m=ui->treeView->selectionModel();
    //Read-write audit model
    auditModel=new PCx_EditableAuditModel(selectedAuditId,0);
    ui->treeView->setModel(auditModel->getAttachedTreeModel());
    delete m;
    ui->treeView->expandToDepth(1);

    ui->tableViewDF->setModel(auditModel->getTableModelDF());
    ui->tableViewDF->hideColumn(0);
    ui->tableViewDF->hideColumn(1);
    ui->tableViewRF->setModel(auditModel->getTableModelRF());
    ui->tableViewRF->hideColumn(0);
    ui->tableViewRF->hideColumn(1);
    ui->tableViewDI->setModel(auditModel->getTableModelDI());
    ui->tableViewDI->hideColumn(0);
    ui->tableViewDI->hideColumn(1);
    ui->tableViewRI->setModel(auditModel->getTableModelRI());
    ui->tableViewRI->hideColumn(0);
    ui->tableViewRI->hideColumn(1);

    //Roots
    auditModel->getTableModelDF()->setFilter(QString("id_node=1"));
    auditModel->getTableModelRF()->setFilter(QString("id_node=1"));
    auditModel->getTableModelDI()->setFilter(QString("id_node=1"));
    auditModel->getTableModelRI()->setFilter(QString("id_node=1"));
    ui->label->setText(auditModel->getAttachedTreeModel()->index(0,0).data().toString());

    ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableViewDF->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode(PCx_EditableAuditModel::COL_ANNEE,QHeaderView::Fixed);
}

void FormEditAudit::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(selectedNode>0);

    auditModel->getTableModelDF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRF()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelDI()->setFilter(QString("id_node=%1").arg(selectedNode));
    auditModel->getTableModelRI()->setFilter(QString("id_node=%1").arg(selectedNode));
    bool isLeaf=auditModel->getAttachedTreeModel()->isLeaf(selectedNode);

    if(isLeaf)
    {
        ui->tableViewDF->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewRF->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewDI->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableViewRI->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    else
    {
        ui->tableViewDF->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewRF->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewDI->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewRI->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    ui->label->setText(index.data().toString());
}

void FormEditAudit::on_randomDataButton_clicked()
{
    if(QMessageBox::question(this,tr("Attention"),tr("Remplir le mode (DF/RF/DI/RI) de l'audit de données aléatoires ?"))==QMessageBox::No)
    {
        return;
    }

    QList<unsigned int> leaves=auditModel->getAttachedTreeModel()->getLeavesId();
    QList<unsigned int> years=auditModel->getYears();

    PCx_AuditManage::DFRFDIRI mode=PCx_AuditManage::DF;
    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=PCx_AuditManage::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=PCx_AuditManage::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=PCx_AuditManage::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=PCx_AuditManage::RI;

    QHash<PCx_AuditManage::ORED,double> data;

    QSqlDatabase::database().transaction();
    foreach(unsigned int leaf,leaves)
    {
        foreach(unsigned int year,years)
        {
            data.clear();
            data.insert(PCx_AuditManage::OUVERTS,(double)(qrand()%100000));
            data.insert(PCx_AuditManage::REALISES,(double)(qrand()%100000));
            data.insert(PCx_AuditManage::ENGAGES,(double)(qrand()%100000));
            auditModel->setLeafValues(leaf,mode,year,data);
        }
    }
    QSqlDatabase::database().commit();
}

void FormEditAudit::on_clearDataButton_clicked()
{
    if(QMessageBox::question(this,tr("Attention"),tr("Effacer toutes les données du mode (DF/RF/DI/RI) sélectionné ?"))==QMessageBox::No)
    {
        return;
    }

    PCx_AuditManage::DFRFDIRI mode=PCx_AuditManage::DF;


    if(ui->tabWidget->currentWidget()==ui->tabDF)
        mode=PCx_AuditManage::DF;
    else if(ui->tabWidget->currentWidget()==ui->tabRF)
        mode=PCx_AuditManage::RF;
    else if(ui->tabWidget->currentWidget()==ui->tabDI)
        mode=PCx_AuditManage::DI;
    else if(ui->tabWidget->currentWidget()==ui->tabRI)
        mode=PCx_AuditManage::RI;

    auditModel->clearAllData(mode);
}

void FormEditAudit::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
    ui->treeView->expandToDepth(0);
}

void FormEditAudit::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormEditAudit::on_statsButton_clicked()
{
    FormAuditInfos *infos=new FormAuditInfos(auditModel->getAuditId(),this);
    infos->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow *mdiSubWin=(QMdiSubWindow *)this->parentWidget();
    QMdiArea *mdiArea=mdiSubWin->mdiArea();
    mdiArea->addSubWindow(infos);
    infos->show();
}
