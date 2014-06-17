#ifndef PCX_REPORT_H
#define PCX_REPORT_H

#include "pcx_tables.h"
#include "pcx_auditmodel.h"
#include "pcx_graphics.h"

class PCx_Report
{
public:
    PCx_Report(PCx_AuditModel *model,QCustomPlot *plot=0,int graphicsWidth=650,int graphicsHeight=400,double scale=1.0);

    QString getCSS() const;
    QString generateHTMLReportForNode(quint8 bitFieldPagesOfTables, quint16 bitFieldTables, quint16 bitFieldGraphics, unsigned int selectedNode, PCx_AuditModel::DFRFDIRI mode,
                                       QTextDocument *document=NULL, const QString &absoluteImagePath="", const QString &relativeImagePath="",QProgressDialog *progress=NULL) const;

    QString generateHTMLHeader() const;
    const PCx_Tables &getTables() const{return tables;}
    PCx_Graphics &getGraphics() {return graphics;}


private:
    PCx_AuditModel *model;
    PCx_Tables tables;
    PCx_Graphics graphics;

};

#endif // PCX_REPORT_H
