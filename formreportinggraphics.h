#ifndef FORMREPORTINGGRAPHICS_H
#define FORMREPORTINGGRAPHICS_H

#include <QWidget>
#include "pcx_reportingwithtreemodel.h"

namespace Ui {
class FormReportingGraphics;
}

class FormReportingGraphics : public QWidget
{
    Q_OBJECT

public slots:
    void onListOfReportingsChanged();

public:
    explicit FormReportingGraphics(QWidget *parent = 0);
    ~FormReportingGraphics();

private slots:
    void on_comboListOfReportings_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_checkBoxBP_toggled(bool checked);

    void on_checkBoxReports_toggled(bool checked);

    void on_checkBoxOCDM_toggled(bool checked);

    void on_checkBoxVCDM_toggled(bool checked);

    void on_checkBoxBudgetVote_toggled(bool checked);

    void on_checkBoxVInternes_toggled(bool checked);

    void on_checkBoxRattachesN1_toggled(bool checked);

    void on_checkBoxPrevus_toggled(bool checked);

    void on_checkBoxRealises_toggled(bool checked);

    void on_checkBoxEngages_toggled(bool checked);

    void on_checkBoxDisponibles_toggled(bool checked);

    void on_pushButtonExportPlot_clicked();

private:
    Ui::FormReportingGraphics *ui;
    PCx_ReportingWithTreeModel *selectedReporting;
    unsigned int selectedNodeId;
    void updateListOfReportings();
    MODES::DFRFDIRI getSelectedMode() const;
    void updatePlot();
    QList<PCx_Reporting::OREDPCR> getSelectedOREDPCR() const;
    QSize sizeHint() const;
};

#endif // FORMREPORTINGGRAPHICS_H
