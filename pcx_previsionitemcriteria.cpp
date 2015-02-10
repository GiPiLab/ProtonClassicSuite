#include "pcx_previsionitemcriteria.h"
#include "utils.h"
#include <QSqlQuery>
#include <QSqlError>

PCx_PrevisionItemCriteria::PCx_PrevisionItemCriteria()
{

}

PCx_PrevisionItemCriteria::PCx_PrevisionItemCriteria(const QString &serializedCriteria)
{
    unserialize(serializedCriteria);
}

PCx_PrevisionItemCriteria::PCx_PrevisionItemCriteria(PREVISIONOPERATOR previsionOperator, PCx_Audit::ORED ored, qint64 previsionOperand)
    :previsionOperator(previsionOperator),previsionOredTarget(ored), previsionOperand(previsionOperand)
{
}



PCx_PrevisionItemCriteria::~PCx_PrevisionItemCriteria()
{

}

qint64 PCx_PrevisionItemCriteria::compute(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(auditId>0 && nodeId>0);

    switch(previsionOperator)
    {
    case PREVISIONOPERATOR::MINIMUM:
        return getMinimumOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::MAXIMUM:
        return getMaximumOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::AVERAGE:
        return getAverageOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::REGLIN:
        return getReglinOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::LASTVALUE:
        return getLastValueOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::PERCENT:
        return getPercentOf(auditId,mode,nodeId);
    case PREVISIONOPERATOR::FIXEDVALUE:
    case PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION:
        return getFixedValue();
    }
    return -MAX_NUM;
}


QString PCx_PrevisionItemCriteria::getCriteriaLongDescription() const
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
    case PREVISIONOPERATOR::REGLIN:
        output=QObject::tr("la tendance linéaire des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QObject::tr("la dernière valeur des crédits %1s").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QObject::tr("%1\% de la dernière valeur des crédits %2s").arg(NUMBERSFORMAT::formatFixedPoint(previsionOperand)).arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::FIXEDVALUE:
        output=QObject::tr("Valeur fixe");
        break;    
    case PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION:
        output=QObject::tr("Distribution en fonction du poids relatif des réalisés de l'année N");
    }
    return output;
}


bool PCx_PrevisionItemCriteria::unserialize(const QString & criteriaString)
{
    if(criteriaString.isEmpty())
        return true;
    QStringList items=criteriaString.split(",",QString::SkipEmptyParts);
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
    else if(items[0]=="fixedvalue")
    {
        previsionOperator=PREVISIONOPERATOR::FIXEDVALUE;
        previsionOperand=items[2].toLongLong();
    }
    else if(items[0]=="fixedvaluefromproportionalrepartition")
    {
        previsionOperator=PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION;
        previsionOperand=items[2].toLongLong();
    }
    else
    {
        qCritical()<<"Invalid prevision operator";
        die();
    }

    return true;
}

void PCx_PrevisionItemCriteria::fillComboBoxWithOperators(QComboBox *combo)
{
    combo->clear();
    combo->addItem(QObject::tr("Minimum"),PREVISIONOPERATOR::MINIMUM);
    combo->addItem(QObject::tr("Maximum"),PREVISIONOPERATOR::MAXIMUM);
    combo->addItem(QObject::tr("Moyenne"),PREVISIONOPERATOR::AVERAGE);
    combo->addItem(QObject::tr("Tendance linéaire"),PREVISIONOPERATOR::REGLIN);
    combo->addItem(QObject::tr("Valeur fixe"),PREVISIONOPERATOR::FIXEDVALUE);
    combo->addItem(QObject::tr("Dernière valeur"),PREVISIONOPERATOR::LASTVALUE);
    combo->addItem(QObject::tr("Pourcentage"),PREVISIONOPERATOR::PERCENT);



}

QString PCx_PrevisionItemCriteria::serialize() const
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
    case PREVISIONOPERATOR::REGLIN:
        output=QString("reglin,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QString("lastvalue,%1,0").arg(PCx_Audit::OREDtoTableString(previsionOredTarget));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QString("percent,%1,%2").arg(PCx_Audit::OREDtoTableString(previsionOredTarget)).arg(previsionOperand);
        break;
    case PREVISIONOPERATOR::FIXEDVALUE:
        output=QString("fixedvalue,0,%1").arg(previsionOperand);
        break;
    case PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION:
        output=QString("fixedvaluefromproportionalrepartition,0,%1").arg(previsionOperand);
        break;
    }
    return output;
}

qint64 PCx_PrevisionItemCriteria::getLastValueOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

    QSqlQuery q;

    q.prepare(QString("select %1 from audit_%2_%3 where id_node=:id_node order by annee desc limit 1").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(auditId));
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

qint64 PCx_PrevisionItemCriteria::getPercentOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    qint64 lastValue=getLastValueOf(auditId,mode,nodeId);

    double perc=NUMBERSFORMAT::fixedPointToDouble(previsionOperand);
    perc/=100.0;
    return lastValue*perc;
}


qint64 PCx_PrevisionItemCriteria::getMinimumOf(unsigned int auditId, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;

    q.prepare(QString("select min(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(auditId));
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

qint64 PCx_PrevisionItemCriteria::getMaximumOf(unsigned int auditId, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select max(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(auditId));
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

qint64 PCx_PrevisionItemCriteria::getAverageOf(unsigned int auditId, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select avg(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(auditId));
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


qint64 PCx_PrevisionItemCriteria::getReglinOf(unsigned int auditId, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);

    QSqlQuery q;
    q.prepare(QString("select annee,%1 from audit_%2_%3 where id_node=:id_node and %1 not null order by annee")
              .arg(PCx_Audit::OREDtoTableString(previsionOredTarget))
              .arg(MODES::modeToTableString(mode))
              .arg(auditId)
              );
    q.bindValue(":id_node",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    QVector<double>x,y;

    while(q.next())
    {
        x.append((double)q.value(0).toUInt());
        y.append(NUMBERSFORMAT::fixedPointToDouble(q.value(1).toLongLong()));
    }
    if(x.isEmpty()||y.isEmpty())
        return 0;

    double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;

    int n=x.size();
    int i;

    for (i = 0; i < n; i++)
    {
        m_x += (x[i] - m_x) / (i + 1.0);
        m_y += (y[i] - m_y) / (i + 1.0);
    }

    for (i = 0; i < n; i++)
    {
        const double dx = x[i] - m_x;
        const double dy = y[i] - m_y;

        m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
        m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }

    // In terms of y = ax + b
    double a = m_dxdy / m_dx2;
    double b = m_y - m_x * a;

    //qDebug()<<"y=ax+b, a="<<a<<"b="<<b;

    return NUMBERSFORMAT::doubleToFixedPoint(a*(x.last()+1.0)+b);
}


