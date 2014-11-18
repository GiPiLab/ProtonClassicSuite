#ifndef FORMREPORTINGTABLES_H
#define FORMREPORTINGTABLES_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include "pcx_reportingtableoverviewmodel.h"
#include "pcx_reportingwithtreemodel.h"

namespace Ui {
class FormReportingTables;
}

class FormReportingTables : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingTables(QWidget *parent = 0);
    ~FormReportingTables();


public slots:
    void onListOfReportingsChanged();

private slots:
    void on_comboListReportings_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);


    void on_checkBoxBP_toggled(bool checked);

    void on_checkBoxReports_toggled(bool checked);

    void on_checkBoxOCDM_toggled(bool checked);

    void on_checkBoxVCDM_toggled(bool checked);

    void on_checkBoxBudgetVote_toggled(bool checked);

    void on_checkBoxVCInternes_toggled(bool checked);

    void on_checkBoxRattaches_toggled(bool checked);

    void on_checkBoxOuverts_toggled(bool checked);

    void on_checkBoxRealises_toggled(bool checked);

    void on_checkBoxEngages_toggled(bool checked);

    void on_checkBoxDisponibles_toggled(bool checked);

    void on_pushButtonExportHtml_clicked();

    void on_comboBoxRefColumn_activated(int index);

    void on_comboBoxRefDate_activated(int index);

private:
    Ui::FormReportingTables *ui;
    void updateListOfReportings();
    PCx_ReportingWithTreeModel *selectedReporting;
    PCx_ReportingTableOverviewModel *tableOverviewModel;
    QSortFilterProxyModel *proxyModel;
    MODES::DFRFDIRI getSelectedMode() const;
    unsigned int selectedNodeId;
};

#endif // FORMREPORTINGTABLES_H
