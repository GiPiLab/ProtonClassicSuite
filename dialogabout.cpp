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

#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "utils.h"
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

DialogAbout::DialogAbout(QWidget *parent) : QDialog(parent), ui(new Ui::DialogAbout) {
  ui->setupUi(this);
  ui->label->setText(QString("ProtonClassicSuite version %1 (build %2)<br>©2006-%3 "
                             "Laboratoire de "
                             "Recherche pour le Développement Local<br><a "
                             "href='https://gipilab.org'>gipilab.org</a> - <a "
                             "href='https://protonclassic.com'>protonclassic.com</a>")
                         .arg(VERSION)
                         .arg(__DATE__)
                         .arg(QDate::currentDate().year()));
  QFile eulaFile(":/EULA/EULA.txt");
  eulaFile.open(QIODevice::ReadOnly);
  QTextStream in(&eulaFile);  
  QString line = in.readAll();
  eulaFile.close();
  ui->plainTextEdit->setPlainText(line);
}

DialogAbout::~DialogAbout() { delete ui; }

void DialogAbout::on_pushButton_clicked() { accept(); }
