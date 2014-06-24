#include "pcx_queryvariation.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QtGlobal>
#include "utils.h"

PCx_QueryVariation::PCx_QueryVariation():PCx_Queries()
{
}

PCx_QueryVariation::PCx_QueryVariation(PCx_AuditModel *model, unsigned int queryId):PCx_Queries(model)
{
    load(queryId);
}

PCx_QueryVariation::PCx_QueryVariation(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri,
                                       INCREASEDECREASE increase,PERCENTORPOINTS percent, OPERATORS op, double val,unsigned int year1,unsigned int year2,
                                       const QString &name):PCx_Queries(model,typeId,ored,dfrfdiri,year1,year2,name),
                                        incDec(increase),percentOrPoints(percent),op(op),val(val)
{
}

bool PCx_QueryVariation::load(unsigned int queryId)
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
        if((PCx_Queries::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Queries::VARIATION)
        {
            qCritical()<<"Invalid PCx_query mode !";
            return false;
        }
        name=q.value("name").toString();
        typeId=q.value("target_type").toUInt();
        ored=(PCx_AuditModel::ORED)q.value("ored").toUInt();
        dfrfdiri=(PCx_AuditModel::DFRFDIRI)q.value("dfrfdiri").toUInt();
        op=(OPERATORS)q.value("oper").toUInt();
        val=q.value("val1").toDouble();

        percentOrPoints=(PERCENTORPOINTS)q.value("percent_or_point").toUInt();
        incDec=(INCREASEDECREASE)q.value("increase_decrease").toUInt();
        setYears(q.value("year1").toUInt(),q.value("year2").toUInt());
    }

    return true;
}

bool PCx_QueryVariation::save(const QString &name)
{

}

QString PCx_QueryVariation::exec()
{

    QMap<unsigned int,double> valuesForYear1,valuesForYear2,variations,matchingNodes;
    QList<unsigned int>nodesOfThisType;

    QSqlQuery q;

    if(typeId!=ALLTYPES)
    {
        q.prepare(QString("select * from arbre_%1 where type=:type").arg(model->getAttachedTreeModel()->getTreeId()));
        q.bindValue(":type",typeId);
        q.exec();

        while(q.next())
        {
            nodesOfThisType.append(q.value("id").toUInt());
        }
    }

    qDebug()<<"Nodes of type "<<typeId<<nodesOfThisType;

    q.prepare(QString("select * from audit_%1_%2 where annee=:year1 or annee=:year2").arg(model->modeToTableString(dfrfdiri)).arg(model->getAuditId()));
    q.bindValue(":year1",year1);
    q.bindValue(":year2",year2);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    QString oredString=model->OREDtoTableString(ored);

    while(q.next())
    {
        //FIXME : NAN and INF
        unsigned int node=q.value("id_node").toUInt();

        if(typeId!=ALLTYPES&&!nodesOfThisType.contains(node))
            continue;

        if(q.value("annee").toUInt()==year1)
        {
            valuesForYear1.insert(node,q.value(oredString).toDouble());
        }
        else if(q.value("annee").toUInt()==year2)
            valuesForYear2.insert(node,q.value(oredString).toDouble());
        else
        {
            Q_UNREACHABLE();
            die();
        }
    }

    Q_ASSERT(valuesForYear1.keys()==valuesForYear2.keys());

    QMapIterator<unsigned int,double> i(valuesForYear1);
    while(i.hasNext())
    {
        i.next();
        double val1=i.value();
        double val2=valuesForYear2.value(i.key());
        double variation;
        if(percentOrPoints==PERCENT)
            variation=(val2-val1)/val1;
        else
            variation=val2-val1;

        variations.insert(i.key(),variation);
    }

    OPERATORS trueOp=op;
    double trueVal;

    if(percentOrPoints==PERCENT)
    {
        trueVal=val/100;
    }
    else
    {
        trueVal=val;
    }
/*
    if(incDec==INCREASE)
    {
        trueOp=op;
    }
    //Reverse value and operator in case of decrease
    else
    {
        trueVal=-trueVal;
        switch(op)
        {
        case LOWERTHAN:
            trueOp=GREATERTHAN;
            break;
        case GREATERTHAN:
            trueOp=LOWERTHAN;
            break;
        case LOWEROREQUAL:
            trueOp=GREATEROREQUAL;
            break;
        case GREATEROREQUAL:
            trueOp=LOWEROREQUAL;
            break;
        default:
            trueOp=op;
        }
    }
*/
    qDebug()<<variations;
    QMapIterator<unsigned int,double> j(variations);
    while(j.hasNext())
    {
        j.next();

        if(incDec==INCREASE)
        {
            if(j.value()<0)
                continue;
        }
        else if(incDec==DECREASE)
        {
            if(j.value()>0)
                continue;
        }
        trueVal=qAbs(trueVal);

        qDebug()<<"Comparing "<<j.value()<<" with "<<trueVal;

        //NOTE : check floating point arithmetics
        switch(trueOp)
        {
        case LOWERTHAN:
            if(j.value()<trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case GREATERTHAN:
            if(j.value()>trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case LOWEROREQUAL:
            if(j.value()<=trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case GREATEROREQUAL:
            if(j.value()>=trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case EQUAL:
            if(qFuzzyCompare(j.value()+1.0,trueVal+1.0))
                matchingNodes.insert(j.key(),j.value());
            break;

        case NOTEQUAL:
            if(!qFuzzyCompare(j.value()+1.0,trueVal+1.0))
                matchingNodes.insert(j.key(),j.value());
            break;
        }
    }

    QString output=QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());
    //output.append(reqName());

    output.append(QString("<table class='req1' align='center' cellpadding='5' style='margin-left:auto;margin-right:auto'>"
                  "<tr><th>&nbsp;</th><th>%1</th><th>%2</th><th>%3</th>").arg(year1).arg(year2).arg(incDecToString(incDec)));

    QMapIterator<unsigned int,double> matchIter(matchingNodes);
    while(matchIter.hasNext())
    {
        matchIter.next();
        unsigned int node=matchIter.key();
        double val=matchIter.value();
        if(percentOrPoints==PERCENT)val*=100;
        if(incDec!=VARIATION)val=qAbs(val);
        output.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td align='right'>%4 %5</td></tr>")
                .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped())
                .arg(formatDouble(valuesForYear1.value(node)))
                .arg(formatDouble(valuesForYear2.value(node)))
                .arg(formatDouble(val))
                .arg(percentOrPointToString(percentOrPoints)));
    }

    output.append("</table>");
    return output;
}

const QString PCx_QueryVariation::operatorToString(OPERATORS op)
{
    QString output;
    switch(op)
    {
    case LOWERTHAN:
        output=QObject::tr("strictement inférieure à");
        break;
    case LOWEROREQUAL:
        output=QObject::tr("inférieure ou égale à");
        break;
    case EQUAL:
        output=QObject::tr("égale à");
        break;
    case NOTEQUAL:
        output=QObject::tr("différente de");
        break;
    case GREATEROREQUAL:
        output=QObject::tr("supérieure ou égale à");
        break;
    case GREATERTHAN:
        output=QObject::tr("strictement supérieure à");
        break;
    default:
        Q_UNREACHABLE();
    }
    return output;
}

PCx_QueryVariation::OPERATORS PCx_QueryVariation::stringToOperator(const QString &s)
{
    if(s=="<")
        return LOWERTHAN;
    else if(s=="<=")
        return LOWEROREQUAL;
    else if(s=="=")
        return EQUAL;
    else if(s=="!=")
        return NOTEQUAL;
    else if(s==">=")
        return GREATEROREQUAL;
    else if(s==">")
        return GREATERTHAN;

    qCritical()<<"Invalid operator string, defaulting to EQUAL";
    return EQUAL;
}

const QString PCx_QueryVariation::incDecToString(INCREASEDECREASE incDec)
{
    QString output;
    switch(incDec)
    {
    case INCREASE:
        output=QObject::tr("augmentation");
        break;
    case DECREASE:
        output=QObject::tr("diminution");
        break;
    case VARIATION:
        output=QObject::tr("variation");
        break;
    default:
        Q_UNREACHABLE();
    }
    return output;
}

const QString PCx_QueryVariation::percentOrPointToString(PCx_QueryVariation::PERCENTORPOINTS pop)
{
    QString out;
    switch(pop)
    {
    case PERCENT:
        out=QObject::tr("%");
        break;

    case POINTS:
        out=QObject::tr("€");
        break;
    default:
        Q_UNREACHABLE();
    }
    return out;
}
