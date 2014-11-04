#ifndef PCX_QUERYMINMAX_H
#define PCX_QUERYMINMAX_H

#include "pcx_query.h"

class PCx_QueryMinMax : public PCx_Query
{
public:

    static const unsigned int DEFAULTCOlOR=0xaabbff;

    PCx_QueryMinMax(PCx_Audit *model,unsigned int queryId);

    PCx_QueryMinMax(PCx_Audit *model, unsigned int typeId, PCx_Audit::ORED ored, MODES::DFRFDIRI dfrfdiri,
                       qint64 val1, qint64 val2, unsigned int year1,unsigned int year2, const QString &name="");

    QPair<qint64,qint64> getVals()const{return QPair<qint64,qint64>(val1,val2);}

    void setVals(QPair<qint64,qint64> vals);

    bool save(const QString &name) const;
    QString exec(QXlsx::Document *xlsDoc=nullptr) const;
    bool load(unsigned int queryId);
    bool canSave(const QString &name) const;
    QString getDescription() const;

    static QString getCSS();
    static QColor getColor();


private:
    PCx_QueryMinMax(const PCx_QueryMinMax &c);
    PCx_QueryMinMax &operator=(const PCx_QueryMinMax &);

    qint64 val1,val2;

};

#endif // PCX_QUERYMINMAX_H
