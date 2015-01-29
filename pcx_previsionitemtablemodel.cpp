#include "pcx_previsionitemtablemodel.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_PrevisionItemTableModel::PCx_PrevisionItemTableModel(PCx_Prevision *prevision,MODES::DFRFDIRI mode,unsigned int nodeId, unsigned int year, QObject *parent) :
    QAbstractTableModel(parent),prevision(prevision),mode(mode),nodeId(nodeId),year(year)
{
    QSqlQuery q;

    q.prepare(QString("select * from prevision_%1_%2 where id_node=:id_node and year=:year").arg(MODES::modeToTableString(mode))
              .arg(prevision->getPrevisionId()));
    q.bindValue(":id_node",nodeId);
    q.bindValue(":year",year);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        label=q.value("label").toString();
        QStringList items=q.value("prevision_operators_to_add").toString().split(';',QString::SkipEmptyParts);
        foreach(const QString &item,items)
        {
            PCx_PrevisionItemCriteria criteria(item);
            itemsToAdd.append(criteria);
        }

        items.clear();
        items=q.value("prevision_operators_to_substract").toString().split(';',QString::SkipEmptyParts);


        foreach(const QString &item,items)
        {
            PCx_PrevisionItemCriteria criteria(item);
            itemsToSubstract.append(criteria);
        }
    }
}

PCx_PrevisionItemTableModel::~PCx_PrevisionItemTableModel()
{

}

QString PCx_PrevisionItemTableModel::getLongDescription() const
{
    QString out;
    QStringList listItems;
    if(itemsToAdd.size()==0 && itemsToSubstract.size()==0)
        return QObject::tr("aucune prévision");
    foreach(const PCx_PrevisionItemCriteria &criteria,itemsToAdd)
    {
        listItems.append(criteria.getCriteriaLongDescription());
    }
    out.append(listItems.join('+'));

    listItems.clear();

    foreach(const PCx_PrevisionItemCriteria &criteria,itemsToSubstract)
    {
        listItems.append(criteria.getCriteriaLongDescription());
    }
    if(listItems.size()>0)
    {
        out.append('-');
        out.append(listItems.join('-'));
    }
    return out;
}

qint64 PCx_PrevisionItemTableModel::computeTotal() const
{
    qint64 total=0;

    foreach(const PCx_PrevisionItemCriteria &criteria, itemsToAdd)
    {
        total+=criteria.compute(prevision->getAttachedAuditId(),mode,nodeId);
    }
    foreach(const PCx_PrevisionItemCriteria &criteria, itemsToSubstract)
    {
        total-=criteria.compute(prevision->getAttachedAuditId(),mode,nodeId);
    }
    return total;
}

void PCx_PrevisionItemTableModel::insertCriteriaToAdd(PCx_PrevisionItemCriteria criteria)
{
    beginResetModel();
    itemsToAdd.append(criteria);
    saveDataToDb();
    endResetModel();
}

void PCx_PrevisionItemTableModel::insertCriteriaToSub(PCx_PrevisionItemCriteria criteria)
{
    beginResetModel();
    itemsToSubstract.append(criteria);
    saveDataToDb();
    endResetModel();
}

bool PCx_PrevisionItemTableModel::deleteCriteria(QModelIndexList selectedIndexes)
{
    //TODO : only single selection supported
    if(selectedIndexes.isEmpty())
        return true;
    if(selectedIndexes.size()>1)
    {
        qDebug()<<"Multiselect not implemented";
    }
    const QModelIndex &index=selectedIndexes[0];

    beginResetModel();
    if(index.row()<itemsToAdd.size())
    {
        itemsToAdd.removeAt(index.row());
    }
    else
    {
        itemsToSubstract.removeAt(index.row()-itemsToAdd.size());
    }
    saveDataToDb();
    endResetModel();

    return true;
}

void PCx_PrevisionItemTableModel::deleteAllCriteria()
{
    beginResetModel();
    itemsToAdd.clear();
    itemsToSubstract.clear();
    saveDataToDb();
    endResetModel();
}

int PCx_PrevisionItemTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}


int PCx_PrevisionItemTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return itemsToAdd.size()+itemsToSubstract.size();
}

QVariant PCx_PrevisionItemTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole)
    {
        const PCx_PrevisionItemCriteria *trueItem=nullptr;
        if(index.row()<itemsToAdd.count())
            trueItem=&itemsToAdd.at(index.row());
        else
            trueItem=&itemsToSubstract.at(index.row()-itemsToAdd.count());


        if(index.column()==0)
        {
            return trueItem->getCriteriaLongDescription();
        }
        else if(index.column()==1)
        {
            return NUMBERSFORMAT::formatFixedPoint(trueItem->compute(prevision->getAttachedAuditId(),mode,nodeId));
        }
    }
    else if(role==Qt::BackgroundRole)
    {
        if(index.row()<itemsToAdd.count())
            return QVariant(QBrush(QColor(Qt::green)));
        else
            return QVariant(QBrush(QColor(Qt::red)));


    }
    else if(role==Qt::TextAlignmentRole)
    {
        if(index.column()==1)
            return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant PCx_PrevisionItemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole)
    {
        if(orientation==Qt::Horizontal)
        {
            if(section==0)
                return QObject::tr("Critère");
            if(section==1)
                return QObject::tr("Valeur");
        }

    }
    return QVariant();
}

void PCx_PrevisionItemTableModel::saveDataToDb() const
{
    QSqlQuery q;
    QStringList items;
    foreach(const PCx_PrevisionItemCriteria &criteria, itemsToAdd)
    {
        items.append(criteria.serialize());
    }
    QString addString=items.join(';');
    items.clear();
    foreach(const PCx_PrevisionItemCriteria &criteria, itemsToSubstract)
    {
        items.append(criteria.serialize());
    }
    QString subString=items.join(';');


    q.prepare(QString("insert into prevision_%1_%2 (id_node,year,prevision_operators_to_add, prevision_operators_to_substract) values (:idnode,:year,:prevopadd,:prevopsub)")
              .arg(MODES::modeToTableString(mode))
              .arg(prevision->getPrevisionId()));
    q.bindValue(":idnode",nodeId);
    q.bindValue(":year",year);
    q.bindValue(":prevopadd",addString);
    q.bindValue(":prevopsub",subString);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

}

