#ifndef PCX_PREVISIONITEMTABLEMODEL_H
#define PCX_PREVISIONITEMTABLEMODEL_H

#include <QAbstractListModel>
#include "pcx_previsionitemcriteria.h"
#include "pcx_prevision.h"

class PCx_PrevisionItemTableModel : public QAbstractTableModel
{
public:
    explicit PCx_PrevisionItemTableModel(PCx_Prevision *prevision,MODES::DFRFDIRI mode, unsigned int nodeId,unsigned int year,QObject *parent=0);

    ~PCx_PrevisionItemTableModel();


    PCx_Prevision *getPrevision()const{return prevision;}
    MODES::DFRFDIRI getMode()const{return mode;}
    unsigned int getNodeId()const{return nodeId;}
    unsigned int getYear()const{return year;}
    //FIXME label not used yet
    QString getLabel()const{return label;}
    QString getLongDescription()const;

    QString getNodeAndAttachedLeavesPrevisionItemHTMLTable() const;

    qint64 getComputedPrevisionItemValue()const;

    void insertCriteriaToAdd(PCx_PrevisionItemCriteria criteria);
    void insertCriteriaToSub(PCx_PrevisionItemCriteria criteria);
    bool deleteCriteria(QModelIndexList selectedIndexes);
    void deleteAllCriteria();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QString dataAsHTML() const;

    void saveDataToDb()const;
    void dispatchComputedValueToChildrenLeaves() const;


    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void dispatchCriteriaItemsToChildrenLeaves();
private:
    PCx_Prevision *prevision;
    MODES::DFRFDIRI mode;
    unsigned int nodeId;
    unsigned int year;
    QString label;
    qint64 computedValue;


    QList<PCx_PrevisionItemCriteria> itemsToAdd,itemsToSubstract;
    qint64 computePrevisionItemValue()const;
};

#endif // PCX_PREVISIONITEMTABLEMODEL_H
