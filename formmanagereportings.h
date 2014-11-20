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

signals:
    void listOfReportingsChanged();

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

private:
    Ui::FormManageReportings *ui;
    void updateListOfTrees();
    void updateListOfReportings();
    PCx_Reporting *selectedReporting;
};

#endif // FORMMANAGEREPORTINGS_H
