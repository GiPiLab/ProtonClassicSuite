#ifndef PCX_TABLES_H
#define PCX_TABLES_H

#include "pcx_auditmodel.h"

class PCx_Tables
{
public:

    enum TABSMASK{
        TABRESULTS=1,
        TABRECAP=2,
        TABEVOLUTION=4,
        TABEVOLUTIONCUMUL=8,
        TABBASE100=16,
        TABJOURSACT=32
    };

    enum TABLESMASK{
        T1=1,
        T2=2,
        T2BIS=4,
        T3=8,
        T3BIS=16,
        T4=32,
        T5=64,
        T6=128,
        T7=256,
        T8=512,
        T9=1024,
        T10=2048,
        T11=4096,
        T12=8192
    };

    PCx_Tables(PCx_AuditModel *model);

    QString getTabJoursAct(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getTabBase100(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getTabEvolutionCumul(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getTabEvolution(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getTabRecap(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getTabResults(unsigned int node) const;

    QString getCSS() const;

    //T1,T2... are named as in the original PCA version
    //Content of tab "Recap"
    QString getT1(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT4(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT8(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;

    //Content of tab "Evolution"
    QString getT2bis(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT3bis(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;

    //Content of tab "EvolutionCumul"
    QString getT2(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT3(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;

    //Content of tab "Base100"
    QString getT5(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT6(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;

    //Content of tab "Jours act."
    QString getT7(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;
    QString getT9(unsigned int node, PCx_AuditModel::DFRFDIRI mode) const;

    //Content of "Resultats" mode (without tabs)
    QString getT10(unsigned int node) const;
    QString getT11(unsigned int node) const;
    QString getT12(unsigned int node) const;

private:
    PCx_AuditModel *model;
};

#endif // PCX_TABLES_H
