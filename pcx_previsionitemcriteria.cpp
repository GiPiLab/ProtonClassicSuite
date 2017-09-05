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
    if(auditId==0 || nodeId==0)
    {
        qFatal("Assertion failed");
    }

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
        output=QObject::tr("le minimum des %1").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::MAXIMUM:
        output=QObject::tr("le maximum des %1").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::AVERAGE:
        output=QObject::tr("la moyenne des %1").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::REGLIN:
        output=QObject::tr("la tendance linéaire des %1").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::LASTVALUE:
        output=QObject::tr("la dernière valeur des %1").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::PERCENT:
        output=QObject::tr("%1\% de la dernière valeur %2").arg(NUMBERSFORMAT::formatFixedPoint(previsionOperand),PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
        break;
    case PREVISIONOPERATOR::FIXEDVALUE:
        output=QObject::tr("Valeur fixe");
        break;    
    case PREVISIONOPERATOR::FIXEDVALUEFROMPROPORTIONALREPARTITION:
        output=QObject::tr("Distribution en fonction du poids relatif des %1 de l'année N").arg(PCx_Audit::OREDtoCompleteString(previsionOredTarget,true));
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
        previsionOredTarget=PCx_Audit::OREDFromTableString(items[1]);
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
    combo->addItem(QObject::tr("Minimum"),(int)PREVISIONOPERATOR::MINIMUM);
    combo->addItem(QObject::tr("Maximum"),(int)PREVISIONOPERATOR::MAXIMUM);
    combo->addItem(QObject::tr("Moyenne"),(int)PREVISIONOPERATOR::AVERAGE);
    combo->addItem(QObject::tr("Tendance linéaire"),(int)PREVISIONOPERATOR::REGLIN);
    combo->addItem(QObject::tr("Valeur fixe"),(int)PREVISIONOPERATOR::FIXEDVALUE);
    combo->addItem(QObject::tr("Dernière valeur"),(int)PREVISIONOPERATOR::LASTVALUE);
    combo->addItem(QObject::tr("Pourcentage"),(int)PREVISIONOPERATOR::PERCENT);
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
        output=QString("fixedvaluefromproportionalrepartition,%1,%2").arg(PCx_Audit::OREDtoTableString(previsionOredTarget)).arg(previsionOperand);
        break;
    }
    return output;
}

qint64 PCx_PrevisionItemCriteria::getLastValueOf(unsigned int auditId, MODES::DFRFDIRI mode, unsigned int nodeId) const
{
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;

    q.prepare(QString("select %1 from audit_%2_%3 where id_node=:id_node order by annee desc limit 1").arg(PCx_Audit::OREDtoTableString(previsionOredTarget),
              MODES::modeToTableString(mode))
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
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    qint64 lastValue=getLastValueOf(auditId,mode,nodeId);

    double perc=NUMBERSFORMAT::fixedPointToDouble(previsionOperand);
    perc/=100.0;
    return lastValue*perc;
}


qint64 PCx_PrevisionItemCriteria::getMinimumOf(unsigned int auditId, MODES::DFRFDIRI mode,unsigned int nodeId) const
{
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;

    q.prepare(QString("select min(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget),
              MODES::modeToTableString(mode))
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
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("select max(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget),
              MODES::modeToTableString(mode))
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
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("select avg(%1) from audit_%2_%3 where id_node=:id_node").arg(PCx_Audit::OREDtoTableString(previsionOredTarget),
              MODES::modeToTableString(mode))
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
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;
    q.prepare(QString("select annee,%1 from audit_%2_%3 where id_node=:id_node and %1 not null order by annee")
              .arg(PCx_Audit::OREDtoTableString(previsionOredTarget),
              MODES::modeToTableString(mode))
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


