#ifndef PCX_STRINGDISTANCETABLEMODEL_H
#define PCX_STRINGDISTANCETABLEMODEL_H

#include <QAbstractTableModel>
#include "pcx_stringdistance.h"

class PCx_StringDistanceTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PCx_StringDistanceTableModel(QObject *parent=0);
    explicit PCx_StringDistanceTableModel(const QStringList strings, QObject *parent = 0);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    bool computeDistances(void);


    unsigned int levenshtein(const QString &sa, const QString &ta);


signals:

public slots:


private:
    QStringList listOfStrings;
    QList<PCx_StringDistance> distances;

};

#endif // PCX_STRINGDISTANCETABLEMODEL_H
