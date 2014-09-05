#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include "pcx_audit.h"

class PCx_Tables
{
public:

    enum TABS{
        TABRESULTS,
        TABRECAP,
        TABEVOLUTION,
        TABEVOLUTIONCUMUL,
        TABBASE100,
        TABJOURSACT
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

    QString getTabJoursAct(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getTabBase100(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getTabEvolutionCumul(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getTabEvolution(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getTabRecap(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getTabResults(unsigned int node) const;

    static QString getCSS();

    //T1,T2... are named as in the original PCA version
    //Content of tab "Recap"
    QString getT1(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT4(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT8(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;

    //Content of tab "Evolution"
    QString getT2bis(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT3bis(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;

    //Content of tab "EvolutionCumul"
    QString getT2(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT3(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;

    //Content of tab "Base100"
    QString getT5(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT6(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;

    //Content of tab "Jours act."
    QString getT7(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;
    QString getT9(unsigned int node, PCx_AuditManage::DFRFDIRI mode) const;

    //Content of "Resultats" mode (without tabs)
    QString getT10(unsigned int node) const;
    QString getT11(unsigned int node) const;
    QString getT12(unsigned int node) const;



private:
    PCx_Audit *model;
};

#endif // PCX_TABLES_H
