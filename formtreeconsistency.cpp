#include "formtreeconsistency.h"
#include "ui_formtreeconsistency.h"
#include "pcx_nodesimilaritytablemodel.h"
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QDebug>

FormTreeConsistency::FormTreeConsistency(unsigned int treeId,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTreeConsistency)
{
    ui->setupUi(this);

    treeModel=new PCx_TreeModel(treeId,true,true,this);
    QDateTime dt=QDateTime::currentDateTime();
    ui->label->setText(tr("Arbre %1 le %2 à %3").arg(treeModel->getName()).arg(dt.date().toString("dd/MM/yyyy")).arg(dt.time().toString()));

    populateTableInfos();
    populateListOfNodesWithSameNameButDifferentTypes();
    ui->tableWidget->resizeColumnsToContents();

    QStringList completeNodeNames=treeModel->getListOfCompleteNodeNames();

    PCx_NodeSimilarityTableModel *distanceModelWithTypes;
    QSortFilterProxyModel *distanceSortModelWithTypes;

    distanceModelWithTypes=new PCx_NodeSimilarityTableModel(completeNodeNames,PCx_StringDistance::JAROWINKLERCI,this);
    distanceSortModelWithTypes=new QSortFilterProxyModel(this);
    distanceSortModelWithTypes->setSourceModel(distanceModelWithTypes);
    distanceSortModelWithTypes->sort(1,Qt::DescendingOrder);

    ui->similarityTableView->setModel(distanceSortModelWithTypes);
    ui->similarityTableView->resizeColumnToContents(1);
}

FormTreeConsistency::~FormTreeConsistency()
{
    delete ui;
    delete treeModel;
}

QSize FormTreeConsistency::sizeHint() const
{
    return QSize(500,400);
}

void FormTreeConsistency::populateListOfNodesWithSameNameButDifferentTypes()
{
    QSet<unsigned int> nodes=treeModel->getNodesWithSharedName();
    QString nodeName;
    foreach(unsigned int node,nodes)
    {
        nodeName=treeModel->getNodeName(node);
        ui->similarityNodeNotTypeListWidget->addItem(nodeName);
    }
}

void FormTreeConsistency::populateTableInfos()
{
    ui->tableWidget->item(0,1)->setText(QString::number(treeModel->getNumberOfNodes()));

    QList<QPair<unsigned int,QString> > listOfTypes=treeModel->getTypes()->getAllTypes();
    QPair<unsigned int,QString> type;
    int row=ui->tableWidget->rowCount();

    foreach(type,listOfTypes)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        unsigned int nbOfThisType=treeModel->getNumberOfNodesWithThisType(type.first);
        QTableWidgetItem *item1=new QTableWidgetItem("Noeuds de type "+type.second);
        QTableWidgetItem *item2=new QTableWidgetItem(QString::number(nbOfThisType));
        item2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        row++;
    }
}
