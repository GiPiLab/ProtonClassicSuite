#ifndef PCX_REPORT_H
#define PCX_REPORT_H

#include "pcx_tables.h"
#include "pcx_audit.h"
#include "pcx_graphics.h"

class PCx_Report
{
public:
    PCx_Report(PCx_Audit *model,QCustomPlot *plot=0,int graphicsWidth=650,int graphicsHeight=400,double scale=1.0);

    static QString getCSS();

    QString generateHTMLReportForNode(QList<PCx_Tables::TABS> listOfTabs, QList<PCx_Tables::TABLES> listOfTables, QList<PCx_Graphics::GRAPHICS> listOfGraphics,
                                      unsigned int selectedNode, PCx_Audit::DFRFDIRI mode,QTextDocument *document=NULL, const QString &absoluteImagePath="",
                                      const QString &relativeImagePath="",QProgressDialog *progress=NULL) const;


    static QString generateHTMLHeader(unsigned int auditId);

    const PCx_Tables &getTables() const{return tables;}
    PCx_Graphics &getGraphics() {return graphics;}


private:
    PCx_Audit *model;
    PCx_Tables tables;
    PCx_Graphics graphics;

};

#endif // PCX_REPORT_H
