#ifndef PCX_PREVISIONITEM_H
#define PCX_PREVISIONITEM_H

#include "pcx_prevision.h"
#include <QTextDocument>

class PCx_PrevisionItem
{
public:
    explicit PCx_PrevisionItem(PCx_Prevision *prevision,MODES::DFRFDIRI mode, unsigned int nodeId,unsigned int year);
    PCx_Prevision *getPrevision()const{return prevision;}
    MODES::DFRFDIRI getMode()const{return mode;}
    unsigned int getNodeId()const{return nodeId;}

    void setNodeId(unsigned int nodeId){this->nodeId=nodeId;}
    void setMode(MODES::DFRFDIRI mode){this->mode=mode;}
    void setYear(unsigned int year){this->year=year;}

    unsigned int getYear()const{return year;}
    //TODO label not used yet
    QString getLabel()const{return label;}
    QString getLongDescription()const;
    QList<PCx_PrevisionItemCriteria> getItemsToAdd()const{return itemsToAdd;}
    QList<PCx_PrevisionItemCriteria> getItemsToSubstract()const{return itemsToSubstract;}

    void insertCriteriaToAdd(PCx_PrevisionItemCriteria criteria,bool compute=true);
    void insertCriteriaToSub(PCx_PrevisionItemCriteria criteria,bool compute=true);
    bool deleteCriteria(QModelIndexList selectedIndexes,bool compute=true);
    void deleteAllCriteria();

    void loadFromDb();
    void saveDataToDb();


    void dispatchCriteriaItemsToChildrenLeaves();
    void dispatchComputedValueToChildrenLeaves();

    QString getPrevisionItemAsHTML() const;
    QString getPrevisionItemsOfDescendantsAsHTML() const;

    qint64 getSummedPrevisionItemValue()const;
    qint64 getPrevisionItemValue()const;

    ~PCx_PrevisionItem();

    QString displayPrevisionItemReportInQTextDocument(QTextDocument *document) const;

    bool savePrevisionItemReport(const QString &fileName, bool showDescendants) const;


private:
    PCx_Prevision *prevision;
    MODES::DFRFDIRI mode;
    unsigned int nodeId;
    unsigned int year;
    QString label;
    qint64 computedValue;


    QList<PCx_PrevisionItemCriteria> itemsToAdd,itemsToSubstract;

};

#endif // PCX_PREVISIONITEM_H
