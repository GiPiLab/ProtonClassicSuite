#include "types.h"
#include "utility.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
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
        this->loadSqlTableModel();
        qDebug()<<"Loaded types with "<<typesTableModel->rowCount()<<" rows";
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

QStringList Types::getListOfDefaultTypes()
{
    QStringList listOfTypes;
    listOfTypes<<tr("Maire adjoint")<<tr("Conseiller")<<tr("Division")<<tr("Service");
    return listOfTypes;

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

bool Types::addType(const QString &type)
{
    if(type.isEmpty() or type.isNull())return false;

    if(nomTypes.contains(type,Qt::CaseInsensitive))
    {
        QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un type portant ce nom !"));
        return false;
    }

    else
    {
        QSqlQuery q;
        q.prepare(QString("insert into types_%1 (nom) values(:nom)").arg(treeId));
        q.bindValue(":nom",type);
        q.exec();
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError().text();
            die();
        }

        loadTypesFromTree(treeId);
        typesTableModel->select();
    }
    return true;
}

bool Types::deleteType(const QString &type)
{
    if(type.isNull() || type.isEmpty())return false;
    QSqlQuery query;
    int typeId=-1;

    query.prepare(QString("select id from types_%1 where nom=:nom limit 1").arg(treeId));
    query.bindValue(":nom",type);
    query.exec();

    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        typeId=query.value(0).toInt();
        qDebug()<<"typeId = "<<typeId<<"Text = "<<type;
    }
    else return false;

    return deleteType(typeId);
}

bool Types::deleteType(int id)
{
    if(id<0)return false;
    QSqlQuery query;

    query.prepare(QString("select count(*) from arbre_%1 where type=:type").arg(treeId));
    query.bindValue(":type",id);
    query.exec();

    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        qDebug()<<"Nombre de noeuds de type "<<id<<" = "<<query.value(0).toInt();
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe des noeuds de ce type dans l'arbre. Supprimez-les d'abord."));
            return false;
        }

    }
    else return false;

    query.prepare(QString("delete from types_%1 where id=:id").arg(treeId));
    query.bindValue(":id",id);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError().text();
        die();
    }
    loadTypesFromTree(treeId);
    typesTableModel->select();
    return true;
}


bool Types::loadTypesFromTree(int treeId)
{
    if(treeId<=0)return false;
    idTypesToNom.clear();
    nomTypes.clear();
    QSqlQuery query(QString("select * from types_%1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        return false;
    }

    while(query.next())
    {
        idTypesToNom.insert(query.value(0).toInt(),query.value(1).toString());
        nomTypes.append(query.value(1).toString());
    }
    initialized=true;
    return true;
}
