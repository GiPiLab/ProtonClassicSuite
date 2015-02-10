#include "pcx_previsionitemtablemodel.h"

PCx_PrevisionItemTableModel::PCx_PrevisionItemTableModel(PCx_PrevisionItem *previsionItem, QObject *parent) :
    QAbstractTableModel(parent),previsionItem(previsionItem)
{    
}


PCx_PrevisionItemTableModel::~PCx_PrevisionItemTableModel()
{
}

void PCx_PrevisionItemTableModel::setPrevisionItem(PCx_PrevisionItem *previsionItem)
{
    beginResetModel();
    this->previsionItem=previsionItem;
    endResetModel();
}

void PCx_PrevisionItemTableModel::resetModel()
{
    beginResetModel();
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
    return previsionItem->getItemsToAdd().size()+previsionItem->getItemsToSubstract().size();
}

QVariant PCx_PrevisionItemTableModel::data(const QModelIndex &index, int role) const
{
//FIXME : Too much refresh
    if(!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole)
    {
        const PCx_PrevisionItemCriteria *trueItem=nullptr;
        if(index.row()<previsionItem->getItemsToAdd().count())
            trueItem=&previsionItem->getItemsToAdd().at(index.row());
        else
            trueItem=&previsionItem->getItemsToSubstract().at(index.row()-previsionItem->getItemsToAdd().count());


        if(index.column()==0)
        {
            return trueItem->getCriteriaLongDescription();
        }
        else if(index.column()==1)
        {
            return NUMBERSFORMAT::formatFixedPoint(trueItem->compute(previsionItem->getPrevision()->getAttachedAuditId(),
                                                                     previsionItem->getMode(),previsionItem->getNodeId()));
        }
    }
    else if(role==Qt::BackgroundRole)
    {
        if(index.row()<previsionItem->getItemsToAdd().count())
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
