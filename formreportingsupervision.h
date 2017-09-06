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

#ifndef FORMREPORTINGSUPERVISION_H
#define FORMREPORTINGSUPERVISION_H

#include "pcx_reporting.h"
#include "pcx_reportingtablesupervisionmodel.h"
#include <QSortFilterProxyModel>
#include <QWidget>
#include <QTimer>

namespace Ui {
class FormReportingSupervision;
}

class FormReportingSupervision : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportingSupervision(QWidget *parent = 0);
    ~FormReportingSupervision();

public slots:
    void onListOfReportingsChanged();
    void onReportingDataChanged(unsigned int reportingId);

private slots:
    void on_comboListReportings_activated(int index);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_checkBoxBP_toggled(bool checked);

    void on_checkBoxOuverts_toggled(bool checked);

    void on_checkBoxRealises_toggled(bool checked);

    void on_checkBoxEngages_toggled(bool checked);

    void on_checkBoxDisponibles_toggled(bool checked);

    void on_checkBoxTauxReal_toggled(bool checked);

    void on_checkBoxTauxEcart_toggled(bool checked);

    void on_checkBoxVariationBP_toggled(bool checked);

    void on_checkBoxTauxEngage_toggled(bool checked);

    void on_checkBoxTauxDisponible_toggled(bool checked);

    void on_pushButtonExportHTML_clicked();

    void on_checkBoxRealisePredit_toggled(bool checked);

    void on_checkBoxDifference_toggled(bool checked);

    void on_checkBoxDECICO_toggled(bool checked);

    void on_checkBoxERO2_toggled(bool checked);

    void on_checkBoxRAC_toggled(bool checked);

    void on_checkBox15NRest_toggled(bool checked);

    void on_checkBoxCPP15N_toggled(bool checked);

    void on_pushButtonSelectNone_clicked();

    void on_pushButtonSelectAll_clicked();

    void on_comboBoxListDates_activated(int index);

    void on_pushButtonCopy_clicked();

    void on_pushButtonClear_clicked();



private:
    Ui::FormReportingSupervision *ui;
    QSize sizeHint()const;
    void updateDateRefCombo();

    PCx_Reporting *selectedReporting;
    PCx_ReportingTableSupervisionModel *model;
    QSortFilterProxyModel *proxyModel;
    void updateListOfReportings();
    MODES::DFRFDIRI getSelectedMode() const;
    void setColumnVisibility();
};

#endif // FORMREPORTINGSUPERVISION_H
