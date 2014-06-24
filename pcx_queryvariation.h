#ifndef PCX_QUERYVARIATION_H
#define PCX_QUERYVARIATION_H

#include "pcx_auditmodel.h"
#include "pcx_queries.h"

class PCx_QueryVariation : public PCx_Queries
{

public:

    enum OPERATORS
    {
        LOWERTHAN,
        LOWEROREQUAL,
        EQUAL,
        NOTEQUAL,
        GREATEROREQUAL,
        GREATERTHAN
    };

    enum PERCENTORPOINTS
    {
        PERCENT,
        POINTS
    };

    enum INCREASEDECREASE
    {
        INCREASE,
        DECREASE,
        VARIATION
    };

    PCx_QueryVariation();

    PCx_QueryVariation(PCx_AuditModel *model,unsigned int queryId);

    PCx_QueryVariation(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri,
                       INCREASEDECREASE incDec, PERCENTORPOINTS percentOrPoints, OPERATORS op, double val, unsigned int year1,
                       unsigned int year2, const QString &name="");


    bool load(unsigned int queryId);

    bool save(const QString &name);

    QString exec();

    void setOperator(OPERATORS op){this->op=op;}
    OPERATORS getOperator() const{return op;}

    static const QString operatorToString(OPERATORS op);
    static OPERATORS stringToOperator(const QString &s);

    INCREASEDECREASE getIncDec() const{return incDec;}
    void setIncDec(INCREASEDECREASE incDec){this->incDec=incDec;}
    static const QString incDecToString(INCREASEDECREASE incDec);

    PERCENTORPOINTS getPercentOrPoints() const{return percentOrPoints;}
    void setPercentOrPoints(PERCENTORPOINTS pop){percentOrPoints=pop;}
    static const QString percentOrPointToString(PERCENTORPOINTS pop);

    double getValue()const{return val;}
    void setValue(double val){this->val=val;}



private:

    INCREASEDECREASE incDec;
    PERCENTORPOINTS percentOrPoints;
    OPERATORS op;
    double val;


};
#endif // PCX_QUERYVARIATION_H
