#ifndef FORMREPORTINGREPORTS_H
#define FORMREPORTINGREPORTS_H

#include "pcx_reportingwithtreemodel.h"
#include "pcx_report.h"
#include "QCustomPlot/qcustomplot.h"
#include <QWidget>

namespace Ui {
class FormReportingReports;
}

class FormReportingReports : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingReports(QWidget *parent = 0);
    ~FormReportingReports();

public slots:
    void onListOfReportingsChanged();
private slots:
    void on_comboListReportings_activated(int index);

    void on_saveButton_clicked();

    void on_pushButtonExpandAll_clicked();

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonSelectType_clicked();

    void on_pushButtonSelectAllNodes_clicked();

    void on_pushButtonUnSelectAllNodes_clicked();

    void on_pushButtonSelectAll_clicked();

    void on_pushButtonSelectNone_clicked();

private:
    Ui::FormReportingReports *ui;
    //QSize sizeHint()const;
    void updateListOfReportings();
    PCx_ReportingWithTreeModel *model;
    PCx_Report *report;
    QCustomPlot *plot;
};

#endif // FORMREPORTINGREPORTS_H
