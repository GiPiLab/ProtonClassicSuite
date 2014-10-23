#ifndef PCX_NODESIMILARITYTABLEMODEL_H
#define PCX_NODESIMILARITYTABLEMODEL_H

#include <QAbstractTableModel>
#include "pcx_stringdistance.h"

class PCx_NodeSimilarityTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    explicit PCx_NodeSimilarityTableModel(const QStringList strings, PCx_StringDistance::SIMILARITYMETRIC metric,QObject *parent = 0);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;



signals:

public slots:


private:
    QStringList listOfStrings;
    QList<PCx_StringDistance> distances;
    PCx_StringDistance::SIMILARITYMETRIC similarityMetric;
    bool computeDistances(void);
    static constexpr double threshold=0.9;

    PCx_NodeSimilarityTableModel(const PCx_NodeSimilarityTableModel &c);
    PCx_NodeSimilarityTableModel &operator=(const PCx_NodeSimilarityTableModel &);



};

#endif
