#include "pcx_reportingtableoverviewmodel.h"
#include "utils.h"
#include <QSqlRecord>

PCx_ReportingTableOverviewModel::PCx_ReportingTableOverviewModel(PCx_Reporting * reporting, unsigned int nodeId, PCx_ReportingTableOverviewModel::OVERVIEWMODE mode,QObject *parent):
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

void PCx_ReportingTableOverviewModel::setMode(OVERVIEWMODE mode)
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

QString PCx_ReportingTableOverviewModel::OVERVIEWMODEToCompleteString(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode)
{
    if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF)
        return MODES::modeToCompleteString(MODES::DFRFDIRI::DF);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF)
        return MODES::modeToCompleteString(MODES::DFRFDIRI::RF);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI)
        return MODES::modeToCompleteString(MODES::DFRFDIRI::DI);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI)
        return MODES::modeToCompleteString(MODES::DFRFDIRI::RI);
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF)
        return tr("Synthèse de fonctionnement (RF - DF)");
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI)
        return tr("Synthèse d'investissement (RI - DI)");
    else if(mode==PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI)
        return tr("Synthèse fonctionnement + investissement (RF - DF + RI - DI)");
    else
    {
        qDebug()<<"Invalid mode";
    }
    return QString();
}

void PCx_ReportingTableOverviewModel::setDateRef(int dateFromTimet)
{
    dateRef=dateFromTimet;
    colRef=PCx_Reporting::OREDPCR::NONE;
    updateQuery();
}

void PCx_ReportingTableOverviewModel::updateQuery()
{
    switch(mode)
    {
    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::DF:
        setQuery(QString("select date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1 from reporting_DF_%1 where id_node=%2 order by date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RF:
        setQuery(QString("select date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1 from reporting_RF_%1 where id_node=%2 order by date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::DI:
        setQuery(QString("select date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1 from reporting_DI_%1 where id_node=%2 order by date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RI:
        setQuery(QString("select date,ouverts,realises,engages,disponibles,bp,reports,ocdm,vcdm,budgetvote,vcinterne,rattachenmoins1 from reporting_RI_%1 where id_node=%2 order by date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDF:
        setQuery(QString("select a.date,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as ouverts,"
                         "coalesce(a.realises,0)-coalesce(b.realises,0) as realises,"
                         "coalesce(a.engages,0)-coalesce(b.engages,0) as engages,"
                         "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) as disponibles,"
                         "coalesce(a.bp,0)-coalesce(b.bp,0) as bp,"
                         "coalesce(a.reports,0)-coalesce(b.reports,0) as reports,"
                         "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) as ocdm,"
                         "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) as vcdm,"
                         "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) as budgetvote,"
                         "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) as vcinterne,"
                         "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) as rattachenmoins1 "
                         "from reporting_RF_%1 as a, reporting_DF_%1 as b where a.id_node=%2 and b.id_node=%2 "
                         "and a.date=b.date order by a.date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RIDI:
        setQuery(QString("select a.date,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) as ouverts,"
                         "coalesce(a.realises,0)-coalesce(b.realises,0) as realises,"
                         "coalesce(a.engages,0)-coalesce(b.engages,0) as engages,"
                         "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) as disponibles,"
                         "coalesce(a.bp,0)-coalesce(b.bp,0) as bp,"
                         "coalesce(a.reports,0)-coalesce(b.reports,0) as reports,"
                         "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) as ocdm,"
                         "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) as vcdm,"
                         "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) as budgetvote,"
                         "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) as vcinterne,"
                         "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) as rattachenmoins1 "
                         "from reporting_RI_%1 as a, reporting_DI_%1 as b where a.id_node=%2 and b.id_node=%2 "
                         "and a.date=b.date order by a.date").arg(reporting->getReportingId()).arg(nodeId));
        break;

    case PCx_ReportingTableOverviewModel::OVERVIEWMODE::RFDFRIDI:
        setQuery(QString("select a.date,coalesce(a.ouverts,0)-coalesce(b.ouverts,0) + coalesce(c.ouverts,0)-coalesce(d.ouverts,0) as ouverts,"
                         "coalesce(a.realises,0)-coalesce(b.realises,0) + coalesce(c.realises,0)-coalesce(d.realises,0) as realises,"
                         "coalesce(a.engages,0)-coalesce(b.engages,0) + coalesce(c.engages,0)-coalesce(d.engages,0) as engages,"
                         "coalesce(a.disponibles,0)-coalesce(b.disponibles,0) + coalesce(c.disponibles,0)-coalesce(d.disponibles,0) as disponibles,"
                         "coalesce(a.bp,0)-coalesce(b.bp,0) + coalesce(c.bp,0)-coalesce(d.bp,0) as bp,"
                         "coalesce(a.reports,0)-coalesce(b.reports,0) + coalesce(c.reports,0)-coalesce(d.reports,0) as reports,"
                         "coalesce(a.ocdm,0)-coalesce(b.ocdm,0) + coalesce(c.ocdm,0)-coalesce(d.ocdm,0) as ocdm,"
                         "coalesce(a.vcdm,0)-coalesce(b.vcdm,0) + coalesce(c.vcdm,0)-coalesce(d.vcdm,0) as vcdm,"
                         "coalesce(a.budgetvote,0)-coalesce(b.budgetvote,0) + coalesce(c.budgetvote,0)-coalesce(d.budgetvote,0) as budgetvote,"
                         "coalesce(a.vcinterne,0)-coalesce(b.vcinterne,0) + coalesce(c.vcinterne,0)-coalesce(d.vcinterne,0) as vcinterne,"
                         "coalesce(a.rattachenmoins1,0)-coalesce(b.rattachenmoins1,0) + coalesce(c.rattachenmoins1,0)-coalesce(d.rattachenmoins1,0) as rattachenmoins1 "
                         "from reporting_RF_%1 as a, reporting_DF_%1 as b, reporting_RI_%1 as c, reporting_DI_%1 as d where a.id_node=%2 and b.id_node=%2 "
                         "and c.id_node=%2 and d.id_node=%2 and a.date=b.date and b.date=c.date and c.date=d.date order by a.date").arg(reporting->getReportingId()).arg(nodeId));
        break;
    default:
        qDebug()<<"Unsupported case";
        break;
    }
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
    bool percentMode=false;

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
            percentMode=true;
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
            percentMode=true;
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
            if(percentMode)
            {
                if(computedValue>100.0)
                    return QVariant::fromValue(QColor(Qt::green));
                if(computedValue>0.0 && computedValue<100.0)
                    return QVariant::fromValue(QColor(Qt::blue));
            }
        }
    }

    else if(role==Qt::TextAlignmentRole && item.column()>0)
    {
        return Qt::AlignCenter;
    }

    return QSqlQueryModel::data(item,role);

}


