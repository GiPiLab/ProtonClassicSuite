#include "formreports.h"
#include "ui_formreports.h"
#include "pcx_auditmodel.h"

FormReports::FormReports(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReports)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    model=NULL;
    populateLists();
    updateListOfAudits();

}

FormReports::~FormReports()
{
    delete ui;
}

void FormReports::populateLists()
{
    QListWidgetItem *item;
    item=new QListWidgetItem(tr("Récapitulatif"),ui->listTables);
    item->setData(Qt::UserRole+1,T1);
    item=new QListWidgetItem(tr("Évolution cumulée du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T2);
    item=new QListWidgetItem(tr("Évolution du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T2BIS);
    item=new QListWidgetItem(tr("Évolution cumulée du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T3);
    item=new QListWidgetItem(tr("Évolution du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T3BIS);
    item=new QListWidgetItem(tr("Poids relatif de [...] au sein de la collectivité"),ui->listTables);
    item->setData(Qt::UserRole+1,T4);
    item=new QListWidgetItem(tr("Analyse en base 100 du compte administratif de la collectivité hors celui de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T5);
    item=new QListWidgetItem(tr("Analyse en base 100 du compte administratif de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T6);
    item=new QListWidgetItem(tr("Transcription en \"jours/activité\" de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T7);
    item=new QListWidgetItem(tr("Moyennes budgétaires de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T8);
    item=new QListWidgetItem(tr("Equivalences moyennes en \"jours activité\" de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T9);
    item=new QListWidgetItem(tr("Résultats de fonctionnement de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T10);
    item=new QListWidgetItem(tr("Résultats d'investissement de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T11);
    item=new QListWidgetItem(tr("Résultats budgétaire de [...]"),ui->listTables);
    item->setData(Qt::UserRole+1,T12);


    item=new QListWidgetItem(tr("Évolution comparée du prévu de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G1);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du prévu de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G2);
    item=new QListWidgetItem(tr("Évolution comparée du réalisé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G3);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du réalisé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G4);
    item=new QListWidgetItem(tr("Évolution comparée de l'engagé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G5);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé de l'engagé de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G6);
    item=new QListWidgetItem(tr("Évolution comparée du disponible de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G7);
    item=new QListWidgetItem(tr("Évolution comparée du cumulé du disponible de la collectivité et de [...]"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G8);
    item=new QListWidgetItem(tr("Décomposition par année"),ui->listGraphics);
    item->setData(Qt::UserRole+1,G9);



}

void FormReports::onListOfAuditsChanged()
{
    updateListOfAudits();
}


void FormReports::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(FinishedAuditsOnly);
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void FormReports::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {

        delete model;
    }
    model=new PCx_AuditModel(selectedAuditId,this);
    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(model->getAttachedTreeModel());
    delete m;
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);

    ui->comboListTypes->clear();

    QList<QPair<unsigned int,QString> >listOfTypes=model->getAttachedTreeModel()->getTypes()->getTypes();
    Q_ASSERT(!listOfTypes.isEmpty());
    QPair<unsigned int, QString> p;
    foreach(p,listOfTypes)
    {
        ui->comboListTypes->addItem(p.second,p.first);
    }
    ui->comboListTypes->setCurrentIndex(0);
}

void FormReports::on_pushButton_clicked()
{
    QItemSelectionModel *sel=ui->treeView->selectionModel();
    QModelIndexList selIndexes=sel->selectedIndexes();
    foreach (const QModelIndex &index, selIndexes)
    {
        unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
        qDebug()<<"Selected node "<<selectedNode;
    }
}

//NOTE : Preselections are done with fixed indexing, refers to populateLists
void FormReports::on_pushButtonPoidsRelatifs_clicked()
{
    //T1,T4,T8
    ui->listTables->item(0)->setSelected(true);
    ui->listTables->item(5)->setSelected(true);
    ui->listTables->item(9)->setSelected(true);
}

void FormReports::on_pushButtonBase100_clicked()
{
    //T5,T6
    ui->listTables->item(6)->setSelected(true);
    ui->listTables->item(7)->setSelected(true);
}

void FormReports::on_pushButtonEvolution_clicked()
{
    //T2bis,T3bis
    ui->listTables->item(2)->setSelected(true);
    ui->listTables->item(4)->setSelected(true);

    //G1,G3,G5,G7
    ui->listGraphics->item(0)->setSelected(true);
    ui->listGraphics->item(2)->setSelected(true);
    ui->listGraphics->item(4)->setSelected(true);
    ui->listGraphics->item(6)->setSelected(true);
}

void FormReports::on_pushButtonEvolutionCumul_clicked()
{
    //T2,T3
    ui->listTables->item(1)->setSelected(true);
    ui->listTables->item(3)->setSelected(true);

    //G2,G4,G6,G8
    ui->listGraphics->item(1)->setSelected(true);
    ui->listGraphics->item(3)->setSelected(true);
    ui->listGraphics->item(5)->setSelected(true);
    ui->listGraphics->item(7)->setSelected(true);
}

void FormReports::on_pushButtonJoursAct_clicked()
{
    //T7,T9
    ui->listTables->item(8)->setSelected(true);
    ui->listTables->item(10)->setSelected(true);
}

void FormReports::on_pushButtonResultats_clicked()
{
    //T10,T11,T12
    ui->listTables->item(11)->setSelected(true);
    ui->listTables->item(12)->setSelected(true);
    ui->listTables->item(13)->setSelected(true);

    //G9
    ui->listGraphics->item(8)->setSelected(true);

}

void FormReports::on_pushButtonExpandAll_clicked()
{
    ui->treeView->expandAll();
}

void FormReports::on_pushButtonCollapseAll_clicked()
{
    ui->treeView->collapseAll();
}

void FormReports::on_pushButtonSelectAll_clicked()
{
    ui->listTables->selectAll();
    ui->listGraphics->selectAll();
}

void FormReports::on_pushButtonSelectNone_clicked()
{
    ui->listTables->clearSelection();
    ui->listGraphics->clearSelection();
}

void FormReports::on_pushButtonSelectType_clicked()
{
    PCx_TreeModel *treeModel=model->getAttachedTreeModel();
    unsigned int selectedType=ui->comboListTypes->currentData().toUInt();
    QModelIndexList indexOfTypes=treeModel->getIndexesOfTypes(selectedType);

    foreach(const QModelIndex &index,indexOfTypes)
    {
        ui->treeView->selectionModel()->select(index,QItemSelectionModel::Select);
    }
}

void FormReports::on_pushButtonSelectAllNodes_clicked()
{
    ui->treeView->expandAll();
    ui->treeView->selectAll();
}

void FormReports::on_pushButtonUnSelectAllNodes_clicked()
{
    ui->treeView->clearSelection();
}
