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

    void on_pushButtonColorPen1_clicked();

    void on_pushButtonColorPen2_clicked();

    void on_sliderAlpha_valueChanged(int value);

    void on_pushButtonColorDF_clicked();

    void on_pushButtonColorRF_clicked();

    void on_pushButtonColorDI_clicked();

    void on_pushButtonColorRI_clicked();

private:
    Ui::DialogOptions *ui;
    QColor colorReqVar,colorReqMinMax,colorReqRank,colorPen1,colorPen2,colorDFBar,colorRFBar,colorDIBar,colorRIBar;
    int alpha;
};

#endif // DIALOGOPTIONS_H
