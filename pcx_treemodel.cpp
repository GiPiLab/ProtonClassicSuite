/*
* ProtonClassicSuite
* 
* Copyright Thibault Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
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

#include "pcx_treemodel.h"
#include <QStandardItem>
#include <QDebug>
#include <QElapsedTimer>
#include <QSqlQuery>
#include <QSqlError>


PCx_TreeModel::PCx_TreeModel(unsigned int treeId, QObject *parent) :
    QStandardItemModel(parent),PCx_Tree(treeId)
{    
    updateTree();

    typesTableModel=new QSqlTableModel();
    typesTableModel->setTable(QString("types_%1").arg(treeId));
    typesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    typesTableModel->select();
    connect(typesTableModel,&QAbstractItemModel::dataChanged,this,&PCx_TreeModel::onTypesModelDataChanged);
}

PCx_TreeModel::~PCx_TreeModel()
{
    clear();
    typesTableModel->clear();
    delete typesTableModel;
}

unsigned int PCx_TreeModel::addNode(unsigned int typeId, const QString &name, const QModelIndex &pidNodeIndex)
{
    unsigned int newId;
    unsigned int pid=pidNodeIndex.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    newId=PCx_Tree::addNode(pid,typeId,name);
    if(pidNodeIndex.isValid() && newId>0)
    {
        QStandardItem *pidItem=this->itemFromIndex(pidNodeIndex);
        QStandardItem *newitem=createItem(typeId,newId);
        pidItem->appendRow(newitem);
    }
    return newId;
}

bool PCx_TreeModel::updateTree()
{
    bool res;
    clear();
    res=createChildrenItems(invisibleRootItem());
    return res;
}




//TODO : DRAG&DROP BETWEEN TREES
bool PCx_TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{

    if(!parent.isValid())
    {
       // qDebug()<<"Dropping outside of the root";
        return false;
    }

    unsigned int dropId;

    dropId=parent.data(PCx_TreeModel::NodeIdUserRole).toUInt();
   // qDebug()<<"DROPID="<<dropId;

    QStandardItemModel tmpModel;
    tmpModel.dropMimeData(data,Qt::CopyAction,0,0,QModelIndex());

    QSqlDatabase::database().transaction();
    for(int i=0;i<tmpModel.rowCount();i++)
    {
        //TODO : check for duplicates nodes under the same pid
        unsigned int dragId=tmpModel.item(i)->data(PCx_TreeModel::NodeIdUserRole).toUInt();
        if(dragId!=1)
            updateNodePid(dragId,dropId);
    }
    QSqlDatabase::database().commit();

    return QStandardItemModel::dropMimeData(data,action,row,column,parent);
}

bool PCx_TreeModel::createChildrenItems(QStandardItem *item,unsigned int nodeId)
{
    QSqlQuery query;
    query.exec(QString("select * from arbre_%1 where pid=%2 order by nom").arg(treeId).arg(nodeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        return false;
    }
    while(query.next())
    {
        unsigned int typeId=query.value(3).toUInt();
        unsigned int nodeId=query.value(0).toUInt();
        QStandardItem *newitem=createItem(typeId,nodeId);
        item->appendRow(newitem);
        createChildrenItems(newitem,nodeId);
    }
    return true;

}

QStandardItem *PCx_TreeModel::createItem(unsigned int typeId, unsigned int nodeId)
{
    QStandardItem *newitem=new QStandardItem(getNodeName(nodeId));
    newitem->setData(nodeId,PCx_TreeModel::NodeIdUserRole);
    newitem->setData(typeId,PCx_TreeModel::TypeIdUserRole);
//    newitem->setIcon(QIcon::fromTheme());
    return newitem;
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType)
{
    Q_ASSERT(nodeIndex.isValid());
    unsigned int nodeId=nodeIndex.data(PCx_TreeModel::NodeIdUserRole).toUInt();

    //qDebug()<<"Node to be updated = "<<nodeId;

    bool res=PCx_Tree::updateNode(nodeId,newName,newType);

    if(res==false)
        return false;

    QStandardItem *item=this->itemFromIndex(nodeIndex);
    //item->setText(QString("%1 %2").arg(idTypeToName(newType)).arg(newName));
    item->setText(getNodeName(nodeId));
    item->setData(newType,PCx_TreeModel::TypeIdUserRole);

    return true;
}

bool PCx_TreeModel::deleteNode(const QModelIndex &nodeIndex)
{
    unsigned int nodeId=nodeIndex.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    Q_ASSERT(nodeId>0);
    this->removeRow(nodeIndex.row(),nodeIndex.parent());
    return PCx_Tree::deleteNode(nodeId);
}

QModelIndexList PCx_TreeModel::getIndexesOfNodesWithThisType(unsigned int typeId) const
{
    return match(index(0,0),PCx_TreeModel::TypeIdUserRole,QVariant(typeId),-1,Qt::MatchRecursive);
}

QModelIndexList PCx_TreeModel::getIndexOfNodeId(unsigned int nodeId) const
{
    return match(index(0,0),PCx_TreeModel::NodeIdUserRole,QVariant(nodeId),-1,Qt::MatchRecursive);
}

int PCx_TreeModel::guessHierarchy()
{
    int count=PCx_Tree::guessHierarchy();
    if(count>0)
    {
        clear();
        updateTree();
    }
    return count;
}

unsigned int PCx_TreeModel::addType(const QString &typeName)
{
    unsigned int typeId=PCx_Tree::addType(typeName);
    typesTableModel->select();
    return typeId;
}

bool PCx_TreeModel::deleteType(unsigned int typeId)
{
    bool res=PCx_Tree::deleteType(typeId);
    typesTableModel->select();
    return res;
}


bool PCx_TreeModel::onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(bottomRight);

    QString newType=topLeft.data().toString();
    if(validateType(newType)==false)
    {
        //In order to prevent reentrant "datachanged" signal with revertAll
        bool oldState=typesTableModel->blockSignals(true);
        typesTableModel->revertAll();
        typesTableModel->blockSignals(oldState);
        return false;
    }

    else
    {
        //NOTE : simplify string
        typesTableModel->setData(topLeft,newType.simplified());
        typesTableModel->submitAll();
        loadTypesFromDb();
        emit typesUpdated();
        return true;
    }
}
