#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QList>
#include "dialogmanageaudits.h"
#include "dialogedittree.h"
#include "dialogeditaudit.h"

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

    void on_actionReset_triggered();

    void on_actionExit_triggered();

    void onDialogEditTreeWindowsDestroyed();

    void onDialogManageAuditsWindowsDestroyed();

    void onDialogEditAuditWindowsDestroyed();

    void on_actionGerer_les_audits_triggered();

    void on_actionSaisie_des_donnees_triggered();

private:

    Ui::MainWindow *ui;
    DialogEditTree *dialogEditTreeWin;
    DialogManageAudits *dialogManageAudits;
    DialogEditAudit *dialogEditAudit;

};

#endif // MAINWINDOW_H
