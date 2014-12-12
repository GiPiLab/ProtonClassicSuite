#ifndef FORMREPORTINGOVERVIEW_H
#define FORMREPORTINGOVERVIEW_H

#include <QWidget>
#include <QTextDocument>
#include <QSortFilterProxyModel>
#include "pcx_reportingtableoverviewmodel.h"
#include "pcx_reportingwithtreemodel.h"

namespace Ui {
class FormReportingOverview;
}

class FormReportingOverview : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingOverview(QWidget *parent = 0);
    ~FormReportingOverview();


public slots:
    void onListOfReportingsChanged();
    void onReportingDataChanged(unsigned int reportingId);

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

    void on_pushButtonCopyToDocument_clicked();

    void on_comboBoxRefColumn_activated(int index);

    void on_comboBoxRefDate_activated(int index);

    void on_radioButtonRFDF_toggled(bool checked);

    void on_radioButtonRIDI_toggled(bool checked);

    void on_radioButtonRFDFRIDI_toggled(bool checked);



    void on_pushButtonClear_clicked();

    void on_pushButtonSelectAll_clicked();

    void on_pushButtonSelectNone_clicked();

    void on_pushButtonExportHTML_clicked();

    void on_pushButtonExpandAll_clicked();

    void on_pushButtonCollapseAll_clicked();

private:
    Ui::FormReportingOverview *ui;
    void updateListOfReportings();
    void updateComboRefDate(QComboBox *combo);
    void changeMode(PCx_ReportingTableOverviewModel::OVERVIEWMODE mode);

    bool dateExistsForNodeAndMode(unsigned int timeT, unsigned int nodeId, PCx_ReportingTableOverviewModel::OVERVIEWMODE mode) const;
    PCx_ReportingWithTreeModel *selectedReporting;
    PCx_ReportingTableOverviewModel *tableOverviewModel;
    QSortFilterProxyModel *proxyModel;
    PCx_ReportingTableOverviewModel::OVERVIEWMODE getSelectedMode() const;
    unsigned int selectedNodeId;
};

#endif // FORMREPORTINGOVERVIEW_H
