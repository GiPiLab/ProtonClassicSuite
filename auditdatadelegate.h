#ifndef AUDITDATADELEGATE_H
#define AUDITDATADELEGATE_H

#include <QStyledItemDelegate>

class auditDataDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit auditDataDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // AUDITDATADELEGATE_H
