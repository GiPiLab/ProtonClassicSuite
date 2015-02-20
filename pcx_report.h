#ifndef PCX_REPORT_H
#define PCX_REPORT_H

#include "pcx_tables.h"
#include "pcx_audit.h"
#include "pcx_reporting.h"
#include "pcx_graphics.h"

class PCx_Report
{
public:

    //Presets for PCR Reports, each of them except the last one contains tables and graphics
    enum PCRPRESETS
    {
        PCRPRESET_A,
        PCRPRESET_B,
        PCRPRESET_C,
        PCRPRESET_D,
        PCRPRESET_S
    };

    explicit PCx_Report(PCx_Audit *auditModel,QCustomPlot *plot=0,int graphicsWidth=650,int graphicsHeight=400,double scale=1.0);

    explicit PCx_Report(PCx_Reporting *reportingModel,QCustomPlot *plot=0,int graphicsWidth=650,int graphicsHeight=400,double scale=1.0);


    QString generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS> listOfTabs, QList<PCx_Tables::PCATABLES> listOfTables, QList<PCx_Graphics::PCAGRAPHICS> listOfGraphics,
                                      unsigned int selectedNode, MODES::DFRFDIRI mode, QTextDocument *document=nullptr, const QString &absoluteImagePath="",
                                      const QString &relativeImagePath="", QProgressDialog *progress=nullptr, const PCx_PrevisionItem *prevItem=nullptr) const;


    QString generateHTMLReportingReportForNode(QList<PCx_Report::PCRPRESETS> listOfPresets,unsigned int selectedNode, MODES::DFRFDIRI mode, bool includeGraphics=true, QTextDocument *document=nullptr,
                                               const QString &absoluteImagePath="",const QString &relativeImagePath="",QProgressDialog *progress=nullptr) const;

    QString generateHTMLTOC(QList<unsigned int> nodes) const;



    const PCx_Tables &getTables() const{return tables;}
    PCx_Graphics &getGraphics() {return graphics;}


private:
    PCx_Audit *auditModel;
    PCx_Reporting *reportingModel;
    PCx_Tables tables;
    PCx_Graphics graphics;

    PCx_Report(const PCx_Report &c);
    PCx_Report &operator=(const PCx_Report &);

};

#endif // PCX_REPORT_H
