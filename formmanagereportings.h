#ifndef FORMMANAGEREPORTINGS_H
#define FORMMANAGEREPORTINGS_H

#include <QWidget>
#include "pcx_reporting.h"

namespace Ui {
class FormManageReportings;
}

class FormManageReportings : public QWidget
{
    Q_OBJECT

public:
    explicit FormManageReportings(QWidget *parent = 0);
    ~FormManageReportings();

public slots:
    void onLOTchanged();
    void updateRandomVisibility();
    void onListOfAuditsChanged();


signals:
    void listOfReportingsChanged();
    void reportingDataUpdated(unsigned int reportingId);
    void auditDataUpdated(unsigned int auditId);

private slots:
    void on_pushButtonDisplayTree_clicked();

    void on_pushButtonAddReporting_clicked();

    void on_pushButtonDeleteReporting_clicked();

    void on_comboListOfReportings_activated(int index);

    void on_pushButtonExportLeaves_clicked();

    void on_pushButtonLoadDF_clicked();

    void on_pushButtonLoadRF_clicked();

    void on_pushButtonLoadDI_clicked();

    void on_pushButtonLoadRI_clicked();

    void on_pushButtonExportDF_clicked();

    void on_pushButtonExportRF_clicked();

    void on_pushButtonExportDI_clicked();

    void on_pushButtonExportRI_clicked();

    void on_pushButtonClearDF_clicked();

    void on_pushButtonClearRF_clicked();

    void on_pushButtonClearDI_clicked();

    void on_pushButtonClearRI_clicked();

    void on_pushButtonRandomDF_clicked();

    void on_pushButtonRandomRF_clicked();

    void on_pushButtonRandomDI_clicked();

    void on_pushButtonRandomRI_clicked();

    void on_pushButtonFillAudit_clicked();

    void on_pushButtonDeleteLastDF_clicked();

    void on_pushButtonDeleteLastRF_clicked();

    void on_pushButtonDeleteLastDI_clicked();

    void on_pushButtonDeleteLastRI_clicked();

private:
    Ui::FormManageReportings *ui;
    void updateListOfTrees();
    void updateListOfReportings();
    void updateListOfPotentialAudits();
    PCx_Reporting *selectedReporting;
    void updateReportingInfos();
};

#endif // FORMMANAGEREPORTINGS_H
