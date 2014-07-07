#ifndef PCX_QUERYMINMAX_H
#define PCX_QUERYMINMAX_H

#include "pcx_query.h"

class PCx_QueryMinMax : public PCx_Query
{
public:

    PCx_QueryMinMax();
    PCx_QueryMinMax(PCx_AuditModel *model,unsigned int queryId);

    PCx_QueryMinMax(PCx_AuditModel *model, unsigned int typeId, PCx_AuditModel::ORED ored, PCx_AuditModel::DFRFDIRI dfrfdiri,
                       qint64 val1, qint64 val2, unsigned int year1,unsigned int year2, const QString &name="");

    QPair<qint64,qint64> getVals()const{return QPair<qint64,qint64>(val1,val2);}

    void setVals(QPair<qint64,qint64> vals);

    bool save(const QString &name) const;
    QString exec() const;
    bool load(unsigned int queryId);
    bool canSave(const QString &name) const;
    QString getDescription() const;


private:

    qint64 val1,val2;

};

#endif // PCX_QUERYMINMAX_H
