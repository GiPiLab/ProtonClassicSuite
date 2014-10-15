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
    loadTypesTableModel();
}

PCx_TreeModel::~PCx_TreeModel()
{
    clear();
    typesTableModel->clear();
    delete typesTableModel;
}

unsigned int PCx_TreeModel::addNode(unsigned int pid, unsigned int typeId, const QString &name, const QModelIndex &pidNodeIndex)
{
    unsigned int newId;
    newId=PCx_Tree::addNode(pid,typeId,name);
    if(pidNodeIndex.isValid() && newId>0)
    {
        QStandardItem *pidItem=this->itemFromIndex(pidNodeIndex);
        QStandardItem *newitem=createItem(idTypeToName(typeId),name,typeId,newId);
        pidItem->appendRow(newitem);
    }
    return newId;
}

bool PCx_TreeModel::updateTree()
{
    QElapsedTimer timer;
    timer.start();
    bool res;
    clear();
    res=createChildrenItems(invisibleRootItem(),0);
    qDebug()<<"updateTree done in"<<timer.elapsed()<<"ms";
    return res;
}


/*
 * Drag and drop : uses QStandardItemModel to compute the logic with items
 *
 * - Check validity (no dd outside of the tree)
 * - get DB IDs of elements
 * - update DB
 * - let QStandardItemModel::dropMimeData do the job with items
 */


//TODO : DRAG&DROP BETWEEN TREES
bool PCx_TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{

    if(!parent.isValid())
    {
        qDebug()<<"Dropping outside of the root";
        return false;
    }

    unsigned int dropId;

    dropId=parent.data(Qt::UserRole+1).toUInt();
    qDebug()<<"DROPID="<<dropId;

    QStandardItemModel tmpModel;
    tmpModel.dropMimeData(data,Qt::CopyAction,0,0,QModelIndex());

    for(int i=0;i<tmpModel.rowCount();i++)
    {
        //TODO : check for duplicates nodes under the same pid
        unsigned int dragId=tmpModel.item(i)->data(Qt::UserRole+1).toUInt();
        if(dragId!=1)
            updateNodePid(dragId,dropId);
    }

    return QStandardItemModel::dropMimeData(data,action,row,column,parent);
}

bool PCx_TreeModel::createChildrenItems(QStandardItem *item,unsigned int nodeId)
{
    QSqlQuery query(QString("select * from arbre_%1 where pid=%2 order by nom").arg(treeId).arg(nodeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        return false;
    }
    while(query.next())
    {
        unsigned int typeId=query.value(3).toUInt();
        unsigned int nodeId=query.value(0).toUInt();
        QStandardItem *newitem=createItem(idTypeToName(typeId),query.value(1).toString(),typeId,nodeId);

        item->appendRow(newitem);
        createChildrenItems(newitem,nodeId);
    }
    return true;

}

QStandardItem *PCx_TreeModel::createItem(const QString &typeName, const QString &nodeName, unsigned int typeId, unsigned int nodeId)
{
    Q_ASSERT(!nodeName.isEmpty());
    //QStandardItem *newitem=new QStandardItem(typeName+" "+nodeName);
    QStandardItem *newitem=new QStandardItem(typeName+" "+nodeName);
    newitem->setData(nodeId,Qt::UserRole+1);
    newitem->setData(typeId,Qt::UserRole+2);
//    newitem->setIcon(QIcon::fromTheme());
    return newitem;
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType)
{
    Q_ASSERT(nodeIndex.isValid());
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();

    qDebug()<<"Node to be updated = "<<nodeId;

    bool res=PCx_Tree::updateNode(nodeId,newName,newType);

    if(res==false)
        return false;

    QStandardItem *item=this->itemFromIndex(nodeIndex);
    item->setText(QString("%1 %2").arg(idTypeToName(newType)).arg(newName));
    item->setData(newType,Qt::UserRole+2);

    return true;
}

bool PCx_TreeModel::deleteNode(const QModelIndex &nodeIndex)
{
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(nodeId>0);
    this->removeRow(nodeIndex.row(),nodeIndex.parent());
    return PCx_Tree::deleteNode(nodeId);
}

QModelIndexList PCx_TreeModel::getIndexesOfNodesWithThisType(unsigned int typeId) const
{
    return match(index(0,0),Qt::UserRole+2,QVariant(typeId),-1,Qt::MatchRecursive);
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

bool PCx_TreeModel::loadTypesTableModel()
{
    typesTableModel->setTable(QString("types_%1").arg(treeId));
    typesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    typesTableModel->select();
    connect(typesTableModel,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onTypesModelDataChanged(const QModelIndex &, const QModelIndex &)));
    return true;
}

/*Check if the new type does not already exists in the table
 * Assumes that only one row is modified at once
 */
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
        loadTypesFromDatabase();
        emit typesUpdated();
        return true;
    }
}
