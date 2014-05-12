#include "auditdatadelegate.h"
#include "pcx_auditmodel.h"
#include "utils.h"
#include <QtGui>
#include <QApplication>
#include <QDoubleSpinBox>
#include <float.h>

auditDataDelegate::auditDataDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}


void auditDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //Add a little horizontal padding
    QRect rect=option.rect;
    rect.setWidth(rect.width()-5);
    rect.setLeft(rect.left()+5);
    painter->save();

    if(!index.data().isNull() && (index.column()==COL_OUVERTS||index.column()==COL_REALISES||index.column()==COL_ENGAGES||index.column()==COL_DISPONIBLES))
    {
        if(index.data().toDouble()<0.0)
        {
            painter->setPen(QColor(255,0,0));
        }
        QString formattedNum=formatDouble(index.data().toDouble());
        painter->drawText(rect,formattedNum,QTextOption(Qt::AlignRight|Qt::AlignVCenter));
    }
    else if(index.column()==COL_ANNEE)
    {
        painter->drawText(rect,index.data().toString(),QTextOption(Qt::AlignLeft|Qt::AlignVCenter));
    }
    painter->restore();
}

QWidget *auditDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    if(index.column()==COL_DISPONIBLES || index.column()==COL_ANNEE)
    {
        return 0;
    }
    QDoubleSpinBox *spin=new QDoubleSpinBox(parent);
    spin->setRange(0.0,DBL_MAX-1);
    return spin;
    //return QStyledItemDelegate::createEditor(parent,option,index);
}

