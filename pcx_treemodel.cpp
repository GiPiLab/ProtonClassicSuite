#include "pcx_treemodel.h"
#include "utils.h"
#include "pcx_typemodel.h"
#include "xlsxdocument.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QUuid>
#include <QElapsedTimer>
#include <QFileInfo>

PCx_TreeModel::PCx_TreeModel(unsigned int treeId, bool typesReadOnly, bool noLoadModel, QObject *parent):QStandardItemModel(parent),noLoadModel(noLoadModel)
{
    types=new PCx_TypeModel(treeId,typesReadOnly);

    if(loadFromDatabase(treeId)==false)
    {
        qCritical()<<"Unable to load tree"<<treeId;
        die();
    }
}

PCx_TreeModel::~PCx_TreeModel()
{
    this->clear();
    delete types;
    types=nullptr;
}

unsigned int PCx_TreeModel::addNode(unsigned int pid, unsigned int type, const QString &name, const QModelIndex &pidNodeIndex)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(pid>0);
    Q_ASSERT(type>0);

    if(getNumberOfNodes()>=MAXNODES)
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Nombre maximum de noeuds atteint !"));
        return 0;
    }

    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":type",type);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
            return 0;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":pid",pid);
    q.bindValue(":type",type);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    //Also add an item to the model
    if(pidNodeIndex.isValid())
    {
        QStandardItem *pidItem=this->itemFromIndex(pidNodeIndex);
        QStandardItem *newitem=createItem(types->idTypeToName(type),name,type,q.lastInsertId().toUInt());
        pidItem->appendRow(newitem);
    }
    return q.lastInsertId().toUInt();
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType)
{
    Q_ASSERT(nodeIndex.isValid());
    Q_ASSERT(!newName.isNull());
    Q_ASSERT(!newName.isEmpty());
    Q_ASSERT(newType>0);
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();
    qDebug()<<"Node to be updated = "<<nodeId;

    QSqlQuery q;

    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",newName);
    q.bindValue(":type",newType);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    q.prepare(QString("update arbre_%1 set nom=:nom, type=:type where id=:id").arg(treeId));
    q.bindValue(":nom",newName);
    q.bindValue(":type",newType);
    q.bindValue(":id",nodeId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    QStandardItem *item=this->itemFromIndex(nodeIndex);
    item->setText(QString("%1 %2").arg(types->idTypeToName(newType)).arg(newName));
    item->setData(newType,Qt::UserRole+2);

    return true;
}

bool PCx_TreeModel::deleteNode(const QModelIndex &nodeIndex)
{
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(nodeId>0);
    this->removeRow(nodeIndex.row(),nodeIndex.parent());
    return deleteNodeAndChildren(nodeId);
}




QList<unsigned int> PCx_TreeModel::getNodesId() const
{
    return PCx_TreeModel::getNodesId(this->treeId);
}

QStringList PCx_TreeModel::getListOfCompleteNodeNames() const
{
    QStringList nodeNames;
    QSqlQuery q;
    if(!q.exec(QString("select nom,type from arbre_%1 where id>1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        return QStringList();
    }
    while(q.next())
    {
        nodeNames.append(QString("%1 %2").arg(types->idTypeToName(q.value("type").toUInt())).arg(q.value("nom").toString()));
    }
    return nodeNames;
}

QStringList PCx_TreeModel::getListOfNodeNames() const
{
    QStringList nodeNames;
    QSqlQuery q;
    if(!q.exec(QString("select distinct nom from arbre_%1 where id>1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        return QStringList();
    }
    while(q.next())
    {
        nodeNames.append(q.value("nom").toString());
    }
    return nodeNames;
}

unsigned int PCx_TreeModel::getNumberOfNodes() const
{
    return PCx_TreeModel::getNumberOfNodes(treeId);
}

unsigned int PCx_TreeModel::getNumberOfNodes(unsigned int treeId)
{
    QSqlQuery q(QString("select count(*) from arbre_%1").arg(treeId));
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_TreeModel::getLeavesId() const
{
    QList<unsigned int> leaves;
    QList<unsigned int> nodes;
    nodes=getNodesId();
    foreach (unsigned int node, nodes) {
        if(isLeaf(node))
        {
            leaves.append(node);
        }
    }
    //qDebug()<<"Leaves for tree "<<treeId<< " = "<<leaves;
    return leaves;
}

QList<unsigned int> PCx_TreeModel::getNodesId(unsigned int treeId)
{
    Q_ASSERT(treeId>0);
    QSqlQuery q;
    QList<unsigned int> nodeIds;
    q.exec(QString("select id from arbre_%1").arg(treeId));
    while(q.next())
    {
        nodeIds.append(q.value(0).toUInt());
    }
    return nodeIds;

}

QList<unsigned int> PCx_TreeModel::getNonLeavesId() const
{
    QList<unsigned int> nonleaves;
    QList<unsigned int> nodes;
    nodes=getNodesId();
    foreach (unsigned int node, nodes) {
        if(!isLeaf(node))
        {
            nonleaves.append(node);
        }
    }
    qDebug()<<"Non-leaves for tree "<<treeId<< " = "<<nonleaves;
    return nonleaves;
}

QSet<unsigned int> PCx_TreeModel::getNodesWithSharedName() const
{
    QSqlQuery q(QString("select * from arbre_%1").arg(treeId));
    QSet<unsigned int> nodes;
    QHash<QString,QPair<unsigned int,unsigned int> > nameToTypeAndId;
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        return QSet<unsigned int>();
    }
    while(q.next())
    {
        QString name=q.value("nom").toString().toLower();
        if(nameToTypeAndId.contains(name))
        {
            nodes.insert(q.value("id").toUInt());
            nodes.insert(nameToTypeAndId.value(name).second);
        }
        else
        {
            QPair<unsigned int,unsigned int>typeAndId;
            typeAndId.first=q.value("type").toUInt();
            typeAndId.second=q.value("id").toUInt();
            nameToTypeAndId.insert(name,typeAndId);
        }
    }
    return nodes;
}

bool PCx_TreeModel::isLeaf(unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where pid=:nodeid").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    q.exec();
    if(q.next())
    {
        if(q.value(0).toInt()==0)
            return true;
        else return false;
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }
    return false;
}

unsigned int PCx_TreeModel::getTreeDepth() const
{
    unsigned int maxDepth=0;
    QList<unsigned int> nodes=getNodesId();
    unsigned int depth,pid,curNode;
    foreach(unsigned int node, nodes)
    {
        if(node==1)
            continue;
        depth=0;

        curNode=node;
        do
        {
            pid=getParentId(curNode);
            depth++;
            curNode=pid;
        }while(pid!=1);

        if(depth>maxDepth)
            maxDepth=depth;
    }
    return maxDepth;
}

QModelIndexList PCx_TreeModel::getIndexesOfNodesWithThisType(unsigned int typeId) const
{
    return match(index(0,0),Qt::UserRole+2,QVariant(typeId),-1,Qt::MatchRecursive);
}

QList<unsigned int> PCx_TreeModel::getIdsOfNodesWithThisType(unsigned int typeId) const
{
    Q_ASSERT(typeId>0);
    QSqlQuery q;
    QList<unsigned int> nodes;
    q.prepare(QString("select id from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return QList<unsigned int>();
    }
    while(q.next())
    {
        nodes.append(q.value(0).toUInt());
    }
    return nodes;
}

unsigned int PCx_TreeModel::getNumberOfNodesWithThisType(unsigned int typeId) const
{
    Q_ASSERT(typeId>0);
    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
        return 0;
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_TreeModel::sortNodesBFS(QList<unsigned int> &nodes) const
{
    QList<unsigned int>sortedNodes;

    QList<unsigned int>toCheck;
    toCheck.append(1);

    while(!toCheck.isEmpty())
    {
        unsigned int node=toCheck.takeFirst();
        if(nodes.contains(node))
            sortedNodes.append(node);
        toCheck.append(getChildren(node));
    }
    return sortedNodes;
}


QList<unsigned int> PCx_TreeModel::sortNodesDFS(QList<unsigned int> &nodes,unsigned int currentNode) const
{
    QList<unsigned int> sortedNodes;

    if(nodes.contains(currentNode))
        sortedNodes.append(currentNode);

    QList<unsigned int> children=getChildren(currentNode);
    foreach(unsigned int childId,children)
    {
        sortedNodes.append(sortNodesDFS(nodes,childId));
    }
    return sortedNodes;
}

unsigned int PCx_TreeModel::getParentId(unsigned int nodeId) const
{
    QSqlQuery q;
    q.prepare(QString("select pid from arbre_%1 where id=:nodeid").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    q.exec();
    if(q.next())
    {
        return q.value(0).toUInt();
    }
    else
    {
        qCritical()<<q.lastError();
    }
    return 0;
}

QList<unsigned int> PCx_TreeModel::getChildren(unsigned int nodeId) const
{
    QList<unsigned int> listOfChildren;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:nodeid order by nom").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    q.exec();

    while(q.next())
    {
        listOfChildren.append(q.value(0).toUInt());
    }
    return listOfChildren;
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
        unsigned int dragId=tmpModel.item(i)->data(Qt::UserRole+1).toUInt();
        if(dragId!=1)
            updateNodePosition(dragId,dropId);
    }

    return QStandardItemModel::dropMimeData(data,action,row,column,parent);
}

void PCx_TreeModel::updateNodePosition(unsigned int nodeId, unsigned int newPid)
{
    Q_ASSERT(nodeId>1 && newPid > 0);
    QSqlQuery q;

    q.prepare(QString("update arbre_%1 set pid=:pid where id=:id").arg(treeId));
    q.bindValue(":pid",newPid);
    q.bindValue(":id",nodeId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
}

bool PCx_TreeModel::finishTree()
{
    Q_ASSERT(treeId>0);
    if(this->finished==false)
    {
        QSqlQuery q;
        q.prepare("update index_arbres set termine=1 where id=:id");
        q.bindValue(":id",treeId);
        q.exec();
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            return false;
        }
        this->finished=true;
    }
    return true;
}


bool PCx_TreeModel::updateTree()
{
    if(noLoadModel==true)
        return true;
    QElapsedTimer timer;
    timer.start();
    bool res;
    this->clear();
    res=createChildrenItems(invisibleRootItem(),0);
    qDebug()<<"updateTree done in"<<timer.elapsed()<<"ms";
    return res;
}

int PCx_TreeModel::getNodeIdFromTypeAndNodeName(const QPair<QString, QString> &typeAndNodeName) const
{
    int typeId=types->nameToIdType(typeAndNodeName.first);
    if(typeId==-1)
        return -1;
    QSqlQuery q;
    QString nodeNameSpl=typeAndNodeName.second.simplified();
    q.prepare(QString("select id from arbre_%1 where nom=:nameId and type=:typeId").arg(treeId));
    q.bindValue(":nameId",nodeNameSpl);
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return -1;
    }
    if(!q.next())
    {
        return -1;
    }
    return q.value(0).toInt();
}


QPair<QString,QString> PCx_TreeModel::getTypeNameAndNodeName(unsigned int node) const
{
    Q_ASSERT(node>0);
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(":id",node);
    q.exec();

    QPair<QString,QString> typeNameAndNodeName;

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=types->idTypeToName(q.value("type").toUInt());
            typeNameAndNodeName.first=typeName;
            typeNameAndNodeName.second=q.value("nom").toString();
        }
        //Root does not has type
        else
        {
            typeNameAndNodeName.first=QString();
            typeNameAndNodeName.second=q.value("nom").toString();
        }
        return typeNameAndNodeName;
    }
    else
    {
        qWarning()<<"Inexistant node"<<node;
    }
    return QPair<QString,QString>();

}


QString PCx_TreeModel::getNodeName(unsigned int node) const
{
    Q_ASSERT(node>0);
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(":id",node);
    q.exec();

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=types->idTypeToName(q.value("type").toUInt());
            return QString("%1 %2").arg(typeName).arg(q.value("nom").toString());
        }
        //Root does not has type
        else
        {
            return q.value("nom").toString();
        }
    }
    else
    {
        qWarning()<<"Inexistant node"<<node;
    }
    return QString();
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
        QStandardItem *newitem=createItem(types->idTypeToName(typeId),query.value(1).toString(),typeId,nodeId);

        item->appendRow(newitem);
        createChildrenItems(newitem,nodeId);
    }
    return true;

}

bool PCx_TreeModel::deleteNodeAndChildren(unsigned int nodeId)
{
    Q_ASSERT(nodeId>0);
    QList<unsigned int> listOfChildrens;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:pid").arg(treeId));
    q.bindValue(":pid",nodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        return false;
    }
    while(q.next())
    {
        listOfChildrens.append(q.value(0).toUInt());
    }

    if(listOfChildrens.size()==0)
    {
        q.prepare(QString("delete from arbre_%1 where id=:id").arg(treeId));
        q.bindValue(":id",nodeId);
        q.exec();

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }
        return true;
    }
    else
    {
        foreach(unsigned int child,listOfChildrens)
        {
            deleteNodeAndChildren(child);
        }
        deleteNodeAndChildren(nodeId);
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

QString PCx_TreeModel::toDot() const
{
    QList<unsigned int> nodes=getNodesId();

    QString out="graph g{\nrankdir=LR;\n";

    foreach(unsigned int node,nodes)
    {
        //Escape double quotes for dot format
        out.append(QString("%1 [label=\"%2\"];\n").arg(node).arg(getNodeName(node).replace('"',QString("\\\""))));
    }

    foreach(unsigned int node,nodes)
    {
        unsigned int pid=getParentId(node);

        if(pid!=0)
        {
            out.append(QString("\t%1--%2;\n").arg(pid).arg(node));
        }
    }
    out.append("}\n");
    return out;
}

bool PCx_TreeModel::toXLSX(const QString &fileName) const
{
    Q_ASSERT(!fileName.isEmpty());
    QList<unsigned int> nodes=getNodesId();

    QXlsx::Document xlsx;

    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Type père");
    xlsx.write(1,4,"Nom père");

    QPair<QString,QString>typeNameAndNodeName,pidTypeNameAndPidNodeName;

    int row=2;
    foreach(unsigned int node,nodes)
    {
        unsigned int pid=getParentId(node);
        typeNameAndNodeName=getTypeNameAndNodeName(node);



        if(pid>1)
        {
            xlsx.write(row,1,typeNameAndNodeName.first);
            xlsx.write(row,2,typeNameAndNodeName.second);
            pidTypeNameAndPidNodeName=getTypeNameAndNodeName(pid);
            xlsx.write(row,3,pidTypeNameAndPidNodeName.first);
            xlsx.write(row,4,pidTypeNameAndPidNodeName.second);
            row++;
        }
        else if(pid==1)
        {
            xlsx.write(row,1,typeNameAndNodeName.first);
            xlsx.write(row,2,typeNameAndNodeName.second);
            row++;
        }

    }
    return xlsx.saveAs(fileName);
}

bool PCx_TreeModel::loadFromDatabase(unsigned int treeId)
{
    Q_ASSERT(treeId>0);

    QSqlQuery query;

    query.prepare("SELECT id,nom,termine,le_timestamp from index_arbres where id=:id");
    query.bindValue(":id",treeId);
    query.exec();
    if(query.next())
    {
        this->treeName=query.value(1).toString();
        this->treeId=treeId;
        this->finished=query.value(2).toBool();
        this->creationTime=query.value(3).toString();
    }
    else
    {
        qWarning()<<"Missing Tree";
        return false;
    }
    return updateTree();
}
