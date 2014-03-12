#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include "dialogdisplaytree.h"

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
    void on_actionAfficher_triggered();

    void on_actionEffacerTout_triggered();

    void on_actionQuitter_triggered();

    void onDdtWindowsDestroyed();



private:

    Ui::MainWindow *ui;
    QMdiSubWindow *ddt;
    DialogDisplayTree *ddtwidget;
};

#endif // MAINWINDOW_H
