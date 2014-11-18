#include "pcx_reportingtableoverviewmodel.h"
#include "utils.h"
#include <QSqlRecord>

PCx_ReportingTableOverviewModel::PCx_ReportingTableOverviewModel(PCx_Reporting * reporting, unsigned int nodeId, MODES::DFRFDIRI mode,QObject *parent) :
    QSqlQueryModel(parent),reporting(reporting),nodeId(nodeId),mode(mode)
{
    Q_ASSERT(reporting!=nullptr&&nodeId>0);
    updateQuery();
    colRef=PCx_Reporting::OREDPCR::NONE;
    dateRef=-1;
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

void PCx_ReportingTableOverviewModel::setColRef(PCx_Reporting::OREDPCR ref)
{
    colRef=ref;
    dateRef=-1;
    updateQuery();
}

void PCx_ReportingTableOverviewModel::setDateRef(int dateFromTimet)
{
    dateRef=dateFromTimet;
    colRef=PCx_Reporting::OREDPCR::NONE;
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
    QVariant value;

    if (role == Qt::TextColorRole||role==Qt::DisplayRole)
    {
        value=record(item.row()).value(item.column());

        //The date column
        if(item.column()==0)
            return QDateTime::fromTime_t(value.toUInt()).date().toString(Qt::DefaultLocaleShortDate);

        double computedValue=NUMBERSFORMAT::fixedPointToDouble(value.toLongLong());
        QString retVal;

        if(dateRef!=-1)
        {
            qint64 valRefDate=-1;
            for(int i=0;i<rowCount();i++)
            {
                if(record(i).value("date").toInt()==dateRef)
                {
                    valRefDate=record(i).value(item.column()).toLongLong();
                    break;
                }
            }
            if(valRefDate==-1)
            {
                qDebug()<<"Ref date not found";
            }
            else if(valRefDate==0)
            {
                return "DIV0";
            }
            else
            {
                computedValue=(double)(value.toLongLong()/(double)valRefDate*100);
                retVal=NUMBERSFORMAT::formatDouble(computedValue)+"%";
            }
        }


        else if(colRef!=PCx_Reporting::OREDPCR::NONE)
        {
            qint64 valRef=record(item.row()).value(PCx_Reporting::OREDPCRtoTableString(colRef)).toLongLong();
            if(valRef!=0)
            {
                computedValue=(double)(value.toLongLong()/(double)valRef*100);
                retVal=NUMBERSFORMAT::formatDouble(computedValue)+"%";
            }
            else return "DIV0";
        }
        else
            retVal=NUMBERSFORMAT::formatDouble(computedValue);

        if(role==Qt::DisplayRole)
            return retVal;

        if(role==Qt::TextColorRole)
        {
            if(computedValue<0.0)
            {
                return QVariant::fromValue(QColor(Qt::red));
            }
        }

    }

    else if(role==Qt::TextAlignmentRole && item.column()>0)
    {
        return Qt::AlignCenter;
    }

    return QSqlQueryModel::data(item,role);

}


