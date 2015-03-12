#include "pcx_queriesmodel.h"
#include <QBrush>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include "pcx_query.h"
#include "pcx_queryvariation.h"
#include "pcx_queryrank.h"
#include "pcx_queryminmax.h"

PCx_QueriesModel::PCx_QueriesModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    auditId=0;
    auditModel=nullptr;
}

PCx_QueriesModel::PCx_QueriesModel(PCx_Audit *auditModel,QObject *parent):QSqlQueryModel(parent),auditModel(auditModel)
{
    auditId=auditModel->getAuditId();
    update();
}

QVariant PCx_QueriesModel::data(const QModelIndex &item, int role) const
{
    if(!item.isValid())return QVariant();

    if(role==Qt::BackgroundRole)
    {
        PCx_Query::QUERIESTYPES queryType=(PCx_Query::QUERIESTYPES)record(item.row()).value("query_mode").toUInt();

        switch(queryType)
        {
        case PCx_Query::VARIATION:
            return QVariant(QBrush(PCx_QueryVariation::getColor()));
        case PCx_Query::RANK:
            return QVariant(QBrush(PCx_QueryRank::getColor()));
        case PCx_Query::MINMAX:
            return QVariant(QBrush(PCx_QueryMinMax::getColor()));
        default:
            Q_UNREACHABLE();
            return QSqlQueryModel::data(item,role);
        }
    }
    else if(role==Qt::ToolTipRole)
    {
        PCx_Query::QUERIESTYPES queryType=(PCx_Query::QUERIESTYPES)record(item.row()).value("query_mode").toUInt();
        unsigned int queryId=record(item.row()).value("id").toUInt();

        switch(queryType)
        {
        case PCx_Query::VARIATION:
        {
            PCx_QueryVariation q(auditModel,queryId);
            return q.getDescription().toHtmlEscaped();
            break;
        }
        case PCx_Query::RANK:
        {
                PCx_QueryRank q(auditModel,queryId);
                return q.getDescription().toHtmlEscaped();
                break;
        }


        case PCx_Query::MINMAX:
        {
                PCx_QueryMinMax q(auditModel,queryId);
                return q.getDescription().toHtmlEscaped();
                break;
        }

        }

    }
    else
        return QSqlQueryModel::data(item,role);
}

void PCx_QueriesModel::update()
{
    if(auditId==0)
    {
        qFatal("Assertion failed");
    }
    setQuery(QString("select name,id,query_mode from audit_queries_%1").arg(auditId));
}
