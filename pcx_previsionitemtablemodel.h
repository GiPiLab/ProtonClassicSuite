#ifndef PCX_PREVISIONITEMTABLEMODEL_H
#define PCX_PREVISIONITEMTABLEMODEL_H

#include <QAbstractTableModel>
#include "pcx_previsionitem.h"

class PCx_PrevisionItemTableModel : public QAbstractTableModel
{

    Q_OBJECT

public:
    explicit PCx_PrevisionItemTableModel(PCx_PrevisionItem *previsionItem,QObject *parent=0);

    ~PCx_PrevisionItemTableModel();


    PCx_PrevisionItem *getPrevisionItem()const{return previsionItem;}
    void setPrevisionItem(PCx_PrevisionItem *previsionItem);


    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void resetModel();

private:
    PCx_PrevisionItem *previsionItem;

};

#endif // PCX_PREVISIONITEMTABLEMODEL_H
