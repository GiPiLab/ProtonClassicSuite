#ifndef PCX_GRAPHICS_H
#define PCX_GRAPHICS_H

#include "pcx_auditmodel.h"
#include "QCustomPlot/qcustomplot.h"

class PCx_Graphics
{
public:

    static const int MAXWIDTH=1024;
    static const int MAXHEIGHT=1024;
    static const int MINWIDTH=350;
    static const int MINHEIGHT=350;
    static const double MINSCALE=1.0;
    static const double MAXSCALE=4.0;
    static const int DEFAULTWIDTH=650;
    static const int DEFAULTHEIGHT=400;
    static const double DEFAULTSCALE=2.0;

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

    PCx_Graphics(PCx_AuditModel *model, QCustomPlot *plot=0, int graphicsWidth=DEFAULTWIDTH, int graphicsHeight=DEFAULTHEIGHT, double scale=1.0);
    virtual ~PCx_Graphics();

    QString getG1(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::ouverts,false);}
    QString getG3(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::realises,false);}
    QString getG5(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::engages,false);}
    QString getG7(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::disponibles,false);}

    QString getG2(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::ouverts,true);}
    QString getG4(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::realises,true);}
    QString getG6(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::engages,true);}
    QString getG8(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::disponibles,true);}

    QString getG9(unsigned int node) const;

    QCustomPlot * getPlot() const{return plot;}
    int getGraphicsWidth() const{return graphicsWidth;}
    int getGraphicsHeight() const{return graphicsHeight;}
    double getScale() const{return scale;}

    void setGraphicsWidth(int width);
    void setGraphicsHeight(int height);
    void setScale(double scale);


private:
    PCx_AuditModel *model;

    //Returns the title of the graphic in html
    QString getG1G8(unsigned int node, PCx_AuditModel::DFRFDIRI mode, PCx_AuditModel::ORED modeORED, bool cumule) const;
    bool ownPlot;
    int graphicsWidth,graphicsHeight;
    double scale;

    QCustomPlot *plot;

};

#endif // PCX_GRAPHICS_H
