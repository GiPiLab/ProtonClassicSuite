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
        return tr("% réalisé");
    case TABLESUPERVISIONCOLUMNS::TAUXECART:
        return tr("Taux d'écart");
    case TABLESUPERVISIONCOLUMNS::PERCENTBP:
        return tr("% BP");
    case TABLESUPERVISIONCOLUMNS::PERCENTENGAGES:
        return tr("% engagé");
    case TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES:
        return tr("% disponible");
    case TABLESUPERVISIONCOLUMNS::REALISESPREDITS:
        return tr("Réalises prédits");
    case TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS:
        return tr("Diff. estimée");
    case TABLESUPERVISIONCOLUMNS::ECICO:
        return tr("DECICP");
    case TABLESUPERVISIONCOLUMNS::ERO2:
        return tr("DER=P/2");
    case TABLESUPERVISIONCOLUMNS::RAC:
        return tr("RAC");
    case TABLESUPERVISIONCOLUMNS::NB15NRESTANTES:
        return tr("#15NR");
    case TABLESUPERVISIONCOLUMNS::CPP15NR:
        return tr("CPP15N");

    default:
        qWarning()<<"Unsupported mode";
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
    return 19;
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
        case TABLESUPERVISIONCOLUMNS::BP:
            computedValue=entry.BP;
            break;
        case TABLESUPERVISIONCOLUMNS::OUVERTS:
            computedValue=entry.ouverts;
            break;
        case TABLESUPERVISIONCOLUMNS::REALISES:
            computedValue=entry.realises;
            break;
        case TABLESUPERVISIONCOLUMNS::ENGAGES:
            computedValue=entry.engages;
            break;
        case TABLESUPERVISIONCOLUMNS::DISPONIBLES:
            computedValue=entry.disponibles;
            break;
        case TABLESUPERVISIONCOLUMNS::PERCENTREALISES:
            if(!qIsNaN(entry.percentReal))
            {
                percentMode=true;
                computedValue=entry.percentReal;
            }
            else
                return("DIV0");
            break;
        case TABLESUPERVISIONCOLUMNS::TAUXECART:
            if(!qIsNaN(entry.tauxEcart))
            {
                percentMode=true;
                computedValue=entry.tauxEcart;
            }
            else
                return("DIV0");
            break;
        case TABLESUPERVISIONCOLUMNS::PERCENTBP:
            if(!qIsNaN(entry.percentBP))
            {
                percentMode=true;
                computedValue=entry.percentBP;
            }
            else
                return("DIV0");
            break;
        case TABLESUPERVISIONCOLUMNS::PERCENTENGAGES:
            if(!qIsNaN(entry.percentEngage))
            {
                percentMode=true;
                computedValue=entry.percentEngage;
            }
            else
                return("DIV0");
            break;

        case TABLESUPERVISIONCOLUMNS::PERCENTDISPONIBLES:
            if(!qIsNaN(entry.percentDisponible))
            {
                percentMode=true;
                computedValue=entry.percentDisponible;
            }
            else
                return("DIV0");
            break;

        case TABLESUPERVISIONCOLUMNS::REALISESPREDITS:
            computedValue=entry.realisesPredits;
            break;

        case TABLESUPERVISIONCOLUMNS::DIFFREALISESPREDITSOUVERTS:
            computedValue=entry.diffRealisesPreditsOuverts;
            break;

        case TABLESUPERVISIONCOLUMNS::ECICO:
            return entry.dECICO.toString(Qt::DefaultLocaleShortDate);
            break;

        case TABLESUPERVISIONCOLUMNS::ERO2:
            return entry.dERO2.toString(Qt::DefaultLocaleShortDate);
            break;

        case TABLESUPERVISIONCOLUMNS::RAC:
            computedValue=entry.resteAConsommer;
            break;

        case TABLESUPERVISIONCOLUMNS::NB15NRESTANTES:
            return(entry.nb15NRestantes);
            break;

        case TABLESUPERVISIONCOLUMNS::CPP15NR:
            computedValue=entry.consommePrevPar15N;
            break;
        }


        output=NUMBERSFORMAT::formatDouble(computedValue);


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

    int nbJoursDepuisDebutAnnee=date.dayOfYear();
    int nbJoursRestants=365-nbJoursDepuisDebutAnnee;
    nb15NRestantes=(int)(floor((double)nbJoursRestants/15.0));
    resteAConsommer=this->ouverts-this->realises;
    if(nb15NRestantes!=0)
    {
        consommePrevPar15N=resteAConsommer/nb15NRestantes;
    }
    else
        consommePrevPar15N=NAN;

   // qDebug()<<nbJoursDepuisDebutAnnee;
    double jourReal=this->realises/nbJoursDepuisDebutAnnee;
    realisesPredits=this->realises+(this->realises*nbJoursRestants/nbJoursDepuisDebutAnnee);
    diffRealisesPreditsOuverts=this->ouverts-realisesPredits;

    this->dECICO=QDate();
    this->dERO2=QDate();
    if(realisesPredits!=0.0)
    {
        int joursDCICO=(int)(365*this->ouverts/realisesPredits);
        if(joursDCICO>0)
        {
            QDate tmpDate(date.year(),1,1);
            dECICO=tmpDate.addDays(joursDCICO);
        }
    }

    double ouvertsDemi=this->ouverts/2.0;
    if(this->realises<ouvertsDemi)
    {
        if(this->realises!=0.0)
        {
            QDate tmpDate(date.year(),1,1);
            int joursCoupure=(int)(ouvertsDemi*nbJoursDepuisDebutAnnee/this->realises);
            this->dERO2=tmpDate.addDays(joursCoupure);
        }
    }



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
