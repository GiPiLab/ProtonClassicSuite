#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;
    DialogDisplayTree *ddt;
};

#endif // MAINWINDOW_H
