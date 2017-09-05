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

#include "pcx_queryminmax.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include "utils.h"


using namespace NUMBERSFORMAT;

PCx_QueryMinMax::PCx_QueryMinMax(PCx_Audit *model, unsigned int queryId):PCx_Query(model)
{
    load(queryId);
}

PCx_QueryMinMax::PCx_QueryMinMax(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri, qint64 val1,
                             qint64 val2, unsigned int year1, unsigned int year2, const QString &name):PCx_Query(model,typeId,ored,dfrfdiri,year1,year2,name)
{
    setVals(QPair<qint64,qint64>(val1,val2));
}

void PCx_QueryMinMax::setVals(QPair<qint64, qint64> vals)
{
    if(vals.first>vals.second)
    {
        qint64 temp=vals.first;
        vals.first=vals.second;
        vals.second=temp;
    }
    val1=vals.first;
    val2=vals.second;
}

unsigned int PCx_QueryMinMax::save(const QString &name) const
{
    if(name.isEmpty()||name.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;
    q.prepare(QString("insert into audit_queries_%1 (name,query_mode,target_type,ored,dfrfdiri,"
                      "val1,val2,year1,year2) values (:name,:qmode,:type,:ored,:dfrfdiri,"
                      ":val1,:val2,:y1,:y2)").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",(int)PCx_Query::QUERIESTYPES::MINMAX);
    q.bindValue(":type",typeId);
    q.bindValue(":ored",(int)ored);
    q.bindValue(":dfrfdiri",(int)dfrfdiri);
    q.bindValue(":val1",val1);
    q.bindValue(":val2",val2);
    q.bindValue(":y1",year1);
    q.bindValue(":y2",year2);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        return 0;
    }
    return q.lastInsertId().toInt();
}

QString PCx_QueryMinMax::exec(QXlsx::Document *xlsDoc) const
{
    QSqlQuery q;

    QString oredString=PCx_Audit::OREDtoTableString(ored);

    if(typeId!=ALLTYPES)
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 as a, arbre_%4 as b where a.id_node=b.id "
                          "and type=:typeId and %1 not null and %1>=:val1 and %1<=:val2 and annee>=:year1 and "
                          "annee<=:year2")
                  .arg(oredString,MODES::modeToTableString(dfrfdiri)).arg(model->getAuditId())
                  .arg(model->getAttachedTree()->getTreeId()));
        q.bindValue(":typeId",typeId);
    }
    else
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 where annee>=:year1 "
                          "and annee<=:year2 and %1 not null and %1>=:val1 and %1<=:val2")
                  .arg(oredString,MODES::modeToTableString(dfrfdiri)).arg(model->getAuditId()));

    }

    q.bindValue(":year1",year1);
    q.bindValue(":year2",year2);
    q.bindValue(":val1",val1);
    q.bindValue(":val2",val2);

    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }


    if(xlsDoc!=nullptr)
    {
        QString sheetName="ReqMinMax_"+name;
        if(xlsDoc->sheetNames().contains(sheetName))
        {
            sheetName="ReqMinMax_"+name+"_"+generateUniqueFileName();

        }
        xlsDoc->addSheet(sheetName);
        xlsDoc->selectSheet(sheetName);
        xlsDoc->write(1,2,QString("Requête %1").arg(name));
        xlsDoc->write(2,2,getDescription());
        xlsDoc->write(4,2,"Type");
        xlsDoc->write(4,3,"Noeud");
        xlsDoc->write(4,4,"Année");
        xlsDoc->write(4,5,PCx_Audit::OREDtoCompleteString(ored,true));
    }


    QString output=QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());

    output.append("<p><i>"+getDescription()+"</i></p>");
    output.append(QString("<table class='req3' cellpadding='5' align='center' style='margin-left:auto;margin-right:auto;'>"
                           "<tr><th>&nbsp;</th><th>année</th><th>%1</th></tr>").arg(PCx_Audit::OREDtoCompleteString(ored).toHtmlEscaped()));

    int currentRow=5;
    while(q.next())
    {
        unsigned int node,year;
        qint64 val;
        node=q.value("id_node").toUInt();
        year=q.value("annee").toUInt();
        val=q.value(oredString).toLongLong();

        output.append(QString("<tr><td>%1</td><td align='right'>%2</td><td align='right'>%3</td></tr>")
                .arg(model->getAttachedTree()->getNodeName(node).toHtmlEscaped())
                .arg(year)
                .arg(formatFixedPoint(val)));

        if(xlsDoc!=nullptr)
        {
            QPair<QString,QString> typeAndNode=model->getAttachedTree()->getTypeNameAndNodeName(node);
            xlsDoc->write(currentRow,2,typeAndNode.first);
            xlsDoc->write(currentRow,3,typeAndNode.second);
            xlsDoc->write(currentRow,4,year);
            xlsDoc->write(currentRow,5,fixedPointToDouble(val));
        }
        currentRow++;
    }
    output.append("</table>");

    return output;
}

bool PCx_QueryMinMax::load(unsigned int queryId)
{
    QSqlQuery q;
    q.prepare(QString("select * from audit_queries_%1 where id=:qid").arg(model->getAuditId()));
    q.bindValue(":qid",queryId);
    q.exec();

    if(!q.next())
    {
        qCritical()<<"Invalid PCx_query specified";
        return false;
    }
    else
    {
        if((PCx_Query::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Query::QUERIESTYPES::MINMAX)
        {
            qCritical()<<"Invalid PCx_query mode !";
            return false;
        }
        name=q.value("name").toString();
        typeId=q.value("target_type").toUInt();
        ored=(PCx_Audit::ORED)q.value("ored").toUInt();
        dfrfdiri=(MODES::DFRFDIRI)q.value("dfrfdiri").toUInt();
        setYears(q.value("year1").toUInt(),q.value("year2").toUInt());
        setVals(QPair<qint64,qint64>(q.value("val1").toLongLong(),q.value("val2").toLongLong()));
    }
    this->queryId=queryId;
    return true;

}

bool PCx_QueryMinMax::canSave(const QString &name) const
{
    if(name.isEmpty())
        return false;

    QSqlQuery q;
    q.prepare(QString("select * from audit_queries_%1 where name=:name and query_mode=:qmode").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",(int)PCx_Query::QUERIESTYPES::MINMAX);
    q.exec();

    if(q.next())
        return false;

    return true;
}

QString PCx_QueryMinMax::getDescription() const
{
    QString out;
    if(typeId==PCx_Query::ALLTYPES)
        out=QObject::tr("Tous les noeuds");
    else
        out=QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTree()->idTypeToName(typeId).toHtmlEscaped());

    out.append(QObject::tr(" dont les %1 des %2 sont compris entre %3€ et %4€ entre %5 et %6")
            .arg(PCx_Audit::OREDtoCompleteString(ored,true).toHtmlEscaped(),
            MODES::modeToCompleteString(dfrfdiri).toLower().toHtmlEscaped(),
            formatFixedPoint(val1),formatFixedPoint(val2))
            .arg(year1).arg(year2));
    return out;
}

QString PCx_QueryMinMax::getCSS()
{
    QColor color=getColor();
    QString css=QString("\n.req3{background-color:%1;padding:5px;}").arg(color.name());
    return css;
}

QColor PCx_QueryMinMax::getColor()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("queries/penminmax",PCx_QueryMinMax::DEFAULTCOlOR).toUInt();
    QColor color(oldcolor);
    return color;
}

