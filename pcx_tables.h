#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include "pcx_audit.h"
#include "pcx_reporting.h"

class PCx_Tables
{
public:

    enum PCAPRESETS{
        PCARESULTS,
        PCAOVERVIEW,
        PCAEVOLUTION,
        PCAEVOLUTIONCUMUL,
        PCABASE100,
        PCADAYOFWORK
    };

    enum PCATABLES{
        PCARAWDATA,
        PCAT1,
        PCAT2,
        PCAT2BIS,
        PCAT3,
        PCAT3BIS,
        PCAT4,
        PCAT5,
        PCAT6,
        PCAT7,
        PCAT8,
        PCAT9,
        PCAT10,
        PCAT11,
        PCAT12
    };

    enum PCRTABLES{
        PCRRATIOPARENTS,
        PCRPROVENANCE,
        PCRVARIATION,
        PCRUTILISATION,
        PCRCYCLES,
        PCRRFDF,
        PCRRIDI,
        PCRRFDFRIDI
    };


    PCx_Tables(PCx_Audit *auditModel);
    PCx_Tables(PCx_Reporting *reportingModel);

    QString getPCAPresetDayOfWork(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAPresetBase100(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAPresetEvolutionCumul(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAPresetEvolution(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAPresetOverview(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAPresetResults(unsigned int node) const;

    static QString getCSS();

    QString getPCARawData(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "OVERVIEW"
    QString getPCAT1(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT4(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT8(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "EVOLUTION"
    QString getPCAT2bis(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT3bis(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "EVOLUTIONCUMUL"
    QString getPCAT2(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT3(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "BASE100"
    QString getPCAT5(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT6(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "DAYOFWORK"
    QString getPCAT7(unsigned int node, MODES::DFRFDIRI mode) const;
    QString getPCAT9(unsigned int node, MODES::DFRFDIRI mode) const;

    //Content of preset "RESULTS"
    QString getPCAT10(unsigned int node) const;
    QString getPCAT11(unsigned int node) const;
    QString getPCAT12(unsigned int node) const;




    QString getPCRRatioParents(unsigned int node,MODES::DFRFDIRI mode)const;
    QString getPCRProvenance(unsigned int node, MODES::DFRFDIRI mode)const;
    QString getPCRVariation(unsigned int node, MODES::DFRFDIRI mode)const;
    QString getPCRUtilisation(unsigned int node, MODES::DFRFDIRI mode)const;
    QString getPCRCycles(unsigned int node, MODES::DFRFDIRI mode)const;
    QString getPCRRFDF(unsigned int node)const;
    QString getPCRRIDI(unsigned int node)const;
    QString getPCRRFDFRIDI(unsigned int node)const;



private:
    PCx_Audit *auditModel;
    PCx_Reporting *reportingModel;

    PCx_Tables(const PCx_Tables &c);
    PCx_Tables &operator=(const PCx_Tables &);


};

#endif // PCX_TABLES_H
