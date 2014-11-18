#ifndef PCX_REPORTINGTABLEOVERVIEWMODEL_H
#define PCX_REPORTINGTABLEOVERVIEWMODEL_H

#include <QSqlQueryModel>
#include "pcx_reporting.h"


class PCx_ReportingTableOverviewModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit PCx_ReportingTableOverviewModel(PCx_Reporting * reporting, unsigned int nodeId, MODES::DFRFDIRI mode, QObject *parent = 0);

    unsigned int getNodeId()const{return nodeId;}
    MODES::DFRFDIRI getMode() const{return mode;}
    void setNodeId(unsigned int nodeId);
    void setMode(MODES::DFRFDIRI mode);
    PCx_Reporting::OREDPCR getColRef()const{return colRef;}
    void setColRef(PCx_Reporting::OREDPCR ref);

    /**
     * @brief getDateRef returns the date that should be used as reference, -1 if none
     * @return the date in time_t format
     */
    int getDateRef()const{return dateRef;}
    /**
     * @brief setDateRef sets the date that will be used as reference, as stored in DB
     * @param dateFromTimet the date (in time_t format)
     */
    void setDateRef(int dateFromTimet);


    QVariant data(const QModelIndex &item, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:
    PCx_Reporting *reporting;
    unsigned int nodeId;
    MODES::DFRFDIRI mode;
    PCx_Reporting::OREDPCR colRef;
    int dateRef;
    void updateQuery();

};

#endif // PCX_REPORTINGTABLEOVERVIEWMODEL_H
