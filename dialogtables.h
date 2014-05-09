#ifndef DIALOGTABLES_H
#define DIALOGTABLES_H

#include <QWidget>
#include "pcx_tables.h"

namespace Ui {
class DialogTables;
}

class DialogTables : public QWidget
{
    Q_OBJECT

public:
    explicit DialogTables(QWidget *parent = 0);
    ~DialogTables();

private slots:

    void on_comboListAudits_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::DialogTables *ui;
    void updateListOfAudits();
    PCx_tables *tables;
};

#endif // DIALOGTABLES_H
