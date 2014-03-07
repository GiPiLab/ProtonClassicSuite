#include "pcx_treemodel.h"
#include "utility.h"
#include <QDateTime>
#include <QStandardItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PCx_TreeModel::PCx_TreeModel()
{
    finished=false;
    model=new QStandardItemModel();
    typesModel=new QSqlTableModel();
    treeId=0;
    root=model->invisibleRootItem();
}

PCx_TreeModel::~PCx_TreeModel()
{
    model->clear();
    typesModel->clear();
}

QDateTime PCx_TreeModel::getCreationTime()
{
    //Assume sqlite CURRENT_TIMESTAMP format
    QDateTime dt(QDateTime::fromString(creationTime,"yyyy-MM-dd hh:mm:ss"));
    return dt;
}

bool PCx_TreeModel::saveToDatabase(void)
{
    return true;
}

bool PCx_TreeModel::loadFromDatabase(unsigned int treeId)
{
    Q_ASSERT(treeId>0);

    typesModel->setTable(QString("types_%1").arg(treeId));
    typesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    typesModel->select();

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
        qDebug()<<Q_FUNC_INFO<<"Arbre inconnu dans "<<__LINE__;
        return false;
    }

    //The root of the tree
    query.exec(QString("select * from arbre_%1 order by id limit 1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError();
        return false;
    }

    if(query.next())
    {
        QStandardItem *trueRoot=new
        QStandardItem(query.value(1).toString());
        root->appendRow(trueRoot);
        createChildren(trueRoot,query.value(0).toInt());
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<__LINE__<<"Empty tree";
        return false;
    }

    return true;
}

bool PCx_TreeModel::createChildren(QStandardItem *item,unsigned int nodeId)
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
        QStandardItem *newitem=new QStandardItem(query.value(1).toString());
        item->appendRow(newitem);
        createChildren(newitem,query.value(0).toInt());
    }
    return true;

}




