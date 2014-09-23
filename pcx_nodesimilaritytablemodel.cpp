#include "pcx_nodesimilaritytablemodel.h"
#include "utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QProgressDialog>


PCx_NodeSimilarityTableModel::PCx_NodeSimilarityTableModel(const QStringList strings, PCx_StringDistance::SIMILARITYMETRIC metric, QObject *parent) :
    QAbstractTableModel(parent),listOfStrings(strings),similarityMetric(metric)
{
    computeDistances();
}

int PCx_NodeSimilarityTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return distances.size();

}

int PCx_NodeSimilarityTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant PCx_NodeSimilarityTableModel::data(const QModelIndex &index, int role) const
{
    if(role==Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return distances.at(index.row()).getStr1();

        case 1:
            return distances.at(index.row()).getNormalizedSimilarity();

        case 2:
            return distances.at(index.row()).getStr2();
        }

    }
    return QVariant();

}

QVariant PCx_NodeSimilarityTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if(role==Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Noeud 1");
        case 1:
            return tr("Similarité");
        case 2:
            return tr("Noeud 2");
        }
    }
    return QVariant();
}

bool PCx_NodeSimilarityTableModel::computeDistances()
{
    QHash<QString,bool> notCompute;
    QElapsedTimer timer;

    int maxVal=listOfStrings.size();

    QProgressDialog progress(QObject::tr("Calcul des similarités..."),0,0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);
    progress.setValue(0);

    timer.start();
    int nbVals=0;

    foreach(const QString & str1, listOfStrings)
    {
        foreach(const QString &str2, listOfStrings)
        {
            if(str1==str2)
                continue;
            if(notCompute.contains(str1+str2))
            {
                continue;
            }
            else
            {
                PCx_StringDistance dist(str1,str2,similarityMetric);
                if(dist.getNormalizedSimilarity()>0.0)
                {
                    distances.append(dist);
                }
                notCompute.insert(str2+str1,true);
            }
        }
        progress.setValue(++nbVals);
    }
    qDebug()<<distances.size()<<" distances computed in"<<timer.elapsed()<<"ms";

    return true;
}




