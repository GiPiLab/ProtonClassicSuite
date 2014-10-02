#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QList>

#include "formmanageaudits.h"
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

#define MAXRECENTFILES 4

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

private:

    Ui::MainWindow *ui;

    //Only one instance of these dialogs is allowed at once
    FormEditTree *formEditTreeWin;
    FormManageAudits *formManageAudits;
    FormEditAudit *formEditAudit;
    FormReports *formReports;

    //Multiple "tables" dialogs are allowed
    QList<FormTablesGraphics *>listOfFormTablesGraphics;

    QList<FormQueries *>listOfFormQueries;
    QString recentDb;

    void updateTitle();
    void setMenusState();
    void saveSettings();
    void restoreSettings();
};

#endif // MAINWINDOW_H
