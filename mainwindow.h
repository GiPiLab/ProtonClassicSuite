#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QList>
#include "pcx_treemodel.h"
#include "dialogedittree.h"

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

private:

    Ui::MainWindow *ui;
    DialogEditTree *dialogEditTreeWin;
};

#endif // MAINWINDOW_H
