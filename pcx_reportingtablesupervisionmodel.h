#ifndef PCX_REPORTINGTABLESUPERVISIONMODEL_H
#define PCX_REPORTINGTABLESUPERVISIONMODEL_H

#include "pcx_reporting.h"
#include "utils.h"
#include <QAbstractTableModel>
#include <QList>

class PCx_ReportingTableSupervisionModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PCx_ReportingTableSupervisionModel(PCx_Reporting *reporting, MODES::DFRFDIRI mode, int selectedDateTimeT=-1, QObject *parent = 0);

    enum TABLESUPERVISIONCOLUMNS
    {
        BP=2,
        OUVERTS,
        REALISES,
        ENGAGES,
        DISPONIBLES,
        PERCENTREALISES,        
        PERCENTBP,
        PERCENTENGAGES,
        PERCENTDISPONIBLES,
        REALISESPREDITS, //Realises prédits à ce rythme
        DIFFREALISESPREDITSOUVERTS, //Difference REALISESPREDITS-OUVERTS
        TAUXECART,
        DECICP, //Date estimee de consommation integrale des credits
        DERPSUR2, //Date estimee du realise = ouverts/2
        NB15NRESTANTES, //Nombre de 15N restant dans l'année
        RAC, //Reste à consommer
        CPP15NR //Consommé prévisionnel par 15N restante

    };



    MODES::DFRFDIRI getMode()const{return currentMode;}
    void setMode(MODES::DFRFDIRI mode);

    void setDateTimeT(int dateTimeT);
    int getSelectedDateTimeT()const{return selectedDateTimeT;}

    static QString getColumnName(TABLESUPERVISIONCOLUMNS column);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:

    class Entry
    {
    public:
        explicit Entry(unsigned int nodeId,unsigned int dateTimeT,qint64 _bp,qint64 ouverts,qint64 realises,qint64 engages,qint64 disponibles);
        unsigned int nodeId;
        QDate date,dECICO,dERO2;
        double bp,ouverts,realises,engages,disponibles;
        double percentReal,tauxEcart,percentEngage,percentDisponible,percentBP;
        double realisesPredits,diffRealisesPreditsOuverts,resteAConsommer;
        unsigned int nb15NRestantes;
        double consommePrevPar15N;
    };

    PCx_Reporting *reporting;
    MODES::DFRFDIRI currentMode;
    void updateQuery();
    unsigned int nbNodes;
    QList<Entry> entries;
    int selectedDateTimeT;

};

#endif // PCX_REPORTINGTABLESUPERVISIONMODEL_H
