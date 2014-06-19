#ifndef FORMQUERIES_H
#define FORMQUERIES_H

#include "pcx_auditmodel.h"
#include <QWidget>

namespace Ui {
class FormQueries;
}

class FormQueries : public QWidget
{
    Q_OBJECT

public:
    explicit FormQueries(QWidget *parent = 0);
    ~FormQueries();

private slots:
    void on_comboBoxListAudits_activated(int index);

private:
    Ui::FormQueries *ui;
    void updateListOfAudits();
    PCx_AuditModel *model;
};

#endif // FORMQUERIES_H
