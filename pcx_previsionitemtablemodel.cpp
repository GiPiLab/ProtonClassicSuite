#include "pcx_previsionitemtablemodel.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_PrevisionItemTableModel::PCx_PrevisionItemTableModel(PCx_Prevision *prevision,MODES::DFRFDIRI mode,unsigned int nodeId, unsigned int year, QObject *parent) :
    QAbstractTableModel(parent),prevision(prevision),mode(mode),nodeId(nodeId),year(year)
{
    QSqlQuery q;
    computedValue=0;

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
        computedValue=q.value("computedPrevision").toLongLong();
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

QString PCx_PrevisionItemTableModel::getNodeAndAttachedLeavesPrevisionItemHTMLTable() const
{
    QString output="<table border='0' width='100%' cellpadding='2'>";
    PCx_Tree tmpTree(prevision->getAttachedTreeId());


    if(!tmpTree.isLeaf(nodeId))
    {
        QList<unsigned int> childLeaves=tmpTree.getLeavesId(nodeId);
        foreach(unsigned int leaf,childLeaves)
        {
            PCx_PrevisionItemTableModel tmpTableModel(prevision,mode,leaf,year,0);
            output.append(QString("<tr><th colspan='2'>%1 : %2€</th></tr>").arg(tmpTree.getNodeName(leaf).toHtmlEscaped())
                          .arg(NUMBERSFORMAT::formatFixedPoint(tmpTableModel.getComputedPrevisionItemValue())));

            output.append(tmpTableModel.dataAsHTML());
        }
    }

    else
    {
        output.append(QString("<tr><th colspan='2'>%1 : %2€</th></tr>").arg(tmpTree.getNodeName(nodeId).toHtmlEscaped())
                      .arg(NUMBERSFORMAT::formatFixedPoint(getComputedPrevisionItemValue())));

        output.append(dataAsHTML());
    }
    output.append("</table>");
    return output;
}

qint64 PCx_PrevisionItemTableModel::getComputedPrevisionItemValue() const
{
    PCx_Tree tmpTree(prevision->getAttachedTreeId());
    qint64 total=0;
    if(tmpTree.isLeaf(nodeId))
    {
        total=computedValue;
    }
    else
    {
        if(itemsToAdd.isEmpty()&& itemsToSubstract.isEmpty())
        {
            QList<unsigned int> children=tmpTree.getLeavesId(nodeId);
            foreach(unsigned int child,children)
            {
                PCx_PrevisionItemTableModel tmpModel(prevision,mode,child,year,0);
                total+=tmpModel.getComputedPrevisionItemValue();
            }
        }
        else total=computedValue;
    }
    return total;
}




qint64 PCx_PrevisionItemTableModel::computePrevisionItemValue() const
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
    computedValue=computePrevisionItemValue();
    endResetModel();
}


void PCx_PrevisionItemTableModel::dispatchCriteriaItemsToChildrenLeaves()
{
    PCx_Tree tmpTree(prevision->getAttachedTreeId());

    QList<unsigned int> leavesId=tmpTree.getLeavesId(nodeId);

    //Dispatch items to children leaves

    foreach(unsigned int leaf,leavesId)
    {
        PCx_PrevisionItemTableModel tmpModel(prevision,mode,leaf,year,0);
        tmpModel.deleteAllCriteria();
        foreach(const PCx_PrevisionItemCriteria &criteria,itemsToAdd)
        {
            tmpModel.insertCriteriaToAdd(criteria);
        }
        foreach(const PCx_PrevisionItemCriteria &criteria,itemsToSubstract)
        {
            tmpModel.insertCriteriaToSub(criteria);
        }
        tmpModel.saveDataToDb();
    }

    //Remove current items
    deleteAllCriteria();
    saveDataToDb();

    //Remove items from ancestors
    QList<unsigned int> ancestors=tmpTree.getAncestorsId(nodeId);
    foreach(unsigned int ancestor,ancestors)
    {
        PCx_PrevisionItemTableModel tmpModel(prevision,mode,ancestor,year,0);
        tmpModel.deleteAllCriteria();
        tmpModel.saveDataToDb();
    }
}


void PCx_PrevisionItemTableModel::dispatchComputedValueToChildrenLeaves() const
{    
    PCx_Audit tmpAudit(prevision->getAttachedAuditId());

    QList<unsigned int> leavesId=tmpAudit.getAttachedTree()->getLeavesId(nodeId);


    unsigned int lastYear=tmpAudit.getYears().last();
    qint64 total=tmpAudit.getNodeValue(nodeId,mode,PCx_Audit::ORED::REALISES,lastYear);
    if(total==0)
    {
        qWarning()<<QObject::tr("Ne peut pas répartir sur la base d'un réalisé nul en %1 !").arg(lastYear);
        return;
    }


    if(itemsToAdd.isEmpty()&&itemsToSubstract.isEmpty())
    {
        foreach(unsigned int leaf,leavesId)
        {
            PCx_PrevisionItemTableModel tmpModel(prevision,mode,leaf,year,0);

            tmpModel.deleteAllCriteria();
            tmpModel.saveDataToDb();
        }

    }

    else
    {
        foreach(unsigned int leaf,leavesId)
        {
            PCx_PrevisionItemTableModel tmpModel(prevision,mode,leaf,year,0);

            qint64 val=tmpAudit.getNodeValue(leaf,mode,PCx_Audit::ORED::REALISES,lastYear);
            if(val!=-MAX_NUM)
            {
                double percent=(double)val/(double)total;
                qint64 newVal=computedValue*percent;
                PCx_PrevisionItemCriteria criteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUE,PCx_Audit::ORED::OUVERTS,newVal);
                tmpModel.deleteAllCriteria();
                tmpModel.insertCriteriaToAdd(criteria);
                tmpModel.saveDataToDb();
            }
            else
            {
                PCx_PrevisionItemCriteria criteria(PCx_PrevisionItemCriteria::PREVISIONOPERATOR::FIXEDVALUE,PCx_Audit::ORED::OUVERTS,0);
                tmpModel.deleteAllCriteria();
                tmpModel.insertCriteriaToAdd(criteria);
                tmpModel.saveDataToDb();
                qDebug()<<"Leaf"<<leaf<<tmpAudit.getAttachedTree()->getNodeName(leaf)<<"has no value";
            }
        }
    }
    //Remove items from ancestors
    QList<unsigned int> ancestors=tmpAudit.getAttachedTree()->getAncestorsId(nodeId);
    foreach(unsigned int ancestor,ancestors)
    {
        PCx_PrevisionItemTableModel tmpModel(prevision,mode,ancestor,year,0);
        tmpModel.deleteAllCriteria();
        tmpModel.saveDataToDb();
    }
}

void PCx_PrevisionItemTableModel::insertCriteriaToSub(PCx_PrevisionItemCriteria criteria)
{
    beginResetModel();
    itemsToSubstract.append(criteria);
    computedValue=computePrevisionItemValue();
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
    computedValue=computePrevisionItemValue();
    //  saveDataToDb();
    endResetModel();

    return true;
}

void PCx_PrevisionItemTableModel::deleteAllCriteria()
{
    beginResetModel();
    itemsToAdd.clear();
    itemsToSubstract.clear();
    computedValue=computePrevisionItemValue();
    //  saveDataToDb();
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

QString PCx_PrevisionItemTableModel::dataAsHTML() const
{
    QString output;
    unsigned int auditId=prevision->getAttachedAuditId();

    foreach(const PCx_PrevisionItemCriteria &criteria,itemsToAdd)
    {
        output.append(QString("<tr style='background-color:lightgreen'><td>%1</td><td align='right'>%2</td></tr>")
                      .arg(criteria.getCriteriaLongDescription().toHtmlEscaped())
                      .arg(NUMBERSFORMAT::formatFixedPoint(criteria.compute(auditId,mode,nodeId))));
    }
    foreach(const PCx_PrevisionItemCriteria &criteria,itemsToSubstract)
    {
        output.append(QString("<tr style='background-color:red'><td>%1</td><td align='right'>%2</td></tr>")
                      .arg(criteria.getCriteriaLongDescription().toHtmlEscaped())
                      .arg(NUMBERSFORMAT::formatFixedPoint(criteria.compute(auditId,mode,nodeId))));
    }
    return output;

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

    q.prepare(QString("insert into prevision_%1_%2 (id_node,year,prevision_operators_to_add, prevision_operators_to_substract,computedPrevision) values (:idnode,:year,:prevopadd,:prevopsub,:computed)")
              .arg(MODES::modeToTableString(mode))
              .arg(prevision->getPrevisionId()));
    q.bindValue(":idnode",nodeId);
    q.bindValue(":year",year);
    q.bindValue(":prevopadd",addString);
    q.bindValue(":prevopsub",subString);
    q.bindValue(":computed",computedValue);
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

