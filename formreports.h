#ifndef FORMREPORTS_H
#define FORMREPORTS_H

#include "pcx_auditmodel.h"
#include <QWidget>

namespace Ui {
class FormReports;
}

class FormReports : public QWidget
{
    Q_OBJECT

public:
    explicit FormReports(QWidget *parent = 0);
    ~FormReports();

public slots:
    void onListOfAuditsChanged();
private slots:
    void on_comboListAudits_activated(int index);

    void on_pushButton_clicked();

private:
    Ui::FormReports *ui;
    void updateListOfAudits();
    void setEnabled(bool state);
    PCx_AuditModel *model;
};

#endif // FORMREPORTS_H
