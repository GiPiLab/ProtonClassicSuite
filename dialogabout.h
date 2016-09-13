#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include "productactivation.h"
#include <QDialog>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

private slots:
    void on_pushButton_clicked();

    void on_pushButtonActivateKey_clicked();

private:
    ProductActivation product;
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
