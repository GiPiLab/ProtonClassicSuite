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
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utils.h"
#include "pcx_reporting.h"
#include "tst_protonclassicsuiteunittests.h"


void ProtonClassicSuiteUnitTests::testCaseForReportingManagement()
{
    unsigned int treeId=PCx_Tree::createRandomTree("RANDOMTREE",20);
    PCx_Tree tree(treeId);
    tree.finishTree();

    QVERIFY(!PCx_Reporting::reportingNameExists("Toto"));
    QVERIFY(PCx_Reporting::getListOfReportings().isEmpty());
    unsigned int reportingId=PCx_Reporting::addNewReporting("Toto",treeId);
    QVERIFY(PCx_Reporting::reportingNameExists("Toto"));
    QList<QPair<unsigned int,QString>> listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> pair;
    bool contains=false;
    foreach(pair,listOfReportings)
    {
        if(pair.second.contains("Toto"))
            contains=true;
    }
    QVERIFY(contains);

    PCx_Reporting reporting(reportingId);
    QCOMPARE(reporting.getAttachedTreeId(),treeId);
    QCOMPARE(reporting.getReportingName(),QString("Toto"));

    PCx_Reporting::deleteReporting(reportingId);
    QVERIFY(!PCx_Reporting::reportingNameExists("Toto"));
    QVERIFY(PCx_Reporting::getListOfReportings().isEmpty());
    PCx_Tree::deleteTree(treeId);
}

void ProtonClassicSuiteUnitTests::testCaseForReportingNodes()
{
    unsigned int treeId=PCx_Tree::createRandomTree("RANDOMTREE",20);
    PCx_Tree tree(treeId);
    tree.finishTree();
    unsigned int reportingId=PCx_Reporting::addNewReporting("Toto",treeId);
    PCx_Reporting reporting(reportingId);

    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::DF),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::RF),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::DI),QDate());
    QCOMPARE(reporting.getLastReportingDate(MODES::DFRFDIRI::RI),QDate());

    unsigned int firstLeafId=tree.getLeavesId().constFirst();
    unsigned int lastLeafId=tree.getLeavesId().constLast();


    for(int i=0;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
    {
        if(i==(int)PCx_Reporting::OREDPCR::DISPONIBLES)
        {
            continue;
        }
        reporting.setLeafValues(firstLeafId,MODES::DFRFDIRI::DF,QDate(2015,01,01),QMap<PCx_Reporting::OREDPCR,double>{{(PCx_Reporting::OREDPCR)i,10.0}});
        reporting.setLeafValues(lastLeafId,MODES::DFRFDIRI::DF,QDate(2015,01,01),QMap<PCx_Reporting::OREDPCR,double>{{(PCx_Reporting::OREDPCR)i,20.0}});
        QCOMPARE(reporting.getNodeValue(firstLeafId,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(10.0));
        QCOMPARE(reporting.getNodeValue(lastLeafId,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(20.0));
    }

    for(int i=0;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
    {
        if(i==(int)PCx_Reporting::OREDPCR::DISPONIBLES)
        {
            continue;
        }
        QCOMPARE(reporting.getNodeValue(1,MODES::DFRFDIRI::DF,(PCx_Reporting::OREDPCR)i,QDate(2015,01,01)),NUMBERSFORMAT::doubleToFixedPoint(30.0));
    }

    PCx_Reporting::deleteReporting(reportingId);
    PCx_Tree::deleteTree(treeId);
}
