#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "formreportingexplore.h"
#include "formreportingsupervision.h"
#include "formreportinggraphics.h"
#include "formmanagereportings.h"
#include "formmanageaudits.h"
#include "formreportingoverview.h"
#include "formedittree.h"
#include "formeditaudit.h"
#include "formtablesgraphics.h"
#include "formreports.h"
#include "dialogoptions.h"
#include "formqueries.h"
#include "utils.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_actionManageTree_triggered();

    void on_actionExit_triggered();

    void onFormEditTreeWindowsDestroyed();

    void onFormManageAuditsWindowsDestroyed();

    void onFormManageReportingsWindowsDestroyed();

    void onFormEditAuditWindowsDestroyed();

    void onFormTablesWindowsDestroyed(QObject *);

    void onFormQueriesWindowsDestroyed(QObject *obj);

    void on_actionManageAudits_triggered();

    void on_actionEditAudit_triggered();

    void on_actionTablesGraphics_triggered();

    void on_actionNewDb_triggered();

    void on_actionOpenDb_triggered();

    void on_actionReport_triggered();

    void onFormReportsWindowsDestroyed();

    void on_actionO_ptions_triggered();

    void on_actionQueries_triggered();

    void on_actionA_propos_triggered();

    void on_actionGestion_des_reportings_triggered();

    void on_actionReportingOverview_triggered();

    void onFormReportingTablesWindowsDestroyed(QObject *obj);
    void onFormReportingSupervisionWindowsDestroyed(QObject *obj);
    void onFormReportingGraphicsWindowsDestroyed(QObject *obj);
    void onFormReportingExploreWindowsDestroyed(QObject *obj);


    void on_actionSurveillance_des_reportings_triggered();

    void on_actionGraphiques_triggered();

    void on_actionExploreReportings_triggered();

private:

    Ui::MainWindow *ui;

    //Only one instance of these dialogs is allowed at once
    FormEditTree *formEditTreeWin;
    FormManageAudits *formManageAudits;
    FormEditAudit *formEditAudit;
    FormReports *formReports;
    FormManageReportings *formManageReportings;


    //Multiple "tables" dialogs are allowed
    QList<FormTablesGraphics *>listOfFormTablesGraphics;

    QList<FormReportingOverview *>listOfFormReportingOverview;
    QList<FormReportingSupervision *>listOfFormReportingSupervision;
    QList<FormReportingGraphics *>listOfFormReportingGraphics;
    QList<FormReportingExplore *>listOfFormReportingExplore;

    QList<FormQueries *>listOfFormQueries;
    QString recentDb;

    void updateTitle();
    void setMenusState();
    void saveSettings();
    void restoreSettings();
};

#endif // MAINWINDOW_H
