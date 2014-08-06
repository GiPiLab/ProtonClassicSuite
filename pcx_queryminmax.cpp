#include "pcx_queryminmax.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include "utils.h"

PCx_QueryMinMax::PCx_QueryMinMax():PCx_Query()
{
}

PCx_QueryMinMax::PCx_QueryMinMax(PCx_Audit *model, unsigned int queryId):PCx_Query(model)
{
    load(queryId);
}

PCx_QueryMinMax::PCx_QueryMinMax(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, PCx_Audit::DFRFDIRI dfrfdiri, qint64 val1,
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

bool PCx_QueryMinMax::save(const QString &name) const
{
    Q_ASSERT(!name.isEmpty());

    QSqlQuery q;
    q.prepare(QString("insert into audit_queries_%1 (name,query_mode,target_type,ored,dfrfdiri,"
                      "val1,val2,year1,year2) values (:name,:qmode,:type,:ored,:dfrfdiri,"
                      ":val1,:val2,:y1,:y2)").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",PCx_Query::MINMAX);
    q.bindValue(":type",typeId);
    q.bindValue(":ored",ored);
    q.bindValue(":dfrfdiri",dfrfdiri);
    q.bindValue(":val1",val1);
    q.bindValue(":val2",val2);
    q.bindValue(":y1",year1);
    q.bindValue(":y2",year2);
    q.exec();

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        return false;
    }
    return true;
}

QString PCx_QueryMinMax::exec() const
{
    QSqlQuery q;

    QString oredString=model->OREDtoTableString(ored);

    if(typeId!=ALLTYPES)
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 as a, arbre_%4 as b where a.id_node=b.id "
                          "and type=:typeId and %1 not null and %1>=:val1 and %1<=:val2 and annee>=:year1 and "
                          "annee<=:year2")
                  .arg(oredString).arg(model->modeToTableString(dfrfdiri)).arg(model->getAuditId())
                  .arg(model->getAttachedTreeModel()->getTreeId()));
        q.bindValue(":typeId",typeId);
    }
    else
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 where annee>=:year1 "
                          "and annee<=:year2 and %1 not null and %1>=:val1 and %1<=:val2")
                  .arg(oredString).arg(model->modeToTableString(dfrfdiri)).arg(model->getAuditId()));

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

    QString output=QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());

    output.append("<p><i>"+getDescription()+"</i></p>");
    output.append(QString("<table class='req3' cellpadding='5' align='center' style='margin-left:auto;margin-right:auto;'>"
                           "<tr><th>&nbsp;</th><th>année</th><th>%1</th></tr>").arg(PCx_Audit::OREDtoCompleteString(ored).toHtmlEscaped()));

    while(q.next())
    {
        output.append(QString("<tr><td>%1</td><td align='right'>%2</td><td align='right'>%3</td></tr>")
                .arg(model->getAttachedTreeModel()->getNodeName(q.value("id_node").toUInt()).toHtmlEscaped())
                .arg(q.value("annee").toUInt())
                .arg(formatCurrency(q.value(oredString).toLongLong())));
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
        if((PCx_Query::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Query::MINMAX)
        {
            qCritical()<<"Invalid PCx_query mode !";
            return false;
        }
        name=q.value("name").toString();
        typeId=q.value("target_type").toUInt();
        ored=(PCx_Audit::ORED)q.value("ored").toUInt();
        dfrfdiri=(PCx_Audit::DFRFDIRI)q.value("dfrfdiri").toUInt();
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
    q.bindValue(":qmode",PCx_Query::MINMAX);
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
        out=QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTreeModel()->getTypes()->getNomType(typeId).toHtmlEscaped());

    out.append(QObject::tr(" dont les crédits %1s des %2 sont compris entre %3€ et %4€ entre %5 et %6")
            .arg(PCx_Audit::OREDtoCompleteString(ored).toHtmlEscaped())
            .arg(PCx_Audit::modeToCompleteString(dfrfdiri).toLower().toHtmlEscaped())
            .arg(formatCurrency(val1)).arg(formatCurrency(val2))
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

