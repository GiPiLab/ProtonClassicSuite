#ifndef PCX_NODESIMILARITYTABLEMODEL_H
#define PCX_NODESIMILARITYTABLEMODEL_H

#include <QAbstractTableModel>
#include "pcx_stringdistance.h"


/**
 * @brief The PCx_NodeSimilarityTableModel class is a TableModel to show similarity between strings, used to compare node names of a Tree
 */
class PCx_NodeSimilarityTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    /**
     * @brief PCx_NodeSimilarityTableModel constructs a table model with three columns
     *
     * Column 1 is the first string
     * Column 2 is the similarity
     * Column 3 is the second string
     *
     * @param strings a list of strings to compare
     * @param metric the metric to use for similarity
     * @param parent the QObject parent
     */
    explicit PCx_NodeSimilarityTableModel(const QStringList strings, PCx_StringDistance::SIMILARITYMETRIC metric,QObject *parent = 0);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;



signals:

public slots:


private:
    /**
     * @brief listOfStrings the list of strings to compare
     */
    QStringList listOfStrings;

    /**
     * @brief distances the list of computed distances
     */
    QList<PCx_StringDistance> distances;

    /**
     * @brief similarityMetric the similarity used
     */
    PCx_StringDistance::SIMILARITYMETRIC similarityMetric;

    /**
     * @brief computeDistances computes the similarities between listOfStrings and fills distances
     */
    void computeDistances(void);

    /**
     * @brief threshold do not display PCx_StringDistance instances that are lower or equal than this threshold
     */
    static constexpr double threshold=0.9;

    PCx_NodeSimilarityTableModel(const PCx_NodeSimilarityTableModel &c);
    PCx_NodeSimilarityTableModel &operator=(const PCx_NodeSimilarityTableModel &);
};

#endif
