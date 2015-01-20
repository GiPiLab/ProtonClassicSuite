#ifndef PCX_PREVISIONCRITERIA_H
#define PCX_PREVISIONCRITERIA_H

#include "pcx_audit.h"

class PCx_PrevisionCriteria
{
public:

    enum PREVISIONOPERATOR
    {
        MINIMUM,
        MAXIMUM,
        AVERAGE,
        MEDIAN,
        REGLIN,
        LASTVALUE,
        PERCENT,
        FIXEDVALUEFORLEAVES,
        FIXEDVALUEFORNODE
    };

    PCx_PrevisionCriteria(PREVISIONOPERATOR previsionOperator, PCx_Audit::ORED ored, qint64 previsionOperand=0);
    PCx_PrevisionCriteria(const QString &serializedCriteria);
    ~PCx_PrevisionCriteria();

    qint64 getOperand() const{return previsionOperand;}
    PCx_Audit::ORED getOredTarget() const{return previsionOredTarget;}
    PREVISIONOPERATOR getOperator() const{return previsionOperator;}

    qint64 compute(PCx_Audit *audit,MODES::DFRFDIRI mode, unsigned int nodeId) const;

    QString getCriteriaLongDescription() const;
    QString getCriteriaShortDescription() const;

    QString serialize() const;



private:

    bool unserialize(const QString &criteriaString);
    qint64 getMinimumOf(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const;
    qint64 getMaximumOf(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const;
    qint64 getAverageOf(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int nodeId) const;
    qint64 getMedianOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;
    qint64 getReglinOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;
    qint64 getLastValueOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;
    qint64 getPercentOf(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;
    qint64 getFixedValueForLeaves(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;
    qint64 getFixedValueForNode(PCx_Audit *audit, MODES::DFRFDIRI mode,unsigned int nodeId) const;


    PREVISIONOPERATOR previsionOperator;
    PCx_Audit::ORED previsionOredTarget;
    qint64 previsionOperand;
};

#endif // PCX_PREVISIONCRITERIA_H
