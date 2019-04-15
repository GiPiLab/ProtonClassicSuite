/*
 * ProtonClassicSuite
 *
 * Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le
 * Développement Local (2006--)
 *
 * labo@gipilab.org
 *
 * Ce logiciel est un programme informatique servant à cerner l'ensemble des
 * données budgétaires de la collectivité territoriale (audit, reporting
 * infra-annuel, prévision des dépenses à venir)
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
#include "pcx_graphics.h"
#include "pcx_queryminmax.h"
#include "pcx_queryrank.h"
#include "pcx_queryvariation.h"
#include "ui_dialogoptions.h"
#include "utils.h"
#include <QDebug>
#include <QSettings>

using namespace NUMBERSFORMAT;

DialogOptions::DialogOptions(QWidget *parent) : QDialog(parent), ui(new Ui::DialogOptions) {
  ui->setupUi(this);
  QSettings settings;

  ui->checkBoxRandomAllowed->setChecked(settings.value("misc/randomAllowed", true).toBool());

  QString css = settings.value("output/style", "CSS").toString();
  if (css == "CSS") {
    ui->radioButtonCSS->setChecked(true);
  } else if (css == "INLINE") {
    ui->radioButtonInline->setChecked(true);
  } else {
    qWarning() << "Unsupported option for CSS !";
  }

  int imageWidth =
      settings.value(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::WIDTH), PCx_Graphics::DEFAULTWIDTH)
          .toInt();
  if (imageWidth < PCx_Graphics::MINWIDTH) {
    imageWidth = PCx_Graphics::MINWIDTH;
  }
  if (imageWidth > PCx_Graphics::MAXWIDTH) {
    imageWidth = PCx_Graphics::MAXWIDTH;
  }

  int imageHeight =
      settings.value(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::HEIGHT), PCx_Graphics::DEFAULTHEIGHT)
          .toInt();
  if (imageHeight < PCx_Graphics::MINHEIGHT) {
    imageHeight = PCx_Graphics::MINHEIGHT;
  }
  if (imageHeight > PCx_Graphics::MAXHEIGHT) {
    imageHeight = PCx_Graphics::MAXHEIGHT;
  }

  unsigned int numdecimals = settings.value("format/numdecimals", DEFAULTNUMDECIMALS).toUInt();
  ui->spinBoxNumDecimals->setValue(static_cast<int>(numdecimals));

  ui->spinBoxHeight->setMinimum(PCx_Graphics::MINHEIGHT);
  ui->spinBoxHeight->setMaximum(PCx_Graphics::MAXHEIGHT);
  ui->spinBoxHeight->setValue(imageHeight);

  ui->spinBoxWidth->setMinimum(PCx_Graphics::MINWIDTH);
  ui->spinBoxWidth->setMaximum(PCx_Graphics::MAXWIDTH);
  ui->spinBoxWidth->setValue(imageWidth);

  colorReqVar = PCx_QueryVariation::getColor();
  colorReqRank = PCx_QueryRank::getColor();
  colorReqMinMax = PCx_QueryMinMax::getColor();

  colorPen1 = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::PENCOLOR1).toUInt();
  colorPen2 = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::PENCOLOR2).toUInt();
  alpha = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::ALPHA).toInt();

  bool showPointLabels = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::SHOWPOINTLABELS).toBool();
  ui->checkBoxShowPointLabels->setChecked(showPointLabels);

  ui->sliderAlpha->setValue(alpha);

  colorDFBar = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::DFBARCOLOR).toUInt();
  colorRFBar = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::RFBARCOLOR).toUInt();
  colorDIBar = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::DIBARCOLOR).toUInt();
  colorRIBar = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::RIBARCOLOR).toUInt();

  ui->pushButtonColorDF->setStyleSheet("background-color:" + colorDFBar.name());
  ui->pushButtonColorRF->setStyleSheet("background-color:" + colorRFBar.name());
  ui->pushButtonColorDI->setStyleSheet("background-color:" + colorDIBar.name());
  ui->pushButtonColorRI->setStyleSheet("background-color:" + colorRIBar.name());

  ui->pushButtonColorReqVar->setStyleSheet("background-color:" + colorReqVar.name());
  ui->pushButtonColorReqRank->setStyleSheet("background-color:" + colorReqRank.name());
  ui->pushButtonColorReqMinMax->setStyleSheet("background-color:" + colorReqMinMax.name());

  ui->pushButtonColorPen1->setStyleSheet("background-color:" + colorPen1.name());
  ui->pushButtonColorPen2->setStyleSheet("background-color:" + colorPen2.name());

  FORMATMODE formatMode = static_cast<FORMATMODE>(
      settings.value("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODENORMAL)).toUInt());
  switch (formatMode) {
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

DialogOptions::~DialogOptions() { delete ui; }

void DialogOptions::on_pushButtonOk_clicked() {
  QSettings settings;

  if (ui->checkBoxRandomAllowed->isChecked()) {
    settings.setValue("misc/randomAllowed", true);
  } else {
    settings.setValue("misc/randomAllowed", false);
  }

  if (ui->radioButtonCSS->isChecked()) {
    settings.setValue("output/style", "CSS");
  } else if (ui->radioButtonInline->isChecked()) {
    settings.setValue("output/style", "INLINE");
  } else {
    qWarning() << "Unknown case of option selection for CSS";
  }

  settings.setValue("format/numdecimals", ui->spinBoxNumDecimals->value());

  if (ui->radioButtonUnits->isChecked()) {
    settings.setValue("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODENORMAL));
  } else if (ui->radioButtonThousands->isChecked()) {
    settings.setValue("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODETHOUSANDS));
  } else if (ui->radioButtonMillions->isChecked()) {
    settings.setValue("format/formatMode", static_cast<int>(FORMATMODE::FORMATMODEMILLIONS));
  }

  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::WIDTH), ui->spinBoxWidth->value());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::HEIGHT), ui->spinBoxHeight->value());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::PENCOLOR1), colorPen1.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::PENCOLOR2), colorPen2.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::ALPHA), alpha);
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::DFBARCOLOR), colorDFBar.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::RFBARCOLOR), colorRFBar.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::DIBARCOLOR), colorDIBar.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::RIBARCOLOR), colorRIBar.rgba());
  settings.setValue(PCx_Graphics::settingKey().value(PCx_Graphics::SETTINGKEY::SHOWPOINTLABELS),
                    ui->checkBoxShowPointLabels->isChecked());

  settings.setValue("queries/penvar", colorReqVar.rgba());
  settings.setValue("queries/penrank", colorReqRank.rgba());
  settings.setValue("queries/penminmax", colorReqMinMax.rgba());

  updateFormatModeAndDecimals();

  done(Accepted);
}

void DialogOptions::on_pushButtonCancel_clicked() { done(Rejected); }

void DialogOptions::on_pushButtonReset_clicked() {
  ui->radioButtonCSS->setChecked(true);
  ui->spinBoxHeight->setValue(PCx_Graphics::DEFAULTHEIGHT);
  ui->spinBoxWidth->setValue(PCx_Graphics::DEFAULTWIDTH);
  colorReqMinMax = QColor(PCx_QueryMinMax::DEFAULTCOlOR);
  colorReqVar = QColor(PCx_QueryVariation::DEFAULTCOlOR);
  colorReqRank = QColor(PCx_QueryRank::DEFAULTCOlOR);

  colorPen1 = PCx_Graphics::DEFAULTPENCOLOR1;
  colorPen2 = PCx_Graphics::DEFAULTPENCOLOR2;
  alpha = PCx_Graphics::DEFAULTALPHA;

  ui->checkBoxShowPointLabels->setChecked(false);

  colorDFBar = PCx_Graphics::DEFAULTCOLORDFBAR;
  colorRFBar = PCx_Graphics::DEFAULTCOLORRFBAR;
  colorDIBar = PCx_Graphics::DEFAULTCOLORDIBAR;
  colorRIBar = PCx_Graphics::DEFAULTCOLORRIBAR;

  ui->spinBoxNumDecimals->setValue(DEFAULTNUMDECIMALS);

  ui->radioButtonUnits->setChecked(true);

  ui->checkBoxRandomAllowed->setChecked(true);

  ui->pushButtonColorReqMinMax->setStyleSheet("background-color:" + colorReqMinMax.name());
  ui->pushButtonColorReqRank->setStyleSheet("background-color:" + colorReqRank.name());
  ui->pushButtonColorReqVar->setStyleSheet("background-color:" + colorReqVar.name());
  ui->pushButtonColorPen1->setStyleSheet("background-color:" + colorPen1.name());
  ui->pushButtonColorPen2->setStyleSheet("background-color:" + colorPen2.name());
  ui->sliderAlpha->setValue(alpha);
  ui->pushButtonColorDF->setStyleSheet("background-color:" + colorDFBar.name());
  ui->pushButtonColorRF->setStyleSheet("background-color:" + colorRFBar.name());
  ui->pushButtonColorDI->setStyleSheet("background-color:" + colorDIBar.name());
  ui->pushButtonColorRI->setStyleSheet("background-color:" + colorRIBar.name());
}

void DialogOptions::on_pushButtonColorReqVar_clicked() {
  QColor oldcolor = PCx_QueryVariation::getColor();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorReqVar = color;
    ui->pushButtonColorReqVar->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorReqRank_clicked() {
  QColor oldcolor = PCx_QueryRank::getColor();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorReqRank = color;
    ui->pushButtonColorReqRank->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorReqMinMax_clicked() {
  QColor oldcolor = PCx_QueryMinMax::getColor();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorReqMinMax = color;
    ui->pushButtonColorReqMinMax->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorPen1_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::PENCOLOR1).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorPen1 = color;
    ui->pushButtonColorPen1->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorPen2_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::PENCOLOR2).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorPen2 = color;
    ui->pushButtonColorPen2->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_sliderAlpha_valueChanged(int value) { alpha = value; }

void DialogOptions::on_pushButtonColorDF_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::DFBARCOLOR).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorDFBar = color;
    ui->pushButtonColorDF->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorRF_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::RFBARCOLOR).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorRFBar = color;
    ui->pushButtonColorRF->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorDI_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::DIBARCOLOR).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorDIBar = color;
    ui->pushButtonColorDI->setStyleSheet("background-color:" + color.name());
  }
}

void DialogOptions::on_pushButtonColorRI_clicked() {
  QColor oldcolor = PCx_Graphics::getSettingValue(PCx_Graphics::SETTINGKEY::RIBARCOLOR).toUInt();
  QColor color = QColorDialog::getColor(oldcolor, this);
  if (color.isValid()) {
    colorRIBar = color;
    ui->pushButtonColorRI->setStyleSheet("background-color:" + color.name());
  }
}
