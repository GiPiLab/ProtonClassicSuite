/*
* ProtonClassicSuite
* 
* Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/

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

    tree=new PCx_Tree(treeId);
    QDateTime dt=QDateTime::currentDateTime();
    ui->label->setText(tr("Arbre %1 le %2 à %3").arg(tree->getName(),dt.date().toString("dd/MM/yyyy"),dt.time().toString()));

    populateTableInfos();
    populateListOfNodesWithSameNameButDifferentTypes();
    ui->tableWidget->resizeColumnsToContents();

    QStringList completeNodeNames=tree->getListOfCompleteNodeNames();

    PCx_NodeSimilarityTableModel *distanceModelWithTypes;
    QSortFilterProxyModel *distanceSortModelWithTypes;

    distanceModelWithTypes=new PCx_NodeSimilarityTableModel(completeNodeNames,PCx_StringDistance::SIMILARITYMETRIC::JAROWINKLERCI,this);
    distanceSortModelWithTypes=new QSortFilterProxyModel(this);
    distanceSortModelWithTypes->setSourceModel(distanceModelWithTypes);
    distanceSortModelWithTypes->sort(1,Qt::DescendingOrder);

    ui->similarityTableView->setModel(distanceSortModelWithTypes);
    ui->similarityTableView->resizeColumnToContents(1);
}

FormTreeConsistency::~FormTreeConsistency()
{
    delete ui;
    delete tree;
}

QSize FormTreeConsistency::sizeHint() const
{
    return {600,500};
}

void FormTreeConsistency::populateListOfNodesWithSameNameButDifferentTypes()
{
    QSet<unsigned int> nodes=tree->getNodesWithSharedName();
    QString nodeName;
    foreach(unsigned int node,nodes)
    {
        nodeName=tree->getNodeName(node);
        ui->similarityNodeNotTypeListWidget->addItem(nodeName);
    }
}

void FormTreeConsistency::populateTableInfos()
{
    ui->tableWidget->item(0,1)->setText(QString::number(tree->getNumberOfNodes()));

    QList<QPair<unsigned int,QString> > listOfTypes=tree->getAllTypes();
    QPair<unsigned int,QString> type;
    int row=ui->tableWidget->rowCount();

    foreach(type,listOfTypes)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        unsigned int nbOfThisType=tree->getNumberOfNodesWithThisType(type.first);
        QTableWidgetItem *item1=new QTableWidgetItem("Noeuds de type "+type.second);
        QTableWidgetItem *item2=new QTableWidgetItem(QString::number(nbOfThisType));
        item2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        row++;
    }
}
