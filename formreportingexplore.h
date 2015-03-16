#ifndef FORMREPORTINGEXPLORE_H
#define FORMREPORTINGEXPLORE_H

#include <QWidget>
#include <QTextDocument>
#include "pcx_report.h"

#include "pcx_reportingwithtreemodel.h"

namespace Ui {
class FormReportingExplore;
}

class FormReportingExplore : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingExplore(QWidget *parent = 0);
    ~FormReportingExplore();

public slots:
    void onListOfReportingsChanged();
    void onReportingDataChanged(unsigned int reportingId);
    void onSettingsChanged();

private slots:
    void on_comboListReportings_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_checkBoxA_toggled(bool checked);

    void on_checkBoxB_toggled(bool checked);

    void on_checkBoxC_toggled(bool checked);

    void on_checkBoxD_toggled(bool checked);

    void on_checkBoxS_toggled(bool checked);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_pushButtonExport_clicked();

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonExpandAll_clicked();

private:
    Ui::FormReportingExplore *ui;
    PCx_ReportingWithTreeModel *selectedReporting;
    unsigned int selectedNode;
    void updateListOfReportings();
    QTextDocument *document;
    PCx_Report * report;
    QSize sizeHint()const;

    QList<PCx_Report::PCRPRESETS> getPresets() const;
    MODES::DFRFDIRI getSelectedMode() const;
    void updateTextEdit();
};

#endif // FORMREPORTINGEXPLORE_H
