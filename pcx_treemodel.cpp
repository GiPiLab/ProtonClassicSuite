#include "pcx_treemodel.h"
#include "utility.h"
#include "types.h"
#include <QDateTime>
#include <QStandardItem>
#include <QString>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHash>

PCx_TreeModel::PCx_TreeModel()
{
    finished=false;
    model=new QStandardItemModel();
    treeId=0;
    root=model->invisibleRootItem();
    types=NULL;
}

PCx_TreeModel::~PCx_TreeModel()
{
    model->clear();
    delete model;
    model=NULL;
    delete types;
    types=NULL;
}

QDateTime PCx_TreeModel::getCreationTime()
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
}

int PCx_TreeModel::addChild(int pid, int type, const QString &name, QModelIndex & pidNodeIndex)
{
    if(name.isNull() || name.isEmpty()||pid<=0 ||type<=0)
    {
        qDebug()<<"addChild données invalides";
        return -1;
    }

    QSqlQuery q;
    q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":pid",pid);
    q.bindValue(":type",type);
    q.exec();

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<q.lastError().text();
        die();
    }

    //Also add an item to the model
    if(pidNodeIndex.isValid())
    {
        QStandardItem *pidItem=model->itemFromIndex(pidNodeIndex);
        QStandardItem *newitem=createItem(types->getNomType(type),name,type,q.lastInsertId().toInt());
        pidItem->appendRow(newitem);
    }

    return q.lastInsertId().toInt();
}

bool PCx_TreeModel::updateNode(const QModelIndex &nodeIndex, const QString &newName, int newType)
{
    Q_ASSERT(nodeIndex.isValid() && !newName.isNull()&& !newName.isEmpty() && newType>0);



    return true;

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
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<"Problème d'id, vérifiez la consistance de la base";
        die();
    }

    qDebug()<<Q_FUNC_INFO<<__LINE__<<"Last inserted id = "<<lastId.toInt();

    //Arbre
    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        die();
    }

    //Types associés
    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        //Penser à supprimer les tables d'avant en cas d'échec pour éviter les orphelines

        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
        die();
    }

    QStringList listOfTypes=Types::getListOfDefaultTypes();
    foreach(QString oneType,listOfTypes)
    {
        query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(lastId.toInt()));
        query.bindValue(":nomtype",oneType);
        query.exec();
        if(query.numRowsAffected()==-1)
        {
            qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
            query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
            query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
            query.exec(QString("drop table types_%1").arg(lastId.toInt()));
            die();
        }
    }

    //Racine
    query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toInt()));
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        query.exec(QString("delete from index_arbres where id=%1").arg(lastId.toInt()));
        query.exec(QString("drop table arbre_%1").arg(lastId.toInt()));
        query.exec(QString("drop table types_%1").arg(lastId.toInt()));
        die();
    }
    return true;
}

bool PCx_TreeModel::loadFromDatabase(int treeId)
{
    Q_ASSERT(treeId>0);

    QSqlQuery query;
    types=new Types(treeId,true);

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
        qDebug()<<Q_FUNC_INFO<<"Arbre inconnu dans "<<__LINE__;
        return false;
    }
    return updateTree();
}

bool PCx_TreeModel::updateTree()
{
    model->clear();
    root=model->invisibleRootItem();
    QSqlQuery query;
    query.exec(QString("select * from arbre_%1 order by id limit 1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError();
        return false;
    }

    if(query.next())
    {
        QStandardItem *trueRoot=createItem(types->getNomType(query.value(3).toInt()),query.value(1).toString(),query.value(3).toInt(),query.value(0).toInt());

        root->appendRow(trueRoot);
        createChildrenItems(trueRoot,query.value(0).toInt());
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<__LINE__<<"Empty tree";
        return false;
    }
    return true;
}

bool PCx_TreeModel::createChildrenItems(QStandardItem *item,unsigned int nodeId)
{
    Q_ASSERT(nodeId>0);

    QSqlQuery query(QString("select * from arbre_%1 where pid=%2 order by nom").arg(treeId).arg(nodeId));
    if(!query.isActive())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError();
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




