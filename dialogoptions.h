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

    void on_pushButtonColorReqVar_clicked();

    void on_pushButtonColorReqRank_clicked();

    void on_pushButtonColorReqMinMax_clicked();

private:
    Ui::DialogOptions *ui;
    QColor colorReqVar,colorReqMinMax,colorReqRank;
};

#endif // DIALOGOPTIONS_H
