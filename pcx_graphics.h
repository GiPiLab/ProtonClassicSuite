#ifndef PCX_GRAPHICS_H
#define PCX_GRAPHICS_H

#include "pcx_auditmodel.h"
#include "QCustomPlot/qcustomplot.h"

class PCx_Graphics
{
public:

    //MASKS FOR TABLE AND GRAPHICS BITSETS


    enum GRAPHICSMASK
    {
        G1=1,
        G2=2,
        G3=4,
        G4=8,
        G5=16,
        G6=32,
        G7=64,
        G8=128,
        G9=256
    };

    PCx_Graphics(PCx_AuditModel *model);

    QString getG1(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::ouverts,false,plot);}
    QString getG3(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::realises,false,plot);}
    QString getG5(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::engages,false,plot);}
    QString getG7(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::disponibles,false,plot);}

    QString getG2(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::ouverts,true,plot);}
    QString getG4(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::realises,true,plot);}
    QString getG6(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::engages,true,plot);}
    QString getG8(unsigned int node,PCx_AuditModel::DFRFDIRI mode, QCustomPlot *plot) const {return getG1G8(node,mode,PCx_AuditModel::disponibles,true,plot);}

    QString getG9(unsigned int node, QCustomPlot *plot) const;



private:
    PCx_AuditModel *model;
    //Returns the title of the graphic in html
    QString getG1G8(unsigned int node, PCx_AuditModel::DFRFDIRI mode, PCx_AuditModel::ORED modeORED, bool cumule, QCustomPlot *plot) const;

};

#endif // PCX_GRAPHICS_H
