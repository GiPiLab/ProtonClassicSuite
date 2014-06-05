#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QList>
#include "dialogmanageaudits.h"
#include "dialogedittree.h"
#include "dialogeditaudit.h"
#include "dialogtablesgraphics.h"

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

    void onDialogEditTreeWindowsDestroyed();

    void onDialogManageAuditsWindowsDestroyed();

    void onDialogEditAuditWindowsDestroyed();

    void onDialogTablesWindowsDestroyed(QObject *);

    void on_actionManageAudits_triggered();

    void on_actionEditAudit_triggered();

    void on_actionTablesGraphics_triggered();

    void on_actionNewDb_triggered();

    void on_actionOpenDb_triggered();

private:

    Ui::MainWindow *ui;

    //Only one instance of these dialogs is allowed at once
    DialogEditTree *dialogEditTreeWin;
    DialogManageAudits *dialogManageAudits;
    DialogEditAudit *dialogEditAudit;

    //Multiple "tables" dialogs are allowed
    QList<DialogTablesGraphics *>listOfDialogTablesGraphics;
    QString recentDb;

    void updateTitle();
    void setMenusState();
    void saveSettings();
    void restoreSettings();
};

#endif // MAINWINDOW_H
