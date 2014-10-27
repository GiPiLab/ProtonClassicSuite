#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include "pcx_audit.h"

class PCx_Tables
{
public:

    enum PRESETS{
        RESULTS,
        OVERVIEW,
        EVOLUTION,
        EVOLUTIONCUMUL,
        BASE100,
        DAYOFWORK
    };

    enum TABLES{
        T1,
        T2,
        T2BIS,
        T3,
        T3BIS,
        T4,
        T5,
        T6,
        T7,
        T8,
        T9,
        T10,
        T11,
        T12
    };

    PCx_Tables(PCx_Audit *model);

    QString getPresetDayOfWork(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getPresetBase100(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getPresetEvolutionCumul(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getPresetEvolution(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getPresetOverview(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getPresetResults(unsigned int node) const;

    static QString getCSS();

    //Content of preset "OVERVIEW"
    QString getT1(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT4(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT8(unsigned int node, PCx_Audit::DFRFDIRI mode) const;

    //Content of preset "EVOLUTION"
    QString getT2bis(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT3bis(unsigned int node, PCx_Audit::DFRFDIRI mode) const;

    //Content of preset "EVOLUTIONCUMUL"
    QString getT2(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT3(unsigned int node, PCx_Audit::DFRFDIRI mode) const;

    //Content of preset "BASE100"
    QString getT5(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT6(unsigned int node, PCx_Audit::DFRFDIRI mode) const;

    //Content of preset "DAYOFWORK"
    QString getT7(unsigned int node, PCx_Audit::DFRFDIRI mode) const;
    QString getT9(unsigned int node, PCx_Audit::DFRFDIRI mode) const;

    //Content of preset "RESULTS"
    QString getT10(unsigned int node) const;
    QString getT11(unsigned int node) const;
    QString getT12(unsigned int node) const;



private:
    PCx_Audit *model;
    PCx_Tables(const PCx_Tables &c);
    PCx_Tables &operator=(const PCx_Tables &);


};

#endif // PCX_TABLES_H
