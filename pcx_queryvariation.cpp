#include "pcx_queryvariation.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QtGlobal>
#include "utils.h"

PCx_QueryVariation::PCx_QueryVariation():PCx_Query()
{
}

PCx_QueryVariation::PCx_QueryVariation(PCx_AuditModel *model, unsigned int queryId):PCx_Query(model)
{
    load(queryId);
}

PCx_QueryVariation::PCx_QueryVariation(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri,
                                       INCREASEDECREASE increase,PERCENTORPOINTS percent, OPERATORS op, double val,unsigned int year1,unsigned int year2,
                                       const QString &name):PCx_Query(model,typeId,ored,dfrfdiri,year1,year2,name),
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
        if((PCx_Query::QUERIESTYPES)q.value("query_mode").toUInt()!=PCx_Query::VARIATION)
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
    this->queryId=queryId;
    return true;
}

bool PCx_QueryVariation::save(const QString &name) const
{
    Q_ASSERT(!name.isEmpty());

    QSqlQuery q;
    q.prepare(QString("insert into audit_queries_%1 (name,query_mode,target_type,ored,dfrfdiri,oper,percent_or_point,"
                      "increase_decrease,val1,year1,year2) values (:name,:qmode,:type,:ored,:dfrfdiri,:oper,:pop,"
                      ":incdec,:val1,:y1,:y2)").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",PCx_Query::VARIATION);
    q.bindValue(":type",typeId);
    q.bindValue(":ored",ored);
    q.bindValue(":dfrfdiri",dfrfdiri);
    q.bindValue(":oper",op);
    q.bindValue(":pop",percentOrPoints);
    q.bindValue(":incdec",incDec);
    q.bindValue(":val1",val);
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

bool PCx_QueryVariation::canSave(const QString &name) const
{
    if(name.isEmpty())
        return false;

    QSqlQuery q;
    q.prepare(QString("select * from audit_queries_%1 where name=:name and query_mode=:qmode").arg(model->getAuditId()));
    q.bindValue(":name",name);
    q.bindValue(":qmode",VARIATION);
    q.exec();

    if(q.next())
        return false;

    return true;
}

QString PCx_QueryVariation::getDescription() const
{
    QString out;
    if(typeId==0)
        out=QObject::tr("Tous les noeuds");
    else
        out=QObject::tr("Noeuds du type [%1]").arg(model->getAttachedTreeModel()->getTypes()->getNomType(typeId));

    out.append(QObject::tr(" dont les crédits %1s des %2 ont connu une %3 %4 %5 %6 entre %7 et %8")
            .arg(PCx_AuditModel::OREDtoCompleteString(ored))
            .arg(PCx_AuditModel::modeToCompleteString(dfrfdiri).toLower())
            .arg(incDecToString(incDec)).arg(operatorToString(op))
            .arg(val).arg(percentOrPointToString(percentOrPoints))
            .arg(year1).arg(year2));
    return out;
}

QString PCx_QueryVariation::exec() const
{
    QMap<unsigned int,qint64> valuesForYear1,valuesForYear2;
    QMap<unsigned int,qint64> variations,matchingNodes;
    QList<unsigned int>nodesOfThisType,problemNodes;


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

    //qDebug()<<"Nodes of type "<<typeId<<nodesOfThisType;

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
        unsigned int node=q.value("id_node").toUInt();

        if(typeId!=ALLTYPES&&!nodesOfThisType.contains(node))
            continue;

        if(q.value("annee").toUInt()==year1)
        {
            valuesForYear1.insert(node,q.value(oredString).toLongLong());
        }
        else if(q.value("annee").toUInt()==year2)
            valuesForYear2.insert(node,q.value(oredString).toLongLong());
        else
        {
            Q_UNREACHABLE();
            die();
        }
    }

    Q_ASSERT(valuesForYear1.keys()==valuesForYear2.keys());

    QMapIterator<unsigned int,qint64> i(valuesForYear1);
    while(i.hasNext())
    {
        i.next();
        qint64 val1=i.value();
        qint64 val2=valuesForYear2.value(i.key());
        qint64 variation;


        if(val1!=0)
        {
            if(percentOrPoints==PERCENT)
            {
                //Convert to fixed point percents with two decimals
                variation=100*100*(((double)val2-val1)/val1);
            }

            else
            {
                variation=val2-val1;
            }
            variations.insert(i.key(),variation);
        }
        else
        {
            problemNodes.append(i.key());
        }
    }

    OPERATORS trueOp=op;

    //qDebug()<<variations;
    QMapIterator<unsigned int,qint64> j(variations);
    while(j.hasNext())
    {
        j.next();
        qint64 nodeVariation=j.value();

        if(incDec==INCREASE)
        {
            if(nodeVariation<0)
                continue;
        }
        else if(incDec==DECREASE)
        {
            if(nodeVariation>0)
               continue;
            nodeVariation=qAbs(nodeVariation);
        }

        qint64 trueVal;
        if(percentOrPoints==PERCENT)
            //Convert to fixed point with 2 decimals
            trueVal=val*100;
        else
            //Convert to usual fixed point as stored in db
            trueVal=val*FIXEDPOINTCOEFF;

        //qDebug()<<"Comparing "<<nodeVariation<<" with "<<trueVal;

        switch(trueOp)
        {
        case LOWERTHAN:
            if(nodeVariation<trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case GREATERTHAN:
            if(nodeVariation>trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case LOWEROREQUAL:
            if(nodeVariation<=trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case GREATEROREQUAL:
            if(nodeVariation>=trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case EQUAL:
            if(nodeVariation==trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;

        case NOTEQUAL:
            if(nodeVariation!=trueVal)
                matchingNodes.insert(j.key(),j.value());
            break;
        }
    }

    QString output=QString("<h4>Requ&ecirc;te %1</h4>").arg(name.toHtmlEscaped());
    output.append("<p><i>"+getDescription().toHtmlEscaped()+"</i></p>");

    output.append(QString("<table class='req1' align='center' cellpadding='5' style='margin-left:auto;margin-right:auto'>"
                  "<tr><th>&nbsp;</th><th>%1</th><th>%2</th><th>%3</th>").arg(year1).arg(year2).arg(incDecToString(incDec)));

    QMapIterator<unsigned int,qint64> matchIter(matchingNodes);
    while(matchIter.hasNext())
    {
        matchIter.next();
        unsigned int node=matchIter.key();
        qint64 val=matchIter.value();

        if(percentOrPoints==PERCENT)
            //Add the last decimal to fit FIXEDPOINTCOEFF
            val=val*10;

        if(incDec!=VARIATION)val=qAbs(val);
        output.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td align='right'>%4 %5</td></tr>")
                .arg(model->getAttachedTreeModel()->getNodeName(node).toHtmlEscaped())
                .arg(formatCurrency(valuesForYear1.value(node)))
                .arg(formatCurrency(valuesForYear2.value(node)))
                .arg(formatCurrency(val))
                .arg(percentOrPointToString(percentOrPoints)));
    }

    if(!problemNodes.isEmpty())
    {
        output.append(QString("<tr><td colspan='4'><i>Non encore pourvus en %1</i></td></tr>").arg(year1));
    }
    foreach(unsigned int probNode,problemNodes)
    {
        output.append(QString("<tr><td><i>%1</i></td><td><i>%2</i></td><td><i>%3</i></td><td align='right'><i>NA</i></td></tr>")
                .arg(model->getAttachedTreeModel()->getNodeName(probNode).toHtmlEscaped())
                .arg(formatCurrency(valuesForYear1.value(probNode)))
                .arg(formatCurrency(valuesForYear2.value(probNode))));
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
