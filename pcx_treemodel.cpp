#include "pcx_treemodel.h"
#include "utility.h"
#include "pcx_typemodel.h"
#include <QtGui>
#include <QtSql>
#include <QMessageBox>


PCx_TreeModel::PCx_TreeModel(unsigned int treeId,QObject *parent):QStandardItemModel(parent)
{
    types=new PCx_TypeModel(treeId);
    loadFromDatabase(treeId);
}

PCx_TreeModel::~PCx_TreeModel()
{
    this->clear();
    delete types;
    types=NULL;
}

QDateTime PCx_TreeModel::getCreationTime()
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
}

int PCx_TreeModel::addNode(int pid, int type, const QString &name, QModelIndex & pidNodeIndex)
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
        QStandardItem *newitem=createItem(types->getNomType(type),name,type,q.lastInsertId().toInt());
        pidItem->appendRow(newitem);
    }
    return q.lastInsertId().toInt();
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, int newType)
{
    Q_ASSERT(nodeIndex.isValid() && !newName.isNull()&& !newName.isEmpty() && newType>0);
    int nodeId=nodeIndex.data(Qt::UserRole+1).toInt();
    qDebug()<<"Node ID = "<<nodeId;

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
        qDebug()<<query.value(0).toInt();
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

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        die();
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Problème d'id, vérifiez la consistance de la base";
        die();
    }

    qDebug()<<"Last inserted id = "<<lastId.toInt();

    //Arbre
    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        die();
    }

    //Types associés
    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        //Penser à supprimer les tables d'avant en cas d'échec pour éviter les orphelines

        qCritical()<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
        die();
    }

    QStringList listOfTypes=PCx_TypeModel::getListOfDefaultTypes();
    foreach(QString oneType,listOfTypes)
    {
        query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(lastId.toInt()));
        query.bindValue(":nomtype",oneType);
        query.exec();
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError().text();
            query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
            query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
            query.exec(QString("drop table types_%1").arg(lastId.toInt()));
            die();
        }
    }

    //Racine
    query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toInt()));
    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
        query.exec(QString("drop table types_%1").arg(lastId.toInt()));
        die();
    }
    return true;
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
        qDebug()<<"Arbre inconnu";
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
        qDebug()<<"Hors racine";
        return false;
    }


    int dragId,dropId;

    dropId=parent.data(Qt::UserRole+1).toInt();

    qDebug()<<"DROP ID = "<<dropId;

    QByteArray encodedData=data->data("application/x-qstandarditemmodeldatalist");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    int arow,acol;
    QMap<int, QVariant> roleDataMap;
    stream >> arow>>acol>>roleDataMap;

    dragId=roleDataMap[Qt::UserRole+1].toInt();
    qDebug()<<"DRAG ID = "<<dragId;
    if(dragId==1)
    {
        qDebug()<<"NOT DRAGGING THE ROOT";
        return false;
    }

    updateNodePosition(dragId,dropId);

    return QStandardItemModel::dropMimeData(data,action,row,column,parent);
}

void PCx_TreeModel::updateNodePosition(int nodeId, int newPid)
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


bool PCx_TreeModel::updateTree()
{
    this->clear();
    root=this->invisibleRootItem();
    createChildrenItems(root,0);
    return true;
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
        QStandardItem *newitem=createItem(types->getNomType(query.value(3).toInt()),query.value(1).toString(),query.value(3).toInt(),query.value(0).toInt());

        item->appendRow(newitem);
        createChildrenItems(newitem,query.value(0).toInt());
    }
    return true;

}

QStandardItem *PCx_TreeModel::createItem(const QString &typeName, const QString &nodeName, int typeId, int nodeId)
{
    QStandardItem *newitem=new QStandardItem(QString("%1 %2").arg(typeName).arg(nodeName));
    newitem->setData(nodeId,Qt::UserRole+1);
    newitem->setData(typeId,Qt::UserRole+2);
    return newitem;
}

