#ifndef PCX_GRAPHICS_H
#define PCX_GRAPHICS_H

#include "pcx_audit.h"
#include "pcx_reporting.h"
#include "QCustomPlot/qcustomplot.h"

class PCx_Graphics
{
public:

    static const int MAXWIDTH=1024;
    static const int MAXHEIGHT=1024;
    static const int MINWIDTH=350;
    static const int MINHEIGHT=350;
    static constexpr double MINSCALE=1.0;
    static constexpr double MAXSCALE=3.0;
    static const int DEFAULTWIDTH=650;
    static const int DEFAULTHEIGHT=400;
    static constexpr double DEFAULTSCALE=1.0;

    static const unsigned int DEFAULTPENCOLOR1=0xff0000;
    static const unsigned int DEFAULTPENCOLOR2=0x0000ff;

    static const unsigned int DEFAULTCOLORDFBAR=0xff0000;
    static const unsigned int DEFAULTCOLORRFBAR=0x00ff00;
    static const unsigned int DEFAULTCOLORDIBAR=0x0000ff;
    static const unsigned int DEFAULTCOLORRIBAR=0xffff00;

    static const int DEFAULTALPHA=70;

    enum PCAGRAPHICS
    {
        PCAG1,
        PCAG2,
        PCAG3,
        PCAG4,
        PCAG5,
        PCAG6,
        PCAG7,
        PCAG8,
        PCAG9
    };

    enum PCRGRAPHICS
    {
        PCRHISTORY,
        PCRGPROVENANCE,
        PCRGVARIATION,
        PCRGUTILISATION,
        PCRCYCLES
    };

    PCx_Graphics(PCx_Audit *auditModel, QCustomPlot *plot=0, int graphicsWidth=DEFAULTWIDTH, int graphicsHeight=DEFAULTHEIGHT, double scale=1.0);
    PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot=0, int graphicsWidth=DEFAULTWIDTH, int graphicsHeight=DEFAULTHEIGHT, double scale=1.0);
    virtual ~PCx_Graphics();

    QString getPCAG1(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::OUVERTS,false);}
    QString getPCAG3(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::REALISES,false);}
    QString getPCAG5(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::ENGAGES,false);}
    QString getPCAG7(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::DISPONIBLES,false);}

    QString getPCAG2(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::OUVERTS,true);}
    QString getPCAG4(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::REALISES,true);}
    QString getPCAG6(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::ENGAGES,true);}
    QString getPCAG8(unsigned int node,MODES::DFRFDIRI mode) const {return getPCAG1G8(node,mode,PCx_Audit::ORED::DISPONIBLES,true);}

    QString getPCAG9(unsigned int node) const;


    //History line plot with selected OREDPCR
    QString getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR)const;

    QString getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode) const;
    QString getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode) const;
    QString getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode) const;
    QString getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode) const;


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
    PCx_Audit *auditModel;
    PCx_Reporting *reportingModel;

    //Returns the title of the graphic in html
    QString getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule) const;

    //Histogram comparing percents of selected OREDs regarding to a reference for the last reporting date
    QString getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference, const QString &plotTitle, QColor color)const;
    bool ownPlot;
    int graphicsWidth,graphicsHeight;
    double scale;

    QCustomPlot *plot;

    PCx_Graphics(const PCx_Graphics &c);
    PCx_Graphics &operator=(const PCx_Graphics &);

};

#endif // PCX_GRAPHICS_H
