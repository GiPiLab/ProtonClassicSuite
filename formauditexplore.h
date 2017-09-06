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

#ifndef FORMAUDITEXPLORE_H
#define FORMAUDITEXPLORE_H

#include <QWidget>
#include "pcx_auditwithtreemodel.h"
#include "pcx_report.h"

namespace Ui {
class FormAuditExplore;
}

class FormAuditExplore : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditExplore(QWidget *parent = 0);
    ~FormAuditExplore();

public slots:
    void onListOfAuditsChanged();
    void onSettingsChanged();

private slots:

    void on_comboListAudits_activated(int index);
    void on_treeView_clicked(const QModelIndex &index);
    void on_radioButtonGlobal_toggled(bool checked);
    void on_checkBoxPoidsRelatif_toggled(bool checked);
    void on_radioButtonDF_toggled(bool checked);
    void on_radioButtonRF_toggled(bool checked);
    void on_radioButtonDI_toggled(bool checked);
    void on_radioButtonRI_toggled(bool checked);
    void on_checkBoxEvolution_toggled(bool checked);
    void on_checkBoxEvolutionCumul_toggled(bool checked);
    void on_checkBoxBase100_toggled(bool checked);
    void on_checkBoxJoursAct_toggled(bool checked);
    //void on_printButton_clicked();
    void on_saveButton_clicked();
    void on_checkBoxResults_toggled(bool checked);
    void on_checkBoxRecapGraph_toggled(bool checked);
    void on_checkBoxOuvert_toggled(bool checked);
    void on_checkBoxEngage_toggled(bool checked);
    void on_checkBoxOuvertCumul_toggled(bool checked);
    void on_checkBoxEngageCumul_toggled(bool checked);
    void on_checkBoxRealise_toggled(bool checked);
    void on_checkBoxDisponible_toggled(bool checked);
    void on_checkBoxRealiseCumul_toggled(bool checked);
    void on_checkBoxDisponibleCumul_toggled(bool checked);

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonExpandAll_clicked();



    void on_checkBoxRawHistoryData_toggled(bool checked);


    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::FormAuditExplore *ui;
    void updateListOfAudits();
    void updateTextBrowser();
    void getSelections();

    QTextDocument *doc;
    PCx_AuditWithTreeModel *model;
    PCx_Report *report;

    QList<PCx_Tables::PCAPRESETS> selectedTabs;
    QList<PCx_Graphics::PCAGRAPHICS> selectedGraphics;
    MODES::DFRFDIRI selectedMode;
    unsigned int referenceNode;

    bool ready;
};

#endif // FORMAUDITEXPLORE_H
