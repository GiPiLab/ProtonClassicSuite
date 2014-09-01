#include "pcx_treemodel.h"
#include "utils.h"
#include "pcx_typemodel.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QUuid>
#include <QElapsedTimer>

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
    types=NULL;
}


QList<QPair<unsigned int, QString> > PCx_TreeModel::getListOfTrees(bool finishedOnly)
{
    QList<QPair<unsigned int,QString> > listOfTrees;
    QDateTime dt;

    QSqlQuery query("select * from index_arbres order by datetime(le_timestamp)");
    while(query.next())
    {
        QString item;

        dt=QDateTime::fromString(query.value(3).toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();

        if(query.value(2).toBool()==true)
        {
            item=QString("%1 - %2 (arbre terminé)").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            QPair<unsigned int, QString> p;
            p.first=query.value(0).toUInt();
            p.second=item;
            listOfTrees.append(p);
        }
        else if(finishedOnly==false)
        {
             item=QString("%1 - %2").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
             QPair<unsigned int, QString> p;
             p.first=query.value(0).toUInt();
             p.second=item;
             listOfTrees.append(p);
        }
    }
    return listOfTrees;
}

//Private static function
int PCx_TreeModel::addTree(const QString &name,bool createRoot)
{
    QSqlQuery query;
    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        return -1;
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Invalid last inserted Id";
        return -1;
    }

    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toUInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        return -1;
    }

    query.exec(QString("create index idx_arbre_%1_pid on arbre_%1(pid)").arg(lastId.toUInt()));

    //When used to duplicate a tree, createRoot=false then create an empty type table
    if(!PCx_TypeModel::createTableTypes(lastId.toUInt(),createRoot))
    {
        return -1;
    }

    if(createRoot)
    {
    //Racine
        query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toUInt()));
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            return -1;
        }
    }
    return lastId.toInt();
}

unsigned int PCx_TreeModel::addNode(unsigned int pid, unsigned int type, const QString &name, const QModelIndex &pidNodeIndex)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && pid>0 && type>0);

    if(getNumberOfNodes()>=MAXNODES)
    {
        QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Nombre maximum de noeuds atteint !"));
        return 0;
    }

    //NOTE : simplify string to remove newline or multiple spaces
    QString sName=name.simplified();
    if(sName.isEmpty())
    {
        QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Le noeud doit avoir un nom non vide"));
        return 0;
    }

    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",sName);
    q.bindValue(":type",type);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
            return 0;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
    q.bindValue(":nom",sName);
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
        QStandardItem *newitem=createItem(types->getNomType(type),sName,type,q.lastInsertId().toUInt());
        pidItem->appendRow(newitem);
    }
    return q.lastInsertId().toUInt();
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType)
{
    Q_ASSERT(nodeIndex.isValid() && !newName.isNull()&& !newName.isEmpty() && newType>0);
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();
    qDebug()<<"Node to be updated = "<<nodeId;

    QSqlQuery q;

    //NOTE : simplify string to remove newline or multiple spaces
    QString sName=newName.simplified();
    if(sName.isEmpty())
    {
        QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Le noeud doit avoir un nom non vide"));
        return false;
    }

    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",sName);
    q.bindValue(":type",newType);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
            return false;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    q.prepare(QString("update arbre_%1 set nom=:nom, type=:type where id=:id").arg(treeId));
    q.bindValue(":nom",sName);
    q.bindValue(":type",newType);
    q.bindValue(":id",nodeId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    QStandardItem *item=this->itemFromIndex(nodeIndex);
    item->setText(QString("%1 %2").arg(types->getNomType(newType)).arg(sName));
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


int PCx_TreeModel::addNewTree(const QString &name)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty());
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    query.exec();

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }
    QSqlDatabase::database().transaction();
    int lastId=addTree(name,true);

    if(lastId==-1)
    {
        QSqlDatabase::database().rollback();
        die();
    }
    QSqlDatabase::database().commit();
    return lastId;
}

QList<unsigned int> PCx_TreeModel::getNodesId() const
{
    return PCx_TreeModel::getNodesId(this->treeId);
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
    q.prepare(QString("select * from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodes.append(q.value("id").toUInt());
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
    Q_ASSERT(nodeId > 0);
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
        die();
    }
    return 0;
}

QList<unsigned int> PCx_TreeModel::getChildren(unsigned int nodeId) const
{
    QList<unsigned int> listOfChildren;
    QSqlQuery q;
    q.prepare(QString("select * from arbre_%1 where pid=:nodeid order by nom").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    q.exec();

    while(q.next())
    {
        listOfChildren.append(q.value("id").toUInt());
    }
    return listOfChildren;
}

//Returns 0 if the tree is linked to an audit, -1 for a non-existant tree (should not happens) and 1 on success
int PCx_TreeModel::deleteTree(unsigned int treeId)
{
    Q_ASSERT(treeId>0);

    QSqlQuery query;

    query.exec(QString("select count(*) from index_arbres where id='%1'").arg(treeId));
    if(query.next())
    {
        if(query.value(0).toInt()==0)
        {
            qWarning()<<"Trying to delete an inexistant tree !";
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }

    query.exec(QString("select count(*) from index_audits where id_arbre='%1'").arg(treeId));
    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
           return 0;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }


    QSqlDatabase::database().transaction();
    query.exec(QString("delete from index_arbres where id='%1'").arg(treeId));
    if(query.numRowsAffected()!=1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }


    query.exec(QString("drop table arbre_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }

    query.exec(QString("drop table types_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }

    QSqlDatabase::database().commit();

    qDebug()<<"Tree "<<treeId<<" deleted.";
    return 1;
}

QString PCx_TreeModel::idTreeToName(unsigned int treeId)
{
    Q_ASSERT(treeId>0);
    QSqlQuery q(QString("select * from index_arbres where id='%1'").arg(treeId));
    if(q.next())
    {
        return q.value("nom").toString();
    }
    else
    {
        qWarning()<<"Inexistant tree !";
        return NULL;
    }
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

/*
 * Drag and drop : uses QStandardItemModel to compute the logic with items
 *
 * - Check validity (no dd outside of the tree)
 * - get DB IDs of elements
 * - update DB
 * - let QStandardItemModel::dropMimeData do the job with items
 */
bool PCx_TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{

    if(!parent.isValid())
    {
        qDebug()<<"Dropping outside of the root";
        return false;
    }

    unsigned int dragId,dropId;

    dropId=parent.data(Qt::UserRole+1).toUInt();

    qDebug()<<"DROP ID = "<<dropId;

    QByteArray encodedData=data->data("application/x-qstandarditemmodeldatalist");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    int arow,acol;
    QMap<int, QVariant> roleDataMap;
    stream >> arow>>acol>>roleDataMap;

    dragId=roleDataMap[Qt::UserRole+1].toUInt();
    qDebug()<<"DRAG ID = "<<dragId;
    if(dragId==1)
    {
        qDebug()<<"DONT DRAG THE ROOT";
        return false;
    }

    updateNodePosition(dragId,dropId);

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

QString PCx_TreeModel::toDot() const
{
    QList<unsigned int> nodes=getNodesId();

    QString out="graph g{\nrankdir=LR;\n";

    foreach(unsigned int node,nodes)
    {
        out.append(QString("%1 [label=\"%2\"];\n").arg(node).arg(getNodeName(node)));
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


int PCx_TreeModel::createRandomTree(const QString &name,unsigned int nbNodes)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && nbNodes>0 && nbNodes<=MAXNODES);
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    query.exec();

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }
    QSqlDatabase::database().transaction();
    int lastId=addTree(name,true);

    if(lastId==-1)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    unsigned int maxNumType=PCx_TypeModel::getListOfDefaultTypes().size();

    for(unsigned int i=1;i<nbNodes;i++)
    {
        QSqlQuery q;
        unsigned int type=(qrand()%maxNumType)+1;
        unsigned int pid=(qrand()%i)+1;
        QString name=QUuid::createUuid().toString();
        q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(lastId));
        q.bindValue(":nom",name);
        q.bindValue(":pid",pid);
        q.bindValue(":type",type);
        q.exec();

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            QSqlDatabase::database().rollback();
            die();
        }
    }

    QSqlDatabase::database().commit();
    return lastId;
}


int PCx_TreeModel::duplicateTree(const QString &newName) const
{
    Q_ASSERT(!newName.isEmpty());
    QSqlQuery q;
    q.prepare("select count(*) from index_arbres where nom=:name");
    q.bindValue(":name",newName);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().transaction();
    int newTreeId=addTree(newName,false);
    if(newTreeId==-1)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    q.exec(QString("insert into types_%1 select * from types_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("insert into arbre_%1 select * from arbre_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return newTreeId;
}

QString PCx_TreeModel::getNodeName(unsigned int node) const
{
    Q_ASSERT(node>0);
    QSqlQuery q;
    q.prepare(QString("select * from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(":id",node);
    q.exec();

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=types->getNomType(q.value("type").toUInt());
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
        QStandardItem *newitem=createItem(types->getNomType(typeId),query.value(1).toString(),typeId,nodeId);

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
    q.prepare(QString("select * from arbre_%1 where pid=:pid").arg(treeId));
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

