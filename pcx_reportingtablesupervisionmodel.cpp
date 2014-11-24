#include "pcx_reportingtablesupervisionmodel.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_ReportingTableSupervisionModel::PCx_ReportingTableSupervisionModel(PCx_Reporting *reporting, MODES::DFRFDIRI mode,QObject *parent) :
    QAbstractTableModel(parent),reporting(reporting),currentMode(mode)
{
    updateQuery();
}

void PCx_ReportingTableSupervisionModel::setMode(MODES::DFRFDIRI mode)
{
    currentMode=mode;
    beginResetModel();
    updateQuery();
    endResetModel();
}

QString PCx_ReportingTableSupervisionModel::getColumnName(PCx_ReportingTableSupervisionModel::TABLESUPERVISIONCOLUMNS column)
{
    switch(column)
    {
    case TABLESUPERVISIONCOLUMNS::BP:
        return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::BP);
    case TABLESUPERVISIONCOLUMNS::OUVERTS:
        return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::OUVERTS);
    case TABLESUPERVISIONCOLUMNS::REALISES:
        return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::REALISES);
    case TABLESUPERVISIONCOLUMNS::ENGAGES:
        return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::ENGAGES);
    case TABLESUPERVISIONCOLUMNS::DISPONIBLES:
        return PCx_Reporting::OREDPCRtoCompleteString(PCx_Reporting::OREDPCR::DISPONIBLES);
    case TABLESUPERVISIONCOLUMNS::PERCENTREALISES:
        return tr("Taux de réalisation");
    case TABLESUPERVISIONCOLUMNS::TAUXECART:
        return tr("Taux d'écart");
    case TABLESUPERVISIONCOLUMNS::PERCENTBP:
        return tr("Variation du BP");
    case TABLESUPERVISIONCOLUMNS::PERCENTENGAGES:
        return tr("Taux d'engagé");
    case TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES:
        return tr("Taux de disponible");
    default:
        qDebug()<<"Unsupported mode";
    }
    return QString();
}

int PCx_ReportingTableSupervisionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return entries.count();
}

int PCx_ReportingTableSupervisionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 12;
}


QVariant PCx_ReportingTableSupervisionModel::data(const QModelIndex &index, int role) const
{
    if(role==Qt::DisplayRole||role==Qt::TextColorRole)
    {
        int row=index.row();
        Entry entry=entries.at(row);
        QString output;
        bool percentMode=false;
        double computedValue=NAN;
        switch(index.column())
        {
        case 0:
            return reporting->getAttachedTree()->getNodeName(entry.nodeId);
        case 1:
            return entry.date.toString(Qt::DefaultLocaleShortDate);
        case 2:
            computedValue=entry.BP;
            output=NUMBERSFORMAT::formatDouble(entry.BP);
            break;
        case 3:
            computedValue=entry.ouverts;
            output=NUMBERSFORMAT::formatDouble(entry.ouverts);
            break;
        case 4:
            computedValue=entry.realises;
            output=NUMBERSFORMAT::formatDouble(entry.realises);
            break;
        case 5:
            computedValue=entry.engages;
            output=NUMBERSFORMAT::formatDouble(entry.engages);
            break;
        case 6:
            computedValue=entry.disponibles;
            output=NUMBERSFORMAT::formatDouble(entry.disponibles);
            break;
        case 7:
            if(!qIsNaN(entry.percentReal))
            {
                percentMode=true;
                computedValue=entry.percentReal;
                output=NUMBERSFORMAT::formatDouble(entry.percentReal);
            }
            else
                output="DIV0";
            break;
        case 8:
            if(!qIsNaN(entry.tauxEcart))
            {
                percentMode=true;
                computedValue=entry.tauxEcart;
                output=NUMBERSFORMAT::formatDouble(entry.tauxEcart);
            }
            else
                output="DIV0";
            break;
        case 9:
            if(!qIsNaN(entry.percentBP))
            {
                percentMode=true;
                computedValue=entry.percentBP;
                output=NUMBERSFORMAT::formatDouble(entry.percentBP);
            }
            else
                output="DIV0";
            break;
        case 10:
            if(!qIsNaN(entry.percentEngage))
            {
                percentMode=true;
                computedValue=entry.percentEngage;
                output=NUMBERSFORMAT::formatDouble(entry.percentEngage);
            }
            else
                output="DIV0";
            break;

        case 11:
            if(!qIsNaN(entry.percentDisponible))
            {
                percentMode=true;
                computedValue=entry.percentDisponible;
                output=NUMBERSFORMAT::formatDouble(entry.percentDisponible);
            }
            else
                output="DIV0";
            break;
        }
        if(role==Qt::DisplayRole)
        {
            if(percentMode==true)
                output.append("%");
            return output;
        }
        else if(role==Qt::TextColorRole)
        {
            if(index.column()>1)
            {
                if(computedValue<0.0)
                    return QVariant::fromValue(QColor(Qt::red));
                if(percentMode)
                {
                    if(computedValue>100.0)
                        return QVariant::fromValue(QColor(Qt::green));
                    if(computedValue>0.0 && computedValue<100.0)
                        return QVariant::fromValue(QColor(Qt::blue));
                }
            }
        }
    }
    else if(role==Qt::TextAlignmentRole && index.column()>1)
    {
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant PCx_ReportingTableSupervisionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        if(section==0)
            return tr("Nom");
        else if(section==1)
            return tr("Date");
        else
            return getColumnName((TABLESUPERVISIONCOLUMNS)section);
    }
    return QVariant();
}


void PCx_ReportingTableSupervisionModel::updateQuery()
{
    QSqlQuery q;
    entries.clear();
    if(!q.exec(QString("select id_node,MAX(date) as mDate,bp,ouverts,realises,engages,disponibles from reporting_%1_%2 group by id_node").arg(MODES::modeToTableString(currentMode)).arg(reporting->getReportingId())))
    {
       qCritical()<<q.lastError();
       die();
    }
    while(q.next())
    {
        Entry entry(q.value("id_node").toUInt(),q.value("mDate").toUInt(),q.value("bp").toLongLong(),
                    q.value("ouverts").toLongLong(),q.value("realises").toLongLong(),q.value("engages").toLongLong(),
                    q.value("disponibles").toLongLong());
        entries.append(entry);
    }
}

PCx_ReportingTableSupervisionModel::Entry::Entry(unsigned int nodeId, unsigned int dateTimeT, qint64 BP, qint64 ouverts, qint64 realises, qint64 engages, qint64 disponibles):nodeId(nodeId)

{
    this->BP=NUMBERSFORMAT::fixedPointToDouble(BP);
    this->ouverts=NUMBERSFORMAT::fixedPointToDouble(ouverts);
    this->realises=NUMBERSFORMAT::fixedPointToDouble(realises);
    this->engages=NUMBERSFORMAT::fixedPointToDouble(engages);
    this->disponibles=NUMBERSFORMAT::fixedPointToDouble(disponibles);
    date=QDateTime::fromTime_t(dateTimeT).date();

    int nbJoursDepuisDebutAnnee=date.dayOfYear()+1;
    double jourReal=this->realises/nbJoursDepuisDebutAnnee;

    if(ouverts!=0)
    {
        percentReal=this->realises/this->ouverts*100.0;
        percentBP=this->BP/this->ouverts*100.0;
        percentEngage=this->engages/this->ouverts*100.0;
        percentDisponible=this->disponibles/this->ouverts*100.0;
        tauxEcart=(jourReal*365-this->ouverts)/this->ouverts*100.0;
    }
    else
    {
        percentReal=NAN;
        percentBP=NAN;
        percentEngage=NAN;
        percentDisponible=NAN;
        tauxEcart=NAN;
    }
}
