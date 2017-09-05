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

#include "utils.h"
#include "pcx_query.h"
#include "pcx_queryvariation.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>

PCx_Query::PCx_Query(PCx_Audit *model):model(model),queryId(0)
{
    dfrfdiri=MODES::DFRFDIRI::GLOBAL;
    ored=PCx_Audit::ORED::OUVERTS;
    typeId=0;
    year1=0;
    year2=0;
}

PCx_Query::PCx_Query(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri,
                         unsigned int year1, unsigned int year2, const QString &name):model(model),typeId(typeId),ored(ored),dfrfdiri(dfrfdiri),
                        name(name),queryId(0)
{
    setYears(year1,year2);
}

void PCx_Query::setYears(unsigned int year1, unsigned int year2)
{
    if(year1>year2)
    {
        unsigned int yeartmp;
        yeartmp=year1;
        year1=year2;
        year2=yeartmp;
    }
    unsigned int firstYear=model->getYears().constFirst();
    unsigned int lastYear=model->getYears().constLast();
    if(year1<firstYear)
    {
        year1=firstYear;
    }

    if(year2>lastYear)
    {
        year2=lastYear;
    }
    this->year1=year1;
    this->year2=year2;
}

bool PCx_Query::remove(unsigned int queryId)
{
    return deleteQuery(model->getAuditId(),queryId);
}

void PCx_Query::createTableQueries(unsigned int auditId)
{
    QSqlQuery q;
    if(!q.exec(QString("create table audit_queries_%1(id integer primary key autoincrement, name text not null, "
                   "query_mode integer not null, target_type integer check (target_type>=0) not null, "
                   "ored integer not null, dfrfdiri integer not null, oper integer, "
                   "percent_or_point integer, increase_decrease integer,val1 integer not null default '0', val2 integer,"
                   "year1 integer not null, year2 integer not null)").arg(auditId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()==-1)
    {
        qCritical()<<q.lastError();
        die();
    }
}

QString PCx_Query::getCSS()
{
    QString css=PCx_QueryVariation::getCSS()+PCx_QueryRank::getCSS()+PCx_QueryMinMax::getCSS();
    return css;
}


QList<unsigned int> PCx_Query::getListOfQueriesId(unsigned int auditId,QUERIESTYPES type)
{
    if(!PCx_Audit::auditIdExists(auditId))
    {
        qCritical()<<"Non existant audit ID";
        die();
    }
    QList<unsigned int> listOfQueries;

    QSqlQuery query;

    if(!query.exec(QString("select id from audit_queries_%1 where query_mode=%2").arg(auditId).arg((unsigned int)type)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        listOfQueries.append(query.value("id").toUInt());
    }
    return listOfQueries;
}

QList<unsigned int> PCx_Query::getListOfQueriesId(unsigned int auditId)
{
    if(!PCx_Audit::auditIdExists(auditId))
    {
        qCritical()<<"Non existant audit ID";
        die();
    }
    QList<unsigned int> listOfQueries;

    QSqlQuery query;

    if(!query.exec(QString("select id from audit_queries_%1").arg(auditId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        listOfQueries.append(query.value("id").toUInt());
    }
    return listOfQueries;
}

bool PCx_Query::deleteQuery(unsigned int auditId,unsigned int queryId)
{
    if(queryId==0 || auditId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("delete from audit_queries_%1 where id=:id").arg(auditId));
    q.bindValue(":id",queryId);
    q.exec();

    if(q.numRowsAffected()==0)
    {
        qCritical()<<q.lastError();
        return false;
    }
    return true;
}
