#ifndef PCX_QUERIES_H
#define PCX_QUERIES_H

#include "pcx_auditmodel.h"

class PCx_Queries
{
public:

    enum QUERIESTYPES
    {
        VARIATION,
        MINMAX,
        BOUND
    };

    static const unsigned int ALLTYPES=0;


    PCx_Queries();
    PCx_Queries(PCx_AuditModel *model);
    PCx_Queries(PCx_AuditModel *model,unsigned int typeId,PCx_AuditModel::ORED ored,PCx_AuditModel::DFRFDIRI dfrfdiri,
                                      unsigned int year1, unsigned int year2,const QString &name="");

    QPair<unsigned int,unsigned int> getYears() const {return QPair<unsigned int, unsigned int>(year1,year2);}
    void setYears(unsigned int year1, unsigned int year2);

    void setORED(PCx_AuditModel::ORED ored){this->ored=ored;}
    PCx_AuditModel::ORED getORED() const{return ored;}

    void setDFRFDIRI(PCx_AuditModel::DFRFDIRI dfrfdiri){this->dfrfdiri=dfrfdiri;}
    PCx_AuditModel::DFRFDIRI getDFRFDIRI() const{return dfrfdiri;}

    unsigned int getTypeId() const{return typeId;}
    void setTypeId(unsigned int typeId){this->typeId=typeId;}

    void setModel(PCx_AuditModel *model){this->model=model;}
    const PCx_AuditModel *getModel() const{return model;}

    const QString &getName() const{return name;}
    void setName(const QString &name){this->name=name;}


    bool remove(unsigned int queryId);

    virtual bool save(const QString &name)=0;
    virtual bool load(unsigned int queryId)=0;

    virtual QString exec()=0;


    static bool createTableQueries(unsigned int auditId);




protected:
    PCx_AuditModel *model;
    //If typeId==ALLTYPES, do not filter nodes
    unsigned int typeId;
    PCx_AuditModel::ORED ored;
    PCx_AuditModel::DFRFDIRI dfrfdiri;
    unsigned int year1,year2;
    QString name;

};

#endif // PCX_QUERIES_H
