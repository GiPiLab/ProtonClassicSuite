#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "formmanageprevisions.h"
#include "formauditprevisions.h"
#include "formreportingreports.h"
#include "formreportingexplore.h"
#include "formreportingsupervision.h"
#include "formreportinggraphics.h"
#include "formmanagereportings.h"
#include "formmanageaudits.h"
#include "formreportingoverview.h"
#include "formedittree.h"
#include "formeditaudit.h"
#include "formauditexplore.h"
#include "formauditreports.h"
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

    void on_actionNewDb_triggered();

    void on_actionOpenDb_triggered();

    void on_actionAuditReport_triggered();

    void onFormReportsWindowsDestroyed();

    void onFormAuditPrevisionsWindowsDestroyed();

    void onFormReportingReportsWindowsDestroyed();

    void on_actionOptions_triggered();

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

    void on_actionReportingGenerateur_de_rapports_triggered();

    void on_actionExploreAudits_triggered();

    void on_actionCascade_triggered();

    void on_actionCloseAllSubwin_triggered();

    void on_actionElaboration_budg_taire_PCB_triggered();

    void on_actionGestion_des_pr_visions_triggered();

    void onFormManagePrevisionsWindowsDestroyed();
private:

    Ui::MainWindow *ui;

    //Only one instance of these dialogs is allowed at once
    FormEditTree *formEditTreeWin;
    FormManageAudits *formManageAudits;
    FormEditAudit *formEditAudit;
    FormAuditReports *formAuditReports;
    FormReportingReports *formReportingReports;
    FormManageReportings *formManageReportings;
    FormAuditPrevisions *formAuditPrevisions;
    FormManagePrevisions *formManagePrevisions;


    //Multiple "tables" dialogs are allowed
    QList<FormAuditExplore *>listOfFormAuditExplore;
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

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
