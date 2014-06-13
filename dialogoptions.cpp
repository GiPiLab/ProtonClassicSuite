#include "dialogoptions.h"
#include "ui_dialogoptions.h"
#include <QtGui>


DialogOptions::DialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);
    QSettings settings;
    QString css=settings.value("output/style","CSS").toString();
    if(css=="CSS")
        ui->radioButtonCSS->setChecked(true);
    else if(css=="INLINE")
        ui->radioButtonInline->setChecked(true);
    else
        qCritical()<<"Unsupported option for CSS !";

    QString imageFormat=settings.value("output/imageFormat","png").toString();
    if(imageFormat=="jpg")
        ui->radioButtonJPG->setChecked(true);
    else if(imageFormat=="png")
        ui->radioButtonPNG->setChecked(true);
    else
        qCritical()<<"Unsupported option for image format !";
}

DialogOptions::~DialogOptions()
{
    delete ui;
}

void DialogOptions::on_pushButtonOk_clicked()
{
    QSettings settings;
    if(ui->radioButtonCSS->isChecked())
        settings.setValue("output/style","CSS");
    else if(ui->radioButtonInline->isChecked())
        settings.setValue("output/style","INLINE");
    else
    {
        qCritical()<<"Unknown case of option selection for CSS";
    }

    if(ui->radioButtonJPG->isChecked())
        settings.setValue("output/imageFormat","jpg");
    else if(ui->radioButtonPNG->isChecked())
        settings.setValue("output/imageFormat","png");
    else
    {
        qCritical()<<"Unknown case of option selection for image format";
    }

    done(Accepted);
}


void DialogOptions::on_pushButtonCancel_clicked()
{
    done(Rejected);
}
