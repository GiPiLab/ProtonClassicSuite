#ifndef PCX_QUERYRANK_H
#define PCX_QUERYRANK_H

#include "pcx_query.h"

class PCx_QueryRank : public PCx_Query
{
public:

    static const unsigned int DEFAULTCOlOR=0xaaffbb;


    enum GREATERSMALLER
    {
        SMALLER,
        GREATER
    };

    PCx_QueryRank(PCx_Audit *model,unsigned int queryId);

    PCx_QueryRank(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, PCx_Audit::DFRFDIRI dfrfdiri,
                       GREATERSMALLER greaterOrSmaller, unsigned int number, unsigned int year1,unsigned int year2, const QString &name="");


    unsigned int getNumber()const{return number;}
    GREATERSMALLER getGreaterOrSmaller()const{return grSm;}

    void setNumber(unsigned int num){number=num;}
    void setGreaterOrSmaller(GREATERSMALLER grsm){grSm=grsm;}

    bool save(const QString &name) const;
    QString exec(QXlsx::Document *xlsDoc=nullptr) const;
    bool load(unsigned int queryId);
    bool canSave(const QString &name) const;
    QString getDescription() const;

    static QString greaterSmallerToString(GREATERSMALLER grSm);

    static QString getCSS();
    static QColor getColor();


private:

    GREATERSMALLER grSm;
    unsigned int number;

    PCx_QueryRank(const PCx_QueryRank &c);
    PCx_QueryRank &operator=(const PCx_QueryRank &);

};

#endif // PCX_QUERYRANK_H
