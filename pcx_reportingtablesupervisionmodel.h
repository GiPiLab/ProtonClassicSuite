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
    explicit PCx_ReportingTableSupervisionModel(PCx_Reporting *reporting, MODES::DFRFDIRI mode, QObject *parent = 0);

    enum TABLESUPERVISIONCOLUMNS
    {
        BP=2,
        OUVERTS,
        REALISES,
        ENGAGES,
        DISPONIBLES,
        PERCENTREALISES,
        TAUXECART,
        PERCENTBP,
        PERCENTENGAGES,
        PERCENTDISPONIBLES
    };



    MODES::DFRFDIRI getMode()const{return currentMode;}
    void setMode(MODES::DFRFDIRI mode);

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
        explicit Entry(unsigned int nodeId,unsigned int dateTimeT,qint64 BP,qint64 ouverts,qint64 realises,qint64 engages,qint64 disponibles);
        unsigned int nodeId;
        QDate date;
        double BP,ouverts,realises,engages,disponibles;
        double percentReal,tauxEcart,percentEngage,percentDisponible,percentBP;
    };

    PCx_Reporting *reporting;
    MODES::DFRFDIRI currentMode;
    void updateQuery();
    unsigned int nbNodes;
    QList<Entry> entries;

};

#endif // PCX_REPORTINGTABLESUPERVISIONMODEL_H
