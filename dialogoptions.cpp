/*
* ProtonClassicSuite
* 
* Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/

#include "dialogoptions.h"
#include "ui_dialogoptions.h"
#include "pcx_graphics.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"
#include "pcx_queryvariation.h"
#include "utils.h"
#include <QSettings>
#include <QDebug>


using namespace NUMBERSFORMAT;


DialogOptions::DialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);
    QSettings settings;


    ui->checkBoxRandomAllowed->setChecked(settings.value("misc/randomAllowed",true).toBool());

    QString css=settings.value("output/style","CSS").toString();
    if(css=="CSS")
        ui->radioButtonCSS->setChecked(true);
    else if(css=="INLINE")
        ui->radioButtonInline->setChecked(true);
    else
        qWarning()<<"Unsupported option for CSS !";

    QString imageFormat=settings.value("output/imageFormat","png").toString();
    if(imageFormat=="jpg")
        ui->radioButtonJPG->setChecked(true);
    else if(imageFormat=="png")
        ui->radioButtonPNG->setChecked(true);
    else if(imageFormat=="svg")
    {
        ui->radioButtonSVG->setChecked(true);
    }
    else
        qWarning()<<"Unsupported option for image format !";

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

    unsigned int numdecimals=settings.value("format/numdecimals",DEFAULTNUMDECIMALS).toUInt();
    ui->spinBoxNumDecimals->setValue(numdecimals);

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

    colorPen1=PCx_Graphics::getColorPen1();
    colorPen2=PCx_Graphics::getColorPen2();
    alpha=PCx_Graphics::getAlpha();

    ui->sliderAlpha->setValue(alpha);

    colorDFBar=PCx_Graphics::getColorDFBar();
    colorRFBar=PCx_Graphics::getColorRFBar();
    colorDIBar=PCx_Graphics::getColorDIBar();
    colorRIBar=PCx_Graphics::getColorRIBar();

    ui->pushButtonColorDF->setStyleSheet("background-color:"+colorDFBar.name());
    ui->pushButtonColorRF->setStyleSheet("background-color:"+colorRFBar.name());
    ui->pushButtonColorDI->setStyleSheet("background-color:"+colorDIBar.name());
    ui->pushButtonColorRI->setStyleSheet("background-color:"+colorRIBar.name());


    ui->pushButtonColorReqVar->setStyleSheet("background-color:"+colorReqVar.name());
    ui->pushButtonColorReqRank->setStyleSheet("background-color:"+colorReqRank.name());
    ui->pushButtonColorReqMinMax->setStyleSheet("background-color:"+colorReqMinMax.name());

    ui->pushButtonColorPen1->setStyleSheet("background-color:"+colorPen1.name());
    ui->pushButtonColorPen2->setStyleSheet("background-color:"+colorPen2.name());

    FORMATMODE formatMode=(FORMATMODE)settings.value("format/formatMode",(int)FORMATMODE::FORMATMODENORMAL).toUInt();
    switch(formatMode)
    {
    case FORMATMODE::FORMATMODENORMAL:
        ui->radioButtonUnits->setChecked(true);
        break;
    case FORMATMODE::FORMATMODETHOUSANDS:
        ui->radioButtonThousands->setChecked(true);
        break;
    case FORMATMODE::FORMATMODEMILLIONS:
        ui->radioButtonMillions->setChecked(true);
        break;
    }
}

DialogOptions::~DialogOptions()
{
    delete ui;
}

void DialogOptions::on_pushButtonOk_clicked()
{
    QSettings settings;

    if(ui->checkBoxRandomAllowed->isChecked())
        settings.setValue("misc/randomAllowed",true);
    else
        settings.setValue("misc/randomAllowed",false);

    if(ui->radioButtonCSS->isChecked())
        settings.setValue("output/style","CSS");
    else if(ui->radioButtonInline->isChecked())
        settings.setValue("output/style","INLINE");
    else
    {
        qWarning()<<"Unknown case of option selection for CSS";
    }

    if(ui->radioButtonJPG->isChecked())
        settings.setValue("output/imageFormat","jpg");
    else if(ui->radioButtonPNG->isChecked())
        settings.setValue("output/imageFormat","png");
    else if(ui->radioButtonSVG->isChecked())
        settings.setValue("output/imageFormat","svg");
    else
    {
        qWarning()<<"Unknown case of option selection for image format";
    }

    settings.setValue("format/numdecimals",ui->spinBoxNumDecimals->value());

    if(ui->radioButtonUnits->isChecked())
    {
        settings.setValue("format/formatMode",(int)FORMATMODE::FORMATMODENORMAL);
    }
    else if(ui->radioButtonThousands->isChecked())
    {
        settings.setValue("format/formatMode",(int)FORMATMODE::FORMATMODETHOUSANDS);
    }
    else if(ui->radioButtonMillions->isChecked())
    {
        settings.setValue("format/formatMode",(int)FORMATMODE::FORMATMODEMILLIONS);
    }

    settings.setValue("graphics/width",ui->spinBoxWidth->value());
    settings.setValue("graphics/height",ui->spinBoxHeight->value());
    settings.setValue("graphics/scale",ui->doubleSpinBoxScale->value());

    settings.setValue("graphics/pen1",colorPen1.rgba());
    settings.setValue("graphics/pen2",colorPen2.rgba());
    settings.setValue("graphics/alpha",alpha);
    settings.setValue("graphics/penDFBar",colorDFBar.rgba());
    settings.setValue("graphics/penRFBar",colorRFBar.rgba());
    settings.setValue("graphics/penDIBar",colorDIBar.rgba());
    settings.setValue("graphics/penRIBar",colorRIBar.rgba());

    settings.setValue("queries/penvar",colorReqVar.rgba());
    settings.setValue("queries/penrank",colorReqRank.rgba());
    settings.setValue("queries/penminmax",colorReqMinMax.rgba());

    updateFormatModeAndDecimals();

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

    colorPen1=PCx_Graphics::DEFAULTPENCOLOR1;
    colorPen2=PCx_Graphics::DEFAULTPENCOLOR2;
    alpha=PCx_Graphics::DEFAULTALPHA;

    colorDFBar=PCx_Graphics::DEFAULTCOLORDFBAR;
    colorRFBar=PCx_Graphics::DEFAULTCOLORRFBAR;
    colorDIBar=PCx_Graphics::DEFAULTCOLORDIBAR;
    colorRIBar=PCx_Graphics::DEFAULTCOLORRIBAR;

    ui->spinBoxNumDecimals->setValue(DEFAULTNUMDECIMALS);

    ui->radioButtonUnits->setChecked(true);

    ui->checkBoxRandomAllowed->setChecked(true);

    ui->pushButtonColorReqMinMax->setStyleSheet("background-color:"+colorReqMinMax.name());
    ui->pushButtonColorReqRank->setStyleSheet("background-color:"+colorReqRank.name());
    ui->pushButtonColorReqVar->setStyleSheet("background-color:"+colorReqVar.name());
    ui->pushButtonColorPen1->setStyleSheet("background-color:"+colorPen1.name());
    ui->pushButtonColorPen2->setStyleSheet("background-color:"+colorPen2.name());
    ui->sliderAlpha->setValue(alpha);
    ui->pushButtonColorDF->setStyleSheet("background-color:"+colorDFBar.name());
    ui->pushButtonColorRF->setStyleSheet("background-color:"+colorRFBar.name());
    ui->pushButtonColorDI->setStyleSheet("background-color:"+colorDIBar.name());
    ui->pushButtonColorRI->setStyleSheet("background-color:"+colorRIBar.name());
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

void DialogOptions::on_pushButtonColorPen1_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorPen1();
    QColor color = QColorDialog::getColor(oldcolor,this);
    if (color.isValid())
    {
        colorPen1=color;
        ui->pushButtonColorPen1->setStyleSheet("background-color:"+color.name());
    }
}

void DialogOptions::on_pushButtonColorPen2_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorPen2();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorPen2=color;
        ui->pushButtonColorPen2->setStyleSheet("background-color:"+color.name());
    }
}


void DialogOptions::on_sliderAlpha_valueChanged(int value)
{
    alpha=value;
}

void DialogOptions::on_pushButtonColorDF_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorDFBar();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorDFBar=color;
        ui->pushButtonColorDF->setStyleSheet("background-color:"+color.name());
    }
}


void DialogOptions::on_pushButtonColorRF_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorRFBar();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorRFBar=color;
        ui->pushButtonColorRF->setStyleSheet("background-color:"+color.name());
    }
}

void DialogOptions::on_pushButtonColorDI_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorDIBar();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorDIBar=color;
        ui->pushButtonColorDI->setStyleSheet("background-color:"+color.name());
    }
}

void DialogOptions::on_pushButtonColorRI_clicked()
{
    QColor oldcolor=PCx_Graphics::getColorRIBar();
    QColor color = QColorDialog::getColor(oldcolor, this);
    if (color.isValid())
    {
        colorRIBar=color;
        ui->pushButtonColorRI->setStyleSheet("background-color:"+color.name());
    }
}
