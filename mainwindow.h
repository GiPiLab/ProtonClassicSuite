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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "formmanageprevisions.h"
#include "formauditprevisions.h"
#include "formreportingreports.h"
#include "formreportingexplore.h"
#include "formreportingsupervision.h"
#include "formreportinggraphics.h"
#include "formmanagereportings.h"
#include "formmanageaudits.h"
#include "formreportingoverview.h"
#include "formedittree.h"
#include "formeditaudit.h"
#include "formauditexplore.h"
#include "formauditreports.h"
#include "dialogoptions.h"
#include "formqueries.h"
#include "formaudittreemap.h"
#include "utils.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_actionManageTree_triggered();
    void on_actionExit_triggered();
    void onFormEditTreeWindowsDestroyed();
    void onFormManageAuditsWindowsDestroyed();
    void onFormManageReportingsWindowsDestroyed();
    void onFormEditAuditWindowsDestroyed();
    void onFormAuditExploreWindowsDestroyed(QObject *);
    void onFormQueriesWindowsDestroyed(QObject *obj);
    void on_actionManageAudits_triggered();
    void on_actionEditAudit_triggered();
    void on_actionNewDb_triggered();
    void on_actionOpenDb_triggered();
    void on_actionAuditReport_triggered();
    void onFormReportsWindowsDestroyed();
    void onFormAuditPrevisionsWindowsDestroyed();
    void onFormReportingReportsWindowsDestroyed();
    void on_actionOptions_triggered();
    void on_actionQueries_triggered();
    void on_actionAbout_triggered();
    void on_actionManageReportings_triggered();
    void on_actionReportingOverview_triggered();
    void onFormReportingTablesWindowsDestroyed(QObject *obj);
    void onFormReportingSupervisionWindowsDestroyed(QObject *obj);
    void onFormReportingGraphicsWindowsDestroyed(QObject *obj);
    void onFormReportingExploreWindowsDestroyed(QObject *obj);
    void on_actionReportingSupervision_triggered();
    void on_actionReportingGraphics_triggered();
    void on_actionExploreReportings_triggered();
    void on_actionReportingReport_triggered();
    void on_actionExploreAudits_triggered();
    void on_actionCascade_triggered();
    void on_actionCloseAllSubwin_triggered();
    void on_actionBudgetElaboration_triggered();
    void on_actionManagePrevisions_triggered();
    void onFormManagePrevisionsWindowsDestroyed();
    void on_actionTreemap_triggered();
    void onFormAuditTreemapWindowsDestroyed(QObject *obj);

private:

    Ui::MainWindow *ui;

    //Only one instance of these dialogs is allowed at once
    FormEditTree *formEditTreeWin;
    FormManageAudits *formManageAudits;
    FormEditAudit *formEditAudit;
    FormAuditReports *formAuditReports;
    FormReportingReports *formReportingReports;
    FormManageReportings *formManageReportings;
    FormAuditPrevisions *formAuditPrevisions;
    FormManagePrevisions *formManagePrevisions;



    //Multiple "tables" dialogs are allowed
    QList<FormAuditExplore *>listOfFormAuditExplore;
    QList<FormReportingOverview *>listOfFormReportingOverview;
    QList<FormReportingSupervision *>listOfFormReportingSupervision;
    QList<FormReportingGraphics *>listOfFormReportingGraphics;
    QList<FormReportingExplore *>listOfFormReportingExplore;
    QList<FormAuditTreemap *>listOfFormAuditTreemap;

    QList<FormQueries *>listOfFormQueries;
    QString recentDb;

    void updateTitle();
    void setMenusState();
    void saveSettings();
    void restoreSettings();

    void desactivateActions();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
