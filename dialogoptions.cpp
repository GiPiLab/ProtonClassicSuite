#include "dialogoptions.h"
#include "ui_dialogoptions.h"
#include "pcx_graphics.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"
#include "pcx_queryvariation.h"

#include <QSettings>
#include <QDebug>

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

    int imageWidth=settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt();
    if(imageWidth<PCx_Graphics::MINWIDTH)
        imageWidth=PCx_Graphics::MINWIDTH;
    if(imageWidth>PCx_Graphics::MAXWIDTH)
        imageWidth=PCx_Graphics::MAXWIDTH;


    int imageHeight=settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt();
    if(imageHeight<PCx_Graphics::MINHEIGHT)
        imageHeight=PCx_Graphics::MINHEIGHT;
    if(imageHeight>PCx_Graphics::MAXHEIGHT)
        imageHeight=PCx_Graphics::MAXHEIGHT;


    double scale=settings.value("graphics/scale",PCx_Graphics::DEFAULTSCALE).toDouble();
    if(scale<PCx_Graphics::MINSCALE)
        scale=PCx_Graphics::MINSCALE;
    if(scale>PCx_Graphics::MAXSCALE)
        scale=PCx_Graphics::MAXSCALE;

    ui->doubleSpinBoxScale->setMinimum(PCx_Graphics::MINSCALE);
    ui->doubleSpinBoxScale->setMaximum(PCx_Graphics::MAXSCALE);
    ui->doubleSpinBoxScale->setValue(scale);

    ui->spinBoxHeight->setMinimum(PCx_Graphics::MINHEIGHT);
    ui->spinBoxHeight->setMaximum(PCx_Graphics::MAXHEIGHT);
    ui->spinBoxHeight->setValue(imageHeight);

    ui->spinBoxWidth->setMinimum(PCx_Graphics::MINWIDTH);
    ui->spinBoxWidth->setMaximum(PCx_Graphics::MAXWIDTH);
    ui->spinBoxWidth->setValue(imageWidth);

    colorReqVar=PCx_QueryVariation::getColor();
    colorReqRank=PCx_QueryRank::getColor();
    colorReqMinMax=PCx_QueryMinMax::getColor();

    ui->pushButtonColorReqVar->setStyleSheet("background-color:"+colorReqVar.name());
    ui->pushButtonColorReqRank->setStyleSheet("background-color:"+colorReqRank.name());
    ui->pushButtonColorReqMinMax->setStyleSheet("background-color:"+colorReqMinMax.name());

    //FIXME : PRECISION
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

    settings.setValue("graphics/width",ui->spinBoxWidth->value());
    settings.setValue("graphics/height",ui->spinBoxHeight->value());
    settings.setValue("graphics/scale",ui->doubleSpinBoxScale->value());

    settings.setValue("colors/reqvar",colorReqVar.rgba());
    settings.setValue("colors/reqrank",colorReqRank.rgba());
    settings.setValue("colors/reqminmax",colorReqMinMax.rgba());

    done(Accepted);
}


void DialogOptions::on_pushButtonCancel_clicked()
{
    done(Rejected);
}


void DialogOptions::on_pushButtonReset_clicked()
{
    ui->radioButtonCSS->setChecked(true);
    ui->radioButtonPNG->setChecked(true);
    ui->spinBoxHeight->setValue(PCx_Graphics::DEFAULTHEIGHT);
    ui->spinBoxWidth->setValue(PCx_Graphics::DEFAULTWIDTH);
    ui->doubleSpinBoxScale->setValue(PCx_Graphics::DEFAULTSCALE);
    colorReqMinMax=QColor(PCx_QueryMinMax::DEFAULTCOlOR);
    colorReqVar=QColor(PCx_QueryVariation::DEFAULTCOlOR);
    colorReqRank=QColor(PCx_QueryRank::DEFAULTCOlOR);

    ui->pushButtonColorReqMinMax->setStyleSheet("background-color:"+colorReqMinMax.name());
    ui->pushButtonColorReqRank->setStyleSheet("background-color:"+colorReqRank.name());
    ui->pushButtonColorReqVar->setStyleSheet("background-color:"+colorReqVar.name());

    //FIXME : Precision
}

void DialogOptions::on_pushButtonColorReqVar_clicked()
{
    QColor oldcolor=PCx_QueryVariation::getColor();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorReqVar=color;
        ui->pushButtonColorReqVar->setStyleSheet("background-color:"+color.name());
    }
}

void DialogOptions::on_pushButtonColorReqRank_clicked()
{
    QColor oldcolor=PCx_QueryRank::getColor();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorReqRank=color;
        ui->pushButtonColorReqRank->setStyleSheet("background-color:"+color.name());
    }

}

void DialogOptions::on_pushButtonColorReqMinMax_clicked()
{
    QColor oldcolor=PCx_QueryMinMax::getColor();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorReqMinMax=color;
        ui->pushButtonColorReqMinMax->setStyleSheet("background-color:"+color.name());
    }
}
