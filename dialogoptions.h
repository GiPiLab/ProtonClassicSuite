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

#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include <QDialog>

namespace Ui {
class DialogOptions;
}

class DialogOptions : public QDialog {
  Q_OBJECT

public:
  explicit DialogOptions(QWidget *parent = nullptr);
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
  QColor colorReqVar, colorReqMinMax, colorReqRank, colorPen1, colorPen2, colorDFBar, colorRFBar, colorDIBar,
      colorRIBar;
  int alpha;
};

#endif // DIALOGOPTIONS_H
