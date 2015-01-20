#include "pcx_previsioncriteria.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_PrevisionCriteria::PCx_PrevisionCriteria(PREVISIONOPERATOR previsionOperator, PCx_Audit::ORED ored, qint64 previsionOperand)
    :previsionOperator(previsionOperator),previsionOredTarget(ored), previsionOperand(previsionOperand)
{
}

PCx_PrevisionCriteria::PCx_PrevisionCriteria(const QString &serializedCriteria)
{
    Q_ASSERT(!serializedCriteria.isEmpty());
    unserialize(serializedCriteria);

}

PCx_PrevisionCriteria::~PCx_PrevisionCriteria()
{

}

qint64 PCx_PrevisionCriteria::compute(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(audit!=nullptr && nodeId>0);
    switch(previsionOperator)
    {
    case PREVISIONOPERATOR::MINIMUM:
        return getMinimumOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::MAXIMUM:
        return getMaximumOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::AVERAGE:
        return getAverageOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::MEDIAN:
        return getMedianOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::REGLIN:
        return getReglinOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::LASTVALUE:
        return getLastValueOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::PERCENT:
        return getPercentOf(audit,mode,nodeId);
    case PREVISIONOPERATOR::FIXEDVALUEFORLEAVES:
        return getFixedValueForLeaves(audit,mode,nodeId);
    case PREVISIONOPERATOR::FIXEDVALUEFORNODE:
        return getFixedValueForNode(audit,mode,nodeId);

    }
    qWarning()<<"Unsupported case";
    return -MAX_NUM;

}


QString PCx_PrevisionCriteria::getCriteriaLongDescription() const
{
    QString output;
    switch(previsionOperator)
    {
    case PREVISIONOPERATOR::MINIMUM:
        output=QObject::tr("le minimum des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MAXIMUM:
        output=QObject::tr("le maximum des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::AVERAGE:
        output=QObject::tr("la moyenne des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MEDIAN:
        output=QObject::tr("la médiane des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::REGLIN:
        output=QObject::tr("la tendance linéaire des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QObject::tr("la dernière valeur des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QObject::tr("%1\% de la dernière valeur des crédits %2s").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand)).arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORNODE:
        output=QObject::tr("%1€ à partager entre les feuilles").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand));
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORLEAVES:
        output=QObject::tr("%1€ pour chaque feuille").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand));
        break;
    }
    return output;
}

QString PCx_PrevisionCriteria::getCriteriaShortDescription() const
{
    QString output;
    switch(previsionOperator)
    {
    case PREVISIONOPERATOR::MINIMUM:
        output=QObject::tr("min %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MAXIMUM:
        output=QObject::tr("max %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::AVERAGE:
        output=QObject::tr("moyenne %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MEDIAN:
        output=QObject::tr("médiane %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::REGLIN:
        output=QObject::tr("tendance %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QObject::tr("derniers %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QObject::tr("%1\% derniers %2s").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand)).arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORNODE:
        output=QObject::tr("%1€ partagé").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand));
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORLEAVES:
        output=QObject::tr("%1€ par feuille").arg(NUMBERSFORMAT::fixedPointToDouble(previsionOperand));
        break;
    }
    return output;
}

bool PCx_PrevisionCriteria::unserialize(const QString & criteriaString)
{
    QStringList items=criteriaString.split(",");
    if(items.size()!=3)
    {
        qWarning()<<"Invalid criteria string : "<<criteriaString;
        return false;
    }
    if(items[0]=="minimum")
    {
        previsionOperator=PREVISIONOPERATOR::MINIMUM;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);

    }
    else if(items[0]=="maximum")
    {
        previsionOperator=PREVISIONOPERATOR::MAXIMUM;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
    }
    else if(items[0]=="average")
    {
        previsionOperator=PREVISIONOPERATOR::AVERAGE;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
    }
    else if(items[0]=="median")
    {
        previsionOperator=PREVISIONOPERATOR::MEDIAN;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
    }
    else if(items[0]=="reglin")
    {
        previsionOperator=PREVISIONOPERATOR::REGLIN;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
    }
    else if(items[0]=="lastvalue")
    {
        previsionOperator=PREVISIONOPERATOR::LASTVALUE;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
    }
    else if(items[0]=="percent")
    {
        previsionOperator=PREVISIONOPERATOR::PERCENT;
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
        previsionOperand=items[2].toLongLong();
    }
    else if(items[0]=="fixedvaluefornode")
    {
        previsionOperator=PREVISIONOPERATOR::FIXEDVALUEFORNODE;
        previsionOperand=items[2].toLongLong();
    }
    else if(items[0]=="fixedvalueforleaves")
    {
        previsionOperator=PREVISIONOPERATOR::FIXEDVALUEFORLEAVES;
        previsionOperand=items[2].toLongLong();
    }
    else
    {
        qWarning()<<"Invalid prevision operator";
        return false;
    }

    return true;
}

QString PCx_PrevisionCriteria::serialize() const
{
    QString output;
    switch(previsionOperator)
    {
    case PREVISIONOPERATOR::MINIMUM:
        output=QString("minimum,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MAXIMUM:
        output=QString("maximum,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::AVERAGE:
        output=QString("average,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::MEDIAN:
        output=QString("median,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::REGLIN:
        output=QString("reglin,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QString("lastvalue,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QString("percent,%1,%2").arg(PCx_Audit::OREDtoTableString(previsionOredTarget)).arg(previsionOperand);
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORNODE:
        output=QString("fixedvaluefornode,0,%1").arg(previsionOperand);
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFORLEAVES:
        output=QString("fixedvalueforleaves,0,%1").arg(previsionOperand);
        break;
    }

    qDebug()<<output;
    return output;
}

qint64 PCx_PrevisionCriteria::getLastValueOf(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

}

qint64 PCx_PrevisionCriteria::getPercentOf(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

}

qint64 PCx_PrevisionCriteria::getFixedValueForLeaves(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const
{

}

qint64 PCx_PrevisionCriteria::getFixedValueForNode(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const
{

}

qint64 PCx_PrevisionCriteria::getMinimumOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select min(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(audit->getAuditId()));
    q.bindValue(":id_node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        return q.value(0).toLongLong();
    }
    else
    {
        qWarning()<<"No data";
    }
    return -MAX_NUM;
}

qint64 PCx_PrevisionCriteria::getMaximumOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select max(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(audit->getAuditId()));
    q.bindValue(":id_node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        return q.value(0).toLongLong();
    }
    else
    {
        qWarning()<<"No data";
    }
    return -MAX_NUM;

}

qint64 PCx_PrevisionCriteria::getAverageOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select avg(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(audit->getAuditId()));
    q.bindValue(":id_node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        return q.value(0).toLongLong();
    }
    else
    {
        qWarning()<<"No data";
    }
    return -MAX_NUM;

}

qint64 PCx_PrevisionCriteria::getMedianOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

}

qint64 PCx_PrevisionCriteria::getReglinOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

}


