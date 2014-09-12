#include "pcx_typemodel.h"
#include "utils.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

PCx_TypeModel::PCx_TypeModel(unsigned int treeId, bool readOnly, QObject *parent):QObject(parent)
{
    this->treeId=treeId;
    loadFromDatabase(treeId);
    typesTableModel=NULL;
    typesQueryModel=NULL;

    if(readOnly==false)
        loadSqlTableModel();
    else
        loadSqlQueryModel();
}

bool PCx_TypeModel::loadSqlTableModel()
{
    Q_ASSERT(treeId>0);
    typesTableModel=new QSqlTableModel();
    typesTableModel->setTable(QString("types_%1").arg(treeId));
    typesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    typesTableModel->select();
    connect(typesTableModel,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(onTypesModelDataChanged(const QModelIndex &, const QModelIndex &)));
    return true;
}

bool PCx_TypeModel::loadSqlQueryModel()
{
    Q_ASSERT(treeId>0);
    typesQueryModel=new QSqlQueryModel();
    typesQueryModel->setQuery(QString("select * from types_%1").arg(treeId));
    return true;
}

QStringList PCx_TypeModel::getListOfDefaultTypes()
{
    QStringList listOfTypes;
    listOfTypes<<tr("Maire adjoint")<<tr("Conseiller")<<tr("Division")<<tr("Service");
    return listOfTypes;
}

//Please use transactions in caller
bool PCx_TypeModel::createTableTypes(unsigned int treeId, bool populateWithDefaultTypes)
{
    Q_ASSERT(treeId>0);
    QSqlQuery query;

    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(treeId));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        return false;
    }

    if(populateWithDefaultTypes==true)
    {
        QStringList listOfTypes=getListOfDefaultTypes();
        foreach(QString oneType,listOfTypes)
        {
            query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(treeId));
            query.bindValue(":nomtype",oneType);
            query.exec();
            if(query.numRowsAffected()!=1)
            {
                qCritical()<<query.lastError();
                return false;
            }
        }
    }
    return true;
}

/*Check if the new type does not already exists in the table
 * Assumes that only one row is modified at once
 */
bool PCx_TypeModel::onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight)
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
        loadFromDatabase(treeId);
        emit typesUpdated();
        return true;
    }
}

bool PCx_TypeModel::validateType(const QString &newType)
{
    qDebug()<<"Type to validate = "<<newType;
    if(newType.isEmpty())
    {
        QMessageBox::warning(NULL,tr("Attention"),tr("Vous ne pouvez pas utiliser un type vide !"));
        return false;
    }
    else if(listOfTypeNames.contains(newType))
    {
        QMessageBox::warning(NULL,tr("Attention"),tr("Le type <b>%1</b> existe déjà !").arg(newType));
        return false;
    }
    return true;
}

PCx_TypeModel::~PCx_TypeModel()
{
    if(typesTableModel!=NULL)
    {
        typesTableModel->clear();
        delete typesTableModel;
    }
    if(typesQueryModel!=NULL)
    {
        typesQueryModel->clear();
        delete typesQueryModel;
    }
}

int PCx_TypeModel::nameToIdType(const QString &typeName) const
{
    Q_ASSERT(!typeName.isNull() && !typeName.isEmpty());
    QString typeNameSpl=typeName.simplified();
    if(listOfTypeNames.contains(typeNameSpl))
        return idToName.key(typeNameSpl);
    else return -1;
}

QList<QPair<unsigned int, QString> > PCx_TypeModel::getAllTypes() const
{
    QList<QPair<unsigned int,QString> > types;
    QSqlQuery query(QString("select * from types_%1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QPair<unsigned int,QString> p;
        p.first=query.value(0).toUInt();
        p.second=query.value(1).toString();
        types.append(p);
    }
    return types;
}

unsigned int PCx_TypeModel::addType(const QString &typeName)
{
    unsigned int typeId=0;
    //Read-only mode
    if(typesTableModel==NULL)return 0;

    if(validateType(typeName)==false)
        return 0;

    else
    {
        QSqlQuery q;
        q.prepare(QString("insert into types_%1 (nom) values(:nom)").arg(treeId));
        q.bindValue(":nom",typeName);
        q.exec();
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }

        typeId=q.lastInsertId().toUInt();
        Q_ASSERT(typeId>0);

        idToName.insert(typeId,typeName);
        listOfTypeNames.append(typeName);
        typesTableModel->select();
    }
    return typeId;
}

bool PCx_TypeModel::deleteType(const QString &type)
{
    if(type.isNull() || type.isEmpty())return false;
    if(typesTableModel==NULL)return false;

    QSqlQuery query;
    unsigned int typeId=0;

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
        typeId=query.value(0).toUInt();
        qDebug()<<"typeId = "<<typeId<<"Text = "<<type;
    }
    else return false;

    return deleteType(typeId);
}

bool PCx_TypeModel::deleteType(unsigned int id)
{
    Q_ASSERT(id>0);
    if(typesTableModel==NULL)return false;
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
        qDebug()<<"Number of nodes of type "<<id<<" = "<<query.value(0).toInt();
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,tr("Attention"),tr("Il existe des noeuds de ce type dans l'arbre. Supprimez-les d'abord."));
            return false;
        }

    }
    else return false;

    query.prepare(QString("delete from types_%1 where id=:id").arg(treeId));
    query.bindValue(":id",id);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }
    loadFromDatabase(treeId);
    typesTableModel->select();
    return true;
}

bool PCx_TypeModel::loadFromDatabase(unsigned int treeId)
{
    Q_ASSERT(treeId>0);
    idToName.clear();
    listOfTypeNames.clear();
    QSqlQuery query(QString("select * from types_%1").arg(treeId));
    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        return false;
    }

    while(query.next())
    {
        idToName.insert(query.value(0).toUInt(),query.value(1).toString());
        listOfTypeNames.append(query.value(1).toString());
    }
    return true;
}
