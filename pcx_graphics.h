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
    static const double MAXSCALE=3.0;
    static const int DEFAULTWIDTH=650;
    static const int DEFAULTHEIGHT=400;
    static const double DEFAULTSCALE=1.0;

    static const unsigned int DEFAULTPENCOLOR1=0xff0000;
    static const unsigned int DEFAULTPENCOLOR2=0x0000ff;

    static const unsigned int DEFAULTCOLORDFBAR=0xff0000;
    static const unsigned int DEFAULTCOLORRFBAR=0x00ff00;
    static const unsigned int DEFAULTCOLORDIBAR=0x0000ff;
    static const unsigned int DEFAULTCOLORRIBAR=0xffff00;

    static const int DEFAULTALPHA=70;

    enum GRAPHICS
    {
        G1,
        G2,
        G3,
        G4,
        G5,
        G6,
        G7,
        G8,
        G9
    };

    PCx_Graphics(PCx_AuditModel *model, QCustomPlot *plot=0, int graphicsWidth=DEFAULTWIDTH, int graphicsHeight=DEFAULTHEIGHT, double scale=1.0);
    virtual ~PCx_Graphics();

    QString getG1(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::OUVERTS,false);}
    QString getG3(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::REALISES,false);}
    QString getG5(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::ENGAGES,false);}
    QString getG7(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::DISPONIBLES,false);}

    QString getG2(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::OUVERTS,true);}
    QString getG4(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::REALISES,true);}
    QString getG6(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::ENGAGES,true);}
    QString getG8(unsigned int node,PCx_AuditModel::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditModel::DISPONIBLES,true);}

    QString getG9(unsigned int node) const;

    QCustomPlot * getPlot() const{return plot;}
    int getGraphicsWidth() const{return graphicsWidth;}
    int getGraphicsHeight() const{return graphicsHeight;}
    double getScale() const{return scale;}

    void setGraphicsWidth(int width);
    void setGraphicsHeight(int height);
    void setScale(double scale);

    static QColor getColorPen1();
    static QColor getColorPen2();

    static QColor getColorDFBar();
    static QColor getColorRFBar();
    static QColor getColorDIBar();
    static QColor getColorRIBar();

    static int getAlpha();

    static QString getCSS();


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
