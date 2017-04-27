#include "pcx_queryrank.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include "utils.h"

using namespace NUMBERSFORMAT;


PCx_QueryRank::PCx_QueryRank(PCx_Audit *model, unsigned int queryId):PCx_Query(model)
{
    load(queryId);
}

PCx_QueryRank::PCx_QueryRank(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri, PCx_QueryRank::GREATERSMALLER greaterOrSmaller,
                             unsigned int number, unsigned int year1, unsigned int year2, const QString &name):PCx_Query(model,typeId,ored,dfrfdiri,year1,year2,name),grSm(greaterOrSmaller),number(number)
{
}

unsigned int PCx_QueryRank::save(const QString &name) const
{
    if(name.isEmpty()|| name.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;
    q.prepare(QString("insert into audit_queries_%1 (name,query_mode,target_type,ored,dfrfdiri,"
                      "increase_decrease,val1,year1,year2) values (:name,:qmode,:type,:ored,:dfrfdiri,"
                      ":incdec,:val1,:y1,:y2)").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",(int)PCx_Query::QUERIESTYPES::RANK);
    q.bindValue(":type",typeId);
    q.bindValue(":ored",(int)ored);
    q.bindValue(":dfrfdiri",(int)dfrfdiri);
    q.bindValue(":incdec",(int)grSm);
    q.bindValue(":val1",number);
    q.bindValue(":y1",year1);
    q.bindValue(":y2",year2);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        return 0;
    }
    return q.lastInsertId().toUInt();
}

QString PCx_QueryRank::exec(QXlsx::Document *xlsDoc) const
{
    if(!(year1>0 && year2>0 && year1<=year2))
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;

    QString oredString=PCx_Audit::OREDtoTableString(ored);
    QString order="asc";

    if(grSm==GREATERSMALLER::GREATER)
        order="desc";

    if(typeId!=ALLTYPES)
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 as a, arbre_%4 as b where a.id_node=b.id "
                          "and type=:typeId and %1 not null and annee>=:year1 and annee<=:year2 order by %1 %5 limit %6")
                  .arg(oredString,MODES::modeToTableString(dfrfdiri)).arg(model->getAuditId())
                  .arg(model->getAttachedTree()->getTreeId()).arg(order).arg(number));
        q.bindValue(":typeId",typeId);
    }
    else
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 where annee>=:year1 "
                          "and annee<=:year2 and %1 not null order by %1 %4 limit %5")
                  .arg(oredString,MODES::modeToTableString(dfrfdiri)).arg(model->getAuditId())
                  .arg(order).arg(number));
    }

    q.bindValue(":year1",year1);
    q.bindValue(":year2",year2);

    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    QString output=QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());
    output.append("<p><i>"+getDescription()+"</i></p>");
    output.append(QString("<table class='req2' cellpadding='5' align='center' style='margin-left:auto;margin-right:auto;'>"
                           "<tr><th>&nbsp;</th><th>année</th><th>%1</th></tr>").arg(PCx_Audit::OREDtoCompleteString(ored).toHtmlEscaped()));

    if(xlsDoc!=nullptr)
    {
        QString sheetName="ReqRank_"+name;
        if(xlsDoc->sheetNames().contains(sheetName))
        {
            sheetName="ReqRank_"+name+"_"+generateUniqueFileName();

        }
        xlsDoc->addSheet(sheetName);
        xlsDoc->selectSheet(sheetName);
        xlsDoc->write(1,2,QString("Requête %1").arg(name));
        xlsDoc->write(2,2,getDescription());
        xlsDoc->write(4,2,"Type");
        xlsDoc->write(4,3,"Noeud");
        xlsDoc->write(4,4,"Année");
        xlsDoc->write(4,5,PCx_Audit::OREDtoCompleteString(ored));
    }

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

bool PCx_QueryRank::load(unsigned int queryId)
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
        if((PCx_Query::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Query::QUERIESTYPES::RANK)
        {
            qCritical()<<"Invalid PCx_query mode !";
            return false;
        }
        name=q.value("name").toString();
        typeId=q.value("target_type").toUInt();
        ored=(PCx_Audit::ORED)q.value("ored").toUInt();
        dfrfdiri=(MODES::DFRFDIRI)q.value("dfrfdiri").toUInt();
        setYears(q.value("year1").toUInt(),q.value("year2").toUInt());
        grSm=(GREATERSMALLER)q.value("increase_decrease").toUInt();
        number=q.value("val1").toUInt();
    }
    this->queryId=queryId;
    return true;

}

bool PCx_QueryRank::canSave(const QString &name) const
{
    if(name.isEmpty())
        return false;

    QSqlQuery q;
    q.prepare(QString("select * from audit_queries_%1 where name=:name and query_mode=:qmode").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",(int)PCx_Query::QUERIESTYPES::RANK);
    q.exec();

    if(q.next())
        return false;

    return true;
}

QString PCx_QueryRank::getDescription() const
{
    QString out;
    if(typeId==PCx_Query::ALLTYPES)
        out=QObject::tr("Tous les noeuds");
    else
        out=QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTree()->idTypeToName(typeId).toHtmlEscaped());

    out.append(QObject::tr(" dont les %1 des %2 sont parmi les [%3] %4 entre %5 et %6")
            .arg(PCx_Audit::OREDtoCompleteString(ored,true).toHtmlEscaped(),
            MODES::modeToCompleteString(dfrfdiri).toLower().toHtmlEscaped())
            .arg(number).arg(greaterSmallerToString(grSm).toHtmlEscaped())
            .arg(year1).arg(year2));
    return out;

}


QString PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::GREATERSMALLER grSm)
{
    switch(grSm)
    {
    case GREATERSMALLER::GREATER:
        return QObject::tr("plus grands");
        break;
    case GREATERSMALLER::SMALLER:
        return QObject::tr("plus petits");
        break;
    default:
        Q_UNREACHABLE();
        return QString();
    }
}

QString PCx_QueryRank::getCSS()
{
    QColor color=getColor();
    QString css=QString("\n.req2{background-color:%1;padding:5px;}").arg(color.name());
    return css;
}

QColor PCx_QueryRank::getColor()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("queries/penrank",PCx_QueryRank::DEFAULTCOlOR).toUInt();
    QColor color(oldcolor);
    return color;
}

