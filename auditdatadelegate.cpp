#include "auditdatadelegate.h"
#include "pcx_auditmodel.h"
#include "utils.h"

#include <QPainter>
#include <QDoubleSpinBox>
#include <QDebug>

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

    if(!index.data().isNull() && (index.column()==PCx_AuditModel::COL_OUVERTS||index.column()==PCx_AuditModel::COL_REALISES||index.column()==PCx_AuditModel::COL_ENGAGES||index.column()==PCx_AuditModel::COL_DISPONIBLES))
    {
        qint64 data=index.data().toLongLong();
        if(data<0)
        {
            painter->setPen(QColor(255,0,0));
        }
        QString formattedNum=formatCurrency(data);
        painter->drawText(rect,formattedNum,QTextOption(Qt::AlignRight|Qt::AlignVCenter));
    }
    else if(index.column()==PCx_AuditModel::COL_ANNEE)
    {
        painter->drawText(rect,index.data().toString(),QTextOption(Qt::AlignLeft|Qt::AlignVCenter));
    }
    painter->restore();
}

void auditDataDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    QDoubleSpinBox *edit=(QDoubleSpinBox *)editor;
    //Convert fixed point arithmetics to double for displaying
    edit->setValue(index.data().toLongLong()/(double)FIXEDPOINTCOEFF);
}


void auditDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *edit=(QDoubleSpinBox *)editor;
    double value=edit->value();
    if(value>MAX_NUM)value=MAX_NUM;
    edit->setValue(value*FIXEDPOINTCOEFF);
    QStyledItemDelegate::setModelData(edit,model,index);
}

QWidget *auditDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    if(index.column()==PCx_AuditModel::COL_DISPONIBLES || index.column()==PCx_AuditModel::COL_ANNEE)
    {
        return 0;
    }
    QDoubleSpinBox *spin=new QDoubleSpinBox(parent);

    spin->setRange(0.0,Q_INFINITY);
    return spin;
}

