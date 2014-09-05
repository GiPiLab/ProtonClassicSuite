#ifndef PCX_QUERYVARIATION_H
#define PCX_QUERYVARIATION_H

#include "pcx_audit.h"
#include "pcx_query.h"

class PCx_QueryVariation : public PCx_Query
{

public:

    static const unsigned int DEFAULTCOlOR=0xffaabb;

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

    PCx_QueryVariation(PCx_Audit *model,unsigned int queryId);

    PCx_QueryVariation(PCx_Audit *model, unsigned int typeId, PCx_AuditManage::ORED ored, PCx_AuditManage::DFRFDIRI dfrfdiri,
                       INCREASEDECREASE incDec, PERCENTORPOINTS percentOrPoints, OPERATORS op, qint64 val, unsigned int year1,
                       unsigned int year2, const QString &name="");


    bool load(unsigned int queryId);

    bool save(const QString &name) const;

    bool canSave(const QString &name) const;

    QString getDescription() const;

    QString exec() const;

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

    qint64 getValue()const{return val;}
    void setValue(qint64 val){this->val=val;}

    //Forbid same years for this query
    void setYears(unsigned int year1,unsigned int year2);

    static QString getCSS();
    static QColor getColor();


private:

    INCREASEDECREASE incDec;
    PERCENTORPOINTS percentOrPoints;
    OPERATORS op;
    qint64 val;


};
#endif // PCX_QUERYVARIATION_H
