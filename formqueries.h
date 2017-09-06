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

#ifndef FORMQUERIES_H
#define FORMQUERIES_H

#include "pcx_audit.h"
#include "pcx_report.h"
#include "pcx_queryvariation.h"
#include "pcx_queriesmodel.h"
#include "pcx_queryrank.h"
#include "xlsxdocument.h"
#include <QWidget>
#include <QTextDocument>

namespace Ui {
class FormQueries;
}

class FormQueries : public QWidget
{
    Q_OBJECT

signals:
    void listOfQueriesChanged(unsigned int auditId);

public:
    explicit FormQueries(QWidget *parent = 0);
    ~FormQueries();

public slots:
    void onListOfAuditsChanged();
    void onColorChanged();
    void onListOfQueriesChanged(unsigned int auditId);

private slots:
    void on_comboBoxListAudits_activated(int index);

    void on_pushButtonExecReqVariation_clicked();

    void on_comboBoxAugDim_activated(int index);

    void on_pushButtonSaveReqVariation_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonExecFromList_clicked();

    void on_listView_activated(const QModelIndex &index);

    void on_pushButtonSave_clicked();

    void on_pushButtonExecReqRank_clicked();

    void on_pushButtonSaveReqRank_clicked();

    void on_pushButtonExecReq3_clicked();

    void on_pushButtonSaveReq3_clicked();

    void on_pushButtonSaveXLSX_clicked();

private:
    Ui::FormQueries *ui;
    void updateListOfAudits();
    PCx_Audit *model;
    PCx_Report *report;
    PCx_QueriesModel *queriesModel;
    QTextDocument *doc;
    QString currentHtmlDoc;

    QXlsx::Document *xlsDoc;

    bool getParamsReqVariation(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri,
                       PCx_QueryVariation::INCREASEDECREASE &increase, PCx_QueryVariation::PERCENTORPOINTS &percent,
                       PCx_QueryVariation::OPERATORS &oper, qint64 &val, unsigned int &year1, unsigned int &year2);
    QString execQueries(QModelIndexList items,QXlsx::Document *xlsDocument=nullptr);
    bool getParamsReqRank(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri, unsigned int &number, PCx_QueryRank::GREATERSMALLER &grSm, unsigned int &year1, unsigned int &year2);
    bool getParamsReqMinMax(unsigned int &typeId, PCx_Audit::ORED &ored, MODES::DFRFDIRI &dfrfdiri, qint64 &val1, qint64 &val2, unsigned int &year1, unsigned int &year2);
};

#endif // FORMQUERIES_H
