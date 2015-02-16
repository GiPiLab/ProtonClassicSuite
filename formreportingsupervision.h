#ifndef FORMREPORTINGSUPERVISION_H
#define FORMREPORTINGSUPERVISION_H

#include "pcx_reporting.h"
#include "pcx_reportingtablesupervisionmodel.h"
#include <QSortFilterProxyModel>
#include <QWidget>

namespace Ui {
class FormReportingSupervision;
}

class FormReportingSupervision : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingSupervision(QWidget *parent = 0);
    ~FormReportingSupervision();

public slots:
    void onListOfReportingsChanged();
    void onReportingDataChanged(unsigned int reportingId);

private slots:
    void on_comboListReportings_activated(int index);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_checkBoxBP_toggled(bool checked);

    void on_checkBoxOuverts_toggled(bool checked);

    void on_checkBoxRealises_toggled(bool checked);

    void on_checkBoxEngages_toggled(bool checked);

    void on_checkBoxDisponibles_toggled(bool checked);

    void on_checkBoxTauxReal_toggled(bool checked);

    void on_checkBoxTauxEcart_toggled(bool checked);

    void on_checkBoxVariationBP_toggled(bool checked);

    void on_checkBoxTauxEngage_toggled(bool checked);

    void on_checkBoxTauxDisponible_toggled(bool checked);

    void on_pushButtonExportHTML_clicked();

    void on_checkBoxRealisePredit_toggled(bool checked);

    void on_checkBoxDifference_toggled(bool checked);

    void on_checkBoxDECICO_toggled(bool checked);

    void on_checkBoxERO2_toggled(bool checked);

    void on_checkBoxRAC_toggled(bool checked);

    void on_checkBox15NRest_toggled(bool checked);

    void on_checkBoxCPP15N_toggled(bool checked);

    void on_pushButtonSelectNone_clicked();

    void on_pushButtonSelectAll_clicked();

    void on_comboBoxListDates_activated(int index);

private:
    Ui::FormReportingSupervision *ui;
    QSize sizeHint()const;
    void updateDateRefCombo();

    PCx_Reporting *selectedReporting;
    PCx_ReportingTableSupervisionModel *model;
    QSortFilterProxyModel *proxyModel;
    void updateListOfReportings();
    MODES::DFRFDIRI getSelectedMode() const;
    void setColumnVisibility();
};

#endif // FORMREPORTINGSUPERVISION_H
