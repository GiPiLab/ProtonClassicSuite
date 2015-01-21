#include "formauditprevisions.h"
#include "ui_formauditprevisions.h"
#include "pcx_prevision.h"

FormAuditPrevisions::FormAuditPrevisions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditPrevisions)
{
    previsionModel=nullptr;
    ui->setupUi(this);
    updateListOfPrevisions();
}

FormAuditPrevisions::~FormAuditPrevisions()
{
    delete ui;
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
        QModelIndex rootIndex=previsionModel->getAttachedAudit()->getAttachedTree()->index(0,0);
        on_treeView_clicked(rootIndex);
    }
}

void FormAuditPrevisions::updateListOfPrevisions()
{
    ui->comboListPrevisions->clear();

    QList<QPair<unsigned int,QString> >listOfPrevisions=PCx_Prevision::getListOfPrevisions();
    //do not update text browser if no audit are available
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


void FormAuditPrevisions::on_comboListPrevisions_activated(int index)
{
    if(index==-1||ui->comboListPrevisions->count()==0)return;
    unsigned int selectedPrevisionId=ui->comboListPrevisions->currentData().toUInt();
    Q_ASSERT(selectedPrevisionId>0);
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(previsionModel!=nullptr)
    {
        delete previsionModel;
    }
    previsionModel=new PCx_Prevision(selectedPrevisionId);

    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(previsionModel->getAttachedAudit()->getAttachedTree());
    delete m;
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=previsionModel->getAttachedAudit()->getAttachedTree()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
    //updateTextBrowser();
}

void FormAuditPrevisions::on_treeView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
   // ui->groupBoxMode->setTitle(index.data().toString());

   // updateTextBrowser();
}
