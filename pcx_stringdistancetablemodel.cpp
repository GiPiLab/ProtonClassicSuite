#include "pcx_stringdistancetablemodel.h"
#include "utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QProgressDialog>

PCx_StringDistanceTableModel::PCx_StringDistanceTableModel(QObject *parent):QAbstractTableModel(parent)
{
}

PCx_StringDistanceTableModel::PCx_StringDistanceTableModel(const QStringList strings, QObject *parent) :
    QAbstractTableModel(parent),listOfStrings(strings)
{
    computeDistances();
}

int PCx_StringDistanceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfStrings.length();

}

int PCx_StringDistanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant PCx_StringDistanceTableModel::data(const QModelIndex &index, int role) const
{
    if(role==Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return distances.at(index.row()).getStr1();
            break;
        case 1:
            return distances.at(index.row()).getDistance();
            break;
        case 2:
            return distances.at(index.row()).getStr2();
        }

    }
    return QVariant();

}

bool PCx_StringDistanceTableModel::computeDistances()
{
    QHash<QString,bool> notCompute;
    QElapsedTimer timer;

    int maxVal=listOfStrings.size();

    QProgressDialog progress(QObject::tr("Calcul des similarités..."),0,0,maxVal);

    progress.setWindowModality(Qt::ApplicationModal);

    progress.setMinimumDuration(300);

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
                distances.append(PCx_StringDistance(str1,str2));
                notCompute.insert(str2+str1,true);
            }
        }
        progress.setValue(++nbVals);
    }
    qDebug()<<distances.size()<<" distances computed in"<<timer.elapsed()<<"ms";

    return true;
}




