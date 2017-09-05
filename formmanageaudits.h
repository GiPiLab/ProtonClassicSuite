/*
* ProtonClassicSuite
* 
* Copyright Thibault Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
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

#ifndef FORMMANAGEAUDITS_H
#define FORMMANAGEAUDITS_H

#include<QWidget>
#include "pcx_audit.h"

namespace Ui {
class FormManageAudits;
}

class FormManageAudits : public QWidget
{
    Q_OBJECT

public:
    explicit FormManageAudits(QWidget *parent = 0);
    ~FormManageAudits();

public slots:
    void onLOTchanged();
    void onListOfAuditsChanged();
    void updateRandomButtonVisibility();



private slots:
    void on_addAuditButton_clicked();

    void on_comboListOfAudits_activated(int index);

    void on_deleteAuditButton_clicked();

    void on_finishAuditButton_clicked();

    void on_cloneAuditButton_clicked();

    void on_pushButtonDisplayTree_clicked();

    void on_statisticsAuditButton_clicked();

    void on_pushButtonRandomDF_clicked();

    void on_pushButtonRandomRF_clicked();

    void on_pushButtonRandomDI_clicked();

    void on_pushButtonRandomRI_clicked();

    void on_pushButtonClearDF_clicked();

    void on_pushButtonClearRF_clicked();

    void on_pushButtonClearDI_clicked();

    void on_pushButtonClearRI_clicked();

    void on_pushButtonLoadDF_clicked();

    void on_pushButtonLoadRF_clicked();

    void on_pushButtonLoadDI_clicked();

    void on_pushButtonLoadRI_clicked();

    void on_pushButtonExportDF_clicked();

    void on_pushButtonExportRF_clicked();

    void on_pushButtonExportDI_clicked();

    void on_pushButtonExportRI_clicked();

    void on_pushButtonSkel_clicked();

signals:
    void listOfAuditsChanged();
    void auditDataUpdated(unsigned int auditId);

private:
    Ui::FormManageAudits *ui;
    void updateListOfTrees();
    void updateListOfAudits();
    PCx_Audit *selectedAudit;
    bool exportLeaves(MODES::DFRFDIRI mode);
    bool importLeaves(MODES::DFRFDIRI mode);
    void updateButtonsVisibility();
};

#endif // FORMMANAGEAUDITS_H
