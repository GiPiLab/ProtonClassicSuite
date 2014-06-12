#ifndef FORMREPORTS_H
#define FORMREPORTS_H

#include "pcx_auditmodel.h"
#include "QCustomPlot/qcustomplot.h"
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

    void on_saveButton_clicked();

    void on_pushButtonPoidsRelatifs_clicked();

    void on_pushButtonBase100_clicked();

    void on_pushButtonEvolution_clicked();

    void on_pushButtonEvolutionCumul_clicked();

    void on_pushButtonJoursAct_clicked();

    void on_pushButtonResultats_clicked();

    void on_pushButtonExpandAll_clicked();

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonSelectAll_clicked();

    void on_pushButtonSelectNone_clicked();

    void on_pushButtonSelectType_clicked();

    void on_pushButtonSelectAllNodes_clicked();

    void on_pushButtonUnSelectAllNodes_clicked();

private:
    Ui::FormReports *ui;
    void updateListOfAudits();
    PCx_AuditModel *model;
    void populateLists();
    QCustomPlot *plot;
};

#endif // FORMREPORTS_H
