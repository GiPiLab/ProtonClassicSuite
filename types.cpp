#include "types.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlTableModel>
#include <QObject>

Types::Types(QObject *parent):QObject(parent)
{
    initialized=false;
    typesTableModel=NULL;
}

Types::Types(int treeId,bool loadSqlTableModel)
{
    initialized=false;
    this->treeId=treeId;
    loadTypesFromTree(treeId);
    if(loadSqlTableModel==true)
    {
        qDebug()<<"Load sql model";
        this->loadSqlTableModel();
        qDebug()<<typesTableModel->columnCount();
    }
}

bool Types::loadSqlTableModel()
{
    if(initialized==false)return false;

    typesTableModel=new QSqlTableModel();
    typesTableModel->setTable(QString("types_%1").arg(treeId));
    typesTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    typesTableModel->select();

    connect(typesTableModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onTypesModelDataChanged()));

    return true;
}

void Types::onTypesModelDataChanged()
{
    qDebug()<<"Data changed";
    loadTypesFromTree(treeId);
}

Types::~Types()
{
    if(typesTableModel!=NULL)
    {
        typesTableModel->clear();
        delete typesTableModel;
    }
}

bool Types::loadTypesFromTree(int treeId)
{
    if(treeId<=0)return false;
    idTypesToNom.clear();
    QSqlQuery query(QString("select * from types_%1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError();
        return false;
    }

    while(query.next())
    {
        idTypesToNom.insert(query.value(0).toInt(),query.value(1).toString());
    }
    initialized=true;
    return true;
}
