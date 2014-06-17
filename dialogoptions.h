#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include <QDialog>

namespace Ui {
class DialogOptions;
}

class DialogOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptions(QWidget *parent = 0);
    ~DialogOptions();

private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonReset_clicked();

private:
    Ui::DialogOptions *ui;
};

#endif // DIALOGOPTIONS_H
