#include "pcx_reportingtableoverviewmodel.h"
#include "utils.h"

PCx_ReportingTableOverviewModel::PCx_ReportingTableOverviewModel(PCx_Reporting * reporting, unsigned int nodeId, MODES::DFRFDIRI mode,QObject *parent) :
    QSqlQueryModel(parent),reporting(reporting),nodeId(nodeId),mode(mode)
{
    Q_ASSERT(reporting!=nullptr&&nodeId>0);
    updateQuery();
}

void PCx_ReportingTableOverviewModel::setNodeId(unsigned int nodeId)
{
    this->nodeId=nodeId;
    updateQuery();
}

void PCx_ReportingTableOverviewModel::setMode(MODES::DFRFDIRI mode)
{
    this->mode=mode;
    updateQuery();
}

void PCx_ReportingTableOverviewModel::updateQuery()
{
    setQuery(QString("select date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1 from reporting_%1_%2 where id_node=%3 order by date").arg(MODES::modeToTableString(mode)).arg(reporting->getReportingId()).arg(nodeId));
}

QVariant PCx_ReportingTableOverviewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant value=QSqlQueryModel::headerData(section,orientation,role);
    if(value.isValid()&& role==Qt::DisplayRole)
    {
        QString val=value.toString();
        if(val!="date")
            return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCRFromTableString(val));
    }
    return value;

}

QVariant PCx_ReportingTableOverviewModel::data(const QModelIndex &item, int role) const
{
    QVariant value = QSqlQueryModel::data(item, role);

    if (value.isValid() && role == Qt::DisplayRole) {

            if(item.column()==0)
            {
                return QDateTime::fromTime_t(value.toUInt()).date().toString(Qt::DefaultLocaleShortDate);
            }

            if (item.column() >0)
            {
                return NUMBERSFORMAT::formatFixedPoint(value.toLongLong());
            }
    }
        //if (role == Qt::TextColorRole && index.column() == 1)
         //   return QVariant::fromValue(QColor(Qt::blue));

    return value;
}


