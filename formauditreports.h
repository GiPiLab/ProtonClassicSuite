#ifndef FORMAUDITREPORTS_H
#define FORMAUDITREPORTS_H

#include "pcx_auditwithtreemodel.h"
#include "pcx_report.h"
#include "QCustomPlot/qcustomplot.h"
#include <QWidget>

namespace Ui {
class FormAuditReports;
}

class FormAuditReports : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditReports(QWidget *parent = 0);
    ~FormAuditReports();

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

    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::FormAuditReports *ui;
    QSize sizeHint()const;
    void updateListOfAudits();
    unsigned int referenceNode;
    PCx_AuditWithTreeModel *model;
    PCx_Report *report;
    void populateLists();
    QCustomPlot *plot;
};

#endif // FORMAUDITREPORTS_H
