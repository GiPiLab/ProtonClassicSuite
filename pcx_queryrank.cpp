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
        q.prepare(QString("select id_node,annee,%1 from audit_%2_%3 where %1 not null and annee>=:year1 "
                          "and annee<=:year2 order by %1 %4 limit %5")
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

}

bool PCx_QueryRank::canSave(const QString &name) const
{

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

