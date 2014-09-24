#ifndef PCX_GRAPHICS_H
#define PCX_GRAPHICS_H

#include "pcx_audit.h"
#include "QCustomPlot/qcustomplot.h"

class PCx_Graphics
{
public:

    static constexpr int MAXWIDTH=1024;
    static constexpr int MAXHEIGHT=1024;
    static constexpr int MINWIDTH=350;
    static constexpr int MINHEIGHT=350;
    static constexpr double MINSCALE=1.0;
    static constexpr double MAXSCALE=3.0;
    static constexpr int DEFAULTWIDTH=650;
    static constexpr int DEFAULTHEIGHT=400;
    static constexpr double DEFAULTSCALE=1.0;

    static constexpr unsigned int DEFAULTPENCOLOR1=0xff0000;
    static constexpr unsigned int DEFAULTPENCOLOR2=0x0000ff;

    static constexpr unsigned int DEFAULTCOLORDFBAR=0xff0000;
    static constexpr unsigned int DEFAULTCOLORRFBAR=0x00ff00;
    static constexpr unsigned int DEFAULTCOLORDIBAR=0x0000ff;
    static constexpr unsigned int DEFAULTCOLORRIBAR=0xffff00;

    static constexpr int DEFAULTALPHA=70;

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

    PCx_Graphics(PCx_Audit *model, QCustomPlot *plot=0, int graphicsWidth=DEFAULTWIDTH, int graphicsHeight=DEFAULTHEIGHT, double scale=1.0);
    virtual ~PCx_Graphics();

    QString getG1(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::OUVERTS,false);}
    QString getG3(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::REALISES,false);}
    QString getG5(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::ENGAGES,false);}
    QString getG7(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::DISPONIBLES,false);}

    QString getG2(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::OUVERTS,true);}
    QString getG4(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::REALISES,true);}
    QString getG6(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::ENGAGES,true);}
    QString getG8(unsigned int node,PCx_AuditManage::DFRFDIRI mode) const {return getG1G8(node,mode,PCx_AuditManage::DISPONIBLES,true);}

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
    PCx_Audit *model;

    //Returns the title of the graphic in html
    QString getG1G8(unsigned int node, PCx_AuditManage::DFRFDIRI mode, PCx_AuditManage::ORED modeORED, bool cumule) const;
    bool ownPlot;
    int graphicsWidth,graphicsHeight;
    double scale;

    QCustomPlot *plot;

    PCx_Graphics(const PCx_Graphics &c);
    PCx_Graphics &operator=(const PCx_Graphics &);

};

#endif // PCX_GRAPHICS_H
