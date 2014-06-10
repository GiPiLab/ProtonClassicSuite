#include "formeditaudit.h"
#include "ui_formeditaudit.h"
#include "pcx_auditmodel.h"
#include "auditdatadelegate.h"

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

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(UnFinishedAuditsOnly);
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
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;
    if(auditModel!=NULL)
    {
        delete auditModel;
    }

    QItemSelectionModel *m=ui->treeView->selectionModel();
    //Read-write audit model
    auditModel=new PCx_AuditModel(selectedAuditId,0,false);
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

    ui->tableViewDF->setEnabled(false);
    ui->tableViewRF->setEnabled(false);
    ui->tableViewDI->setEnabled(false);
    ui->tableViewRI->setEnabled(false);

    ui->tableViewDF->horizontalHeader()->setSectionResizeMode(COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRF->horizontalHeader()->setSectionResizeMode(COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewDI->horizontalHeader()->setSectionResizeMode(COL_ANNEE,QHeaderView::Fixed);
    ui->tableViewRI->horizontalHeader()->setSectionResizeMode(COL_ANNEE,QHeaderView::Fixed);
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

    ui->tableViewDF->setEnabled(isLeaf);
    ui->tableViewRF->setEnabled(isLeaf);
    ui->tableViewDI->setEnabled(isLeaf);
    ui->tableViewRI->setEnabled(isLeaf);
    ui->label->setText(index.data().toString());
}
