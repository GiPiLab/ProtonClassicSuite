#include "pcx_treemodel.h"
#include "utils.h"
#include "pcx_typemodel.h"
#include <QtGui>
#include <QtSql>
#include <QMessageBox>


PCx_TreeModel::PCx_TreeModel(unsigned int treeId, bool typesReadOnly, QObject *parent):QStandardItemModel(parent)
{
    types=new PCx_TypeModel(treeId,typesReadOnly);
    loadFromDatabase(treeId);
}

PCx_TreeModel::~PCx_TreeModel()
{
    this->clear();
    delete types;
    types=NULL;
}

QDateTime PCx_TreeModel::getCreationTime() const
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
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

unsigned int PCx_TreeModel::addNode(unsigned int pid, unsigned int type, const QString &name, const QModelIndex &pidNodeIndex)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && pid>0 && type>0);

    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":type",type);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<q.lastError().text();
        die();
    }

    q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":pid",pid);
    q.bindValue(":type",type);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError().text();
        die();
    }

    //Also add an item to the model
    if(pidNodeIndex.isValid())
    {
        QStandardItem *pidItem=this->itemFromIndex(pidNodeIndex);
        QStandardItem *newitem=createItem(types->getNomType(type),name,type,q.lastInsertId().toUInt());
        pidItem->appendRow(newitem);
    }
    return q.lastInsertId().toUInt();
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, unsigned int newType)
{
    Q_ASSERT(nodeIndex.isValid() && !newName.isNull()&& !newName.isEmpty() && newType>0);
    unsigned int nodeId=nodeIndex.data(Qt::UserRole+1).toUInt();
    //qDebug()<<"Node ID = "<<nodeId;

    QSqlQuery q;

    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",newName);
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
        qCritical()<<q.lastError().text();
        die();
    }

    q.prepare(QString("update arbre_%1 set nom=:nom, type=:type where id=:id").arg(treeId));
    q.bindValue(":nom",newName);
    q.bindValue(":type",newType);
    q.bindValue(":id",nodeId);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError().text();
        die();
    }

    QStandardItem *item=this->itemFromIndex(nodeIndex);
    item->setText(QString("%1 %2").arg(types->getNomType(newType)).arg(newName));
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


bool PCx_TreeModel::addNewTree(const QString &name)
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
            return false;
        }
    }
    else
    {
        qCritical()<<query.lastError().text();
        die();
    }
    QSqlDatabase::database().transaction();
    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Problème d'id";
        QSqlDatabase::database().rollback();
        die();
    }

    //qDebug()<<"Last inserted id = "<<lastId.toUInt();

    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toUInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(lastId.toUInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    QStringList listOfTypes=PCx_TypeModel::getListOfDefaultTypes();
    foreach(QString oneType,listOfTypes)
    {
        query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(lastId.toUInt()));
        query.bindValue(":nomtype",oneType);
        query.exec();
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError().text();
            QSqlDatabase::database().rollback();
            die();
        }
    }

    //Racine
    query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toUInt()));
    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }
    QSqlDatabase::database().commit();
    //qDebug()<<"Tree "<<lastId.toUInt()<< " inserted";

    return true;
}

QList<unsigned int> PCx_TreeModel::getNodesId() const
{
    return PCx_TreeModel::getNodesId(this->treeId);
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
    //qDebug()<<"Non-leaves for tree "<<treeId<< " = "<<nonleaves;
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
        qCritical()<<q.lastError().text();
        die();
    }
    return false;
}

QModelIndexList PCx_TreeModel::getIndexesOfNodesWithThisType(unsigned int typeId) const
{
    return match(index(0,0),Qt::UserRole+2,QVariant(typeId),-1,Qt::MatchRecursive);
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
        qCritical()<<q.lastError().text();
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
            qCritical()<<"Arbre inexistant !";
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError().text();
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
        qCritical()<<query.lastError().text();
        die();
    }


    QSqlDatabase::database().transaction();
    query.exec(QString("delete from index_arbres where id='%1'").arg(treeId));
    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }


    query.exec(QString("drop table arbre_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    query.exec(QString("drop table types_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError().text();
        QSqlDatabase::database().rollback();
        die();
    }

    QSqlDatabase::database().commit();

    //qDebug()<<"Tree "<<treeId<<" deleted.";
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
        qCritical()<<"Missing tree";
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
        qCritical()<<"Bad Tree";
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
       // qDebug()<<"Not root";
        return false;
    }

    unsigned int dragId,dropId;

    dropId=parent.data(Qt::UserRole+1).toUInt();

    //qDebug()<<"DROP ID = "<<dropId;

    QByteArray encodedData=data->data("application/x-qstandarditemmodeldatalist");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    int arow,acol;
    QMap<int, QVariant> roleDataMap;
    stream >> arow>>acol>>roleDataMap;

    dragId=roleDataMap[Qt::UserRole+1].toUInt();
    //qDebug()<<"DRAG ID = "<<dragId;
    if(dragId==1)
    {
       // qDebug()<<"NOT DRAGGING THE ROOT";
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
        qCritical()<<q.lastError().text();
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
            qCritical()<<q.lastError().text();
            return false;
        }
        this->finished=true;
    }
    return true;
}


bool PCx_TreeModel::updateTree()
{
    this->clear();
    createChildrenItems(invisibleRootItem(),0);
    return true;
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
        qCritical()<<"Bad node id";
    }
    return QString();
}

bool PCx_TreeModel::createChildrenItems(QStandardItem *item,unsigned int nodeId)
{
    QSqlQuery query(QString("select * from arbre_%1 where pid=%2 order by nom").arg(treeId).arg(nodeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError().text();
        return false;
    }
    while(query.next())
    {
        QStandardItem *newitem=createItem(types->getNomType(query.value(3).toUInt()),query.value(1).toString(),query.value(3).toUInt(),query.value(0).toUInt());

        item->appendRow(newitem);
        createChildrenItems(newitem,query.value(0).toUInt());
//        qDebug()<<getNodeName(query.value(0).toUInt());
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
            qCritical()<<q.lastError().text();
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
    QStandardItem *newitem=new QStandardItem(QString("%1 %2").arg(typeName).arg(nodeName));
    newitem->setData(nodeId,Qt::UserRole+1);
    newitem->setData(typeId,Qt::UserRole+2);
//    newitem->setIcon(QIcon::fromTheme());
    return newitem;
}

