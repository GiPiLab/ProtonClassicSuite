#include "pcx_queryrank.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "utils.h"

PCx_QueryRank::PCx_QueryRank():PCx_Query()
{
}

PCx_QueryRank::PCx_QueryRank(PCx_AuditModel *model, unsigned int queryId):PCx_Query(model)
{
    load(queryId);
}

PCx_QueryRank::PCx_QueryRank(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri, PCx_QueryRank::GREATERSMALLER greaterOrSmaller,
                             unsigned int number, unsigned int year1, unsigned int year2, const QString &name):PCx_Query(model,typeId,ored,dfrfdiri,year1,year2,name),grSm(greaterOrSmaller),number(number)
{

}

bool PCx_QueryRank::save(const QString &name) const
{
    Q_ASSERT(!name.isEmpty());

    QSqlQuery q;
    q.prepare(QString("insert into audit_queries_%1 (name,query_mode,target_type,ored,dfrfdiri,"
                      "increase_decrease,val1,year1,year2) values (:name,:qmode,:type,:ored,:dfrfdiri,"
                      ":incdec,:val1,:y1,:y2)").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",PCx_Query::RANK);
    q.bindValue(":type",typeId);
    q.bindValue(":ored",ored);
    q.bindValue(":dfrfdiri",dfrfdiri);
    q.bindValue(":incdec",grSm);
    q.bindValue(":val1",number);
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

QString PCx_QueryRank::exec() const
{
    QSqlQuery q;

    QString oredString=model->OREDtoTableString(ored);
    QString order="asc";

    if(grSm==GREATER)
        order="desc";

    if(typeId!=ALLTYPES)
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 as a, arbre_%4 as b where a.id_node=b.id "
                          "and type=:typeId and %1 not null and annee>=:year1 and annee<=:year2 order by %1 %5 limit %6")
                  .arg(oredString).arg(model->modeToTableString(dfrfdiri)).arg(model->getAuditId())
                  .arg(model->getAttachedTreeModel()->getTreeId()).arg(order).arg(number));
        q.bindValue(":typeId",typeId);
    }
    else
    {
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 where annee>=:year1 "
                          "and annee<=:year2 and %1 not null order by %1 %4 limit %5")
                  .arg(oredString).arg(model->modeToTableString(dfrfdiri)).arg(model->getAuditId())
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
                           "<tr><th>&nbsp;</th><th>année</th><th>%1</th></tr>").arg(PCx_AuditModel::OREDtoCompleteString(ored).toHtmlEscaped()));

    while(q.next())
    {
        output.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(model->getAttachedTreeModel()->getNodeName(q.value("id_node").toUInt()).toHtmlEscaped())
                .arg(q.value("annee").toUInt())
                .arg(formatCurrency(q.value(oredString).toLongLong())));
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
        if((PCx_Query::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Query::RANK)
        {
            qCritical()<<"Invalid PCx_query mode !";
            return false;
        }
        name=q.value("name").toString();
        typeId=q.value("target_type").toUInt();
        ored=(PCx_AuditModel::ORED)q.value("ored").toUInt();
        dfrfdiri=(PCx_AuditModel::DFRFDIRI)q.value("dfrfdiri").toUInt();
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
    q.bindValue(":qmode",PCx_Query::RANK);
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
        out=QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTreeModel()->getTypes()->getNomType(typeId).toHtmlEscaped());

    out.append(QObject::tr(" dont les crédits %1s des %2 sont parmi les [%3] %4 entre %5 et %6")
            .arg(PCx_AuditModel::OREDtoCompleteString(ored).toHtmlEscaped())
            .arg(PCx_AuditModel::modeToCompleteString(dfrfdiri).toLower().toHtmlEscaped())
            .arg(number).arg(greaterSmallerToString(grSm).toHtmlEscaped())
            .arg(year1).arg(year2));
    return out;

}


QString PCx_QueryRank::greaterSmallerToString(PCx_QueryRank::GREATERSMALLER grSm)
{
    switch(grSm)
    {
    case GREATER:
        return QObject::tr("plus grands");
        break;
    case SMALLER:
        return QObject::tr("plus petits");
        break;
    default:
        Q_UNREACHABLE();
        return QString();
    }
}

