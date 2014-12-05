#include "pcx_report.h"
#include "utils.h"
#include "pcx_query.h"

PCx_Report::PCx_Report(PCx_Audit *model,QCustomPlot *plot,int graphicsWidth,int graphicsHeight,double scale):auditModel(model),tables(model),graphics(model,plot,graphicsWidth,graphicsHeight,scale)
{
    Q_ASSERT(model!=nullptr);
    reportingModel=nullptr;
}

PCx_Report::PCx_Report(PCx_Reporting *reportingModel, QCustomPlot *plot, int graphicsWidth, int graphicsHeight, double scale):reportingModel(reportingModel),tables(reportingModel),graphics(reportingModel,plot,graphicsWidth,graphicsHeight,scale)
{
    auditModel=nullptr;
}

QString PCx_Report::generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS> listOfTabs, QList<PCx_Tables::PCATABLES> listOfTables, QList<PCx_Graphics::PCAGRAPHICS> listOfGraphics,
                                              unsigned int selectedNode, MODES::DFRFDIRI mode,QTextDocument *document,const QString &absoluteImagePath,
                                              const QString &relativeImagePath,QProgressDialog *progress) const
{
    Q_ASSERT(selectedNode>0);
    if(auditModel==nullptr)
    {
        qCritical()<<"Invalid null audit model";
        return QString();
    }

    QString output;

    //Either group of tables, or individual tables
    if(!listOfTabs.isEmpty())
    {
        foreach(PCx_Tables::PCAPRESETS tab,listOfTabs)
        {
            switch(tab)
            {
            case PCx_Tables::PCAOVERVIEW:
                output.append(tables.getPCAPresetOverview(selectedNode,mode));
                break;
            case PCx_Tables::PCAEVOLUTION:
                output.append(tables.getPCAPresetEvolution(selectedNode,mode));
                break;
            case PCx_Tables::PCAEVOLUTIONCUMUL:
                output.append(tables.getPCAPresetEvolutionCumul(selectedNode,mode));
                break;
            case PCx_Tables::PCABASE100:
                output.append(tables.getPCAPresetBase100(selectedNode,mode));
                break;
            case PCx_Tables::PCADAYOFWORK:
                output.append(tables.getPCAPresetDayOfWork(selectedNode,mode));
                break;
            case PCx_Tables::PCARESULTS:
                output.append(tables.getPCAPresetResults(selectedNode));
                break;
            }
        }
    }
    else
    {
        foreach(PCx_Tables::PCATABLES table,listOfTables)
        {
            switch(table)
            {
            case PCx_Tables::PCAT1:
                output.append(tables.getPCAT1(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT2:
                output.append(tables.getPCAT2(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT2BIS:
                output.append(tables.getPCAT2bis(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT3:
                output.append(tables.getPCAT3(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT3BIS:
                output.append(tables.getPCAT3bis(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT4:
                output.append(tables.getPCAT4(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT5:
                output.append(tables.getPCAT5(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT6:
                output.append(tables.getPCAT6(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT7:
                output.append(tables.getPCAT7(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT8:
                output.append(tables.getPCAT8(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT9:
                output.append(tables.getPCAT9(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCAT10:
                output.append(tables.getPCAT10(selectedNode)+"<br>");
                break;
            case PCx_Tables::PCAT11:
                output.append(tables.getPCAT11(selectedNode)+"<br>");
                break;
            case PCx_Tables::PCAT12:
                output.append(tables.getPCAT12(selectedNode)+"<br>");
                break;
            }
        }
    }

    QCustomPlot *plot=graphics.getPlot();
    int graphicsWidth=graphics.getGraphicsWidth();
    int graphicsHeight=graphics.getGraphicsHeight();
    double scale=graphics.getScale();


    //Inline graphics mode
    if(document!=nullptr)
    {
        foreach(PCx_Graphics::PCAGRAPHICS graph,listOfGraphics)
        {
            switch(graph)
            {
            case PCx_Graphics::PCAG1:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG1(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG2:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG2(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG3:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG3(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG4:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG4(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG5:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG5(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG6:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG6(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG7:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG7(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG8:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG8(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG9:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG9(selectedNode)+"</b><br>");
                break;
            }
            QString name="mydata://"+QString::number(qrand());
            document->addResource(QTextDocument::ImageResource,QUrl(name),QVariant(plot->toPixmap(graphicsWidth,graphicsHeight,scale)));
            output.append(QString("<img width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(name));
        }
    }

    //Export mode
    else
    {
        QSettings settings;
        QString imageFormat=settings.value("output/imageFormat","png").toString();
        const char *imgFormat="png";
        int quality=-1;
        if(imageFormat=="png")
        {
            imgFormat="png";
            quality=-1;
        }

        //FIXME : save to JPG as a workaround because saving to PNG is very slow
        else if(imageFormat=="jpg")
        {
            imgFormat="jpeg";
            quality=96;
        }
        else
        {
            qCritical()<<"Invalid image format";
            die();
        }
        QString suffix="."+imageFormat;

        int progressValue=0;
        if(progress!=nullptr)
        {
            progressValue=progress->value();
        }

        if(absoluteImagePath.isEmpty()|| relativeImagePath.isEmpty())
        {
            qWarning()<<"Please pass an absolute and relative path to store images";
            die();
        }

        foreach(PCx_Graphics::PCAGRAPHICS graph,listOfGraphics)
        {
            switch(graph)
            {
            case PCx_Graphics::PCAG1:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG1(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG2:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG2(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG3:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG3(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG4:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG4(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG5:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG5(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG6:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG6(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG7:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG7(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG8:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG8(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::PCAG9:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG9(selectedNode)+"</b><br>");
                break;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents,100);

            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(relativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");

            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }
            output.append(QString("<img width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);
        }
    }
    return output;
}

QString PCx_Report::generateHTMLReportingReportForNode(QList<PCx_Report::PCRPRESETS> listOfPresets, unsigned int selectedNode, MODES::DFRFDIRI mode, QTextDocument *document, const QString &absoluteImagePath, const QString &relativeImagePath, QProgressDialog *progress) const
{
    if(reportingModel==nullptr)
    {
        qCritical()<<"NULL model error";
        return QString();
    }
    bool inlineImageMode=false;

    if(document!=nullptr)
        inlineImageMode=true;

    else if(absoluteImagePath.isEmpty()|| relativeImagePath.isEmpty())
    {
        qWarning()<<"Please pass an absolute and relative path to store images";
        die();
    }

    QSettings settings;
    QString imageFormat=settings.value("output/imageFormat","png").toString();
    const char *imgFormat="png";
    int quality=-1;
    if(imageFormat=="png")
    {
        imgFormat="png";
        quality=-1;
    }

    //FIXME : save to JPG as a workaround because saving to PNG is very slow
    else if(imageFormat=="jpg")
    {
        imgFormat="jpeg";
        quality=96;
    }
    else
    {
        qCritical()<<"Invalid image format";
        die();
    }
    QString suffix="."+imageFormat;

    int progressValue=0;
    if(progress!=nullptr)
    {
        progressValue=progress->value();
    }

    QCustomPlot *plot=graphics.getPlot();
    //Reduce size a little bit
    int graphicsWidth=graphics.getGraphicsWidth()/1.2;
    int graphicsHeight=graphics.getGraphicsHeight()/1.2;
    double scale=graphics.getScale();

    QString nodeName=reportingModel->getAttachedTree()->getNodeName(selectedNode);
    QString modeName=MODES::modeToCompleteString(mode);
    QString output=QString("<h2>%1 - %2</h2>").arg(nodeName.toHtmlEscaped())
            .arg(modeName);

    output.append(tables.getPCRRatioParents(selectedNode,mode));

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_S))
    {
        output.append("<h2>Synthèse</h2><h4>Synthèse de fonctionnement (RF-DF)</h4>");
        output.append(tables.getPCRRFDF(selectedNode));
        output.append("<h4>Synthèse d'investissement (RI-DI)</h4>");
        output.append(tables.getPCRRIDI(selectedNode));
        output.append("<h4>Synthèse globale (RF-DF)+(RI-DI)</h4>");
        output.append(tables.getPCRRFDFRIDI(selectedNode));
    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_A))
    {
        output.append("<h4>Provenance des crédits</h4>");
        output.append(tables.getPCRProvenance(selectedNode,mode));

        QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::BP,
                                                    PCx_Reporting::REPORTS,
                                                    PCx_Reporting::OCDM,
                                                    PCx_Reporting::VCDM,
                                                    PCx_Reporting::VIREMENTSINTERNES
                                                   };
        graphics.getPCRHistoPercent(selectedNode,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Provenance des crédits de %1\n(%2)").arg(nodeName).arg(modeName),graphics.getColorDFBar());

        if(inlineImageMode)
        {
            QString name="mydata://"+QString::number(qrand());
             document->addResource(QTextDocument::ImageResource,QUrl(name),QVariant(plot->toPixmap(graphicsWidth,graphicsHeight,1.0)));
             output.append(QString("<br><div class='g' align='center'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                           .arg(graphicsWidth).arg(graphicsHeight).arg(name));

        }
        else
        {
            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(relativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");

            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }
            output.append(QString("<br><div align='center' class='g'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);

        }
    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_B))
    {
        output.append("<h4>Facteurs de variation des crédits</h4>");
        output.append(tables.getPCRVariation(selectedNode,mode));

        QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::OCDM,
                                                    PCx_Reporting::VCDM,
                                                    PCx_Reporting::VIREMENTSINTERNES
                                                   };
        graphics.getPCRHistoPercent(selectedNode,mode,selectedORED,PCx_Reporting::OREDPCR::BP,QString("Facteurs de variation des crédits de %1\n(%2)").arg(nodeName).arg(modeName),graphics.getColorRFBar());

        if(inlineImageMode)
        {
            QString name="mydata://"+QString::number(qrand());
             document->addResource(QTextDocument::ImageResource,QUrl(name),QVariant(plot->toPixmap(graphicsWidth,graphicsHeight,1.0)));
             output.append(QString("<br><div class='g' align='center'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                           .arg(graphicsWidth).arg(graphicsHeight).arg(name));

        }
        else
        {
            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(relativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");

            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }
            output.append(QString("<br><div align='center' class='g'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);

        }
    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_C))
    {
        output.append("<h4>Utilisation des crédits</h4>");
        output.append(tables.getPCRUtilisation(selectedNode,mode));

        QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::REALISES,
                                                    PCx_Reporting::ENGAGES,
                                                    PCx_Reporting::DISPONIBLES
                                                   };
        graphics.getPCRHistoPercent(selectedNode,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Utilisation des crédits de %1\n(%2)").arg(nodeName).arg(modeName),graphics.getColorDIBar());

        if(inlineImageMode)
        {
            QString name="mydata://"+QString::number(qrand());
             document->addResource(QTextDocument::ImageResource,QUrl(name),QVariant(plot->toPixmap(graphicsWidth,graphicsHeight,1.0)));
             output.append(QString("<br><div class='g' align='center'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                           .arg(graphicsWidth).arg(graphicsHeight).arg(name));

        }
        else
        {
            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(relativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");

            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }
            output.append(QString("<br><div align='center' class='g'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);

        }

    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_D))
    {
        output.append("<h4>Détection des cycles et des écarts</h4>");
        output.append(tables.getPCRCycles(selectedNode,mode));
        QList<PCx_Reporting::OREDPCR> oredPCR={PCx_Reporting::OREDPCR::OUVERTS,PCx_Reporting::OREDPCR::REALISES};
        graphics.getPCRG1(selectedNode,mode,oredPCR);

        if(inlineImageMode)
        {
            QString name="mydata://"+QString::number(qrand());
             document->addResource(QTextDocument::ImageResource,QUrl(name),QVariant(plot->toPixmap(graphicsWidth,graphicsHeight,1.0)));
             output.append(QString("<br><div class='g' align='center'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                           .arg(graphicsWidth).arg(graphicsHeight).arg(name));

        }
        else
        {
            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(relativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");

            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }
            output.append(QString("<br><div align='center' class='g'><img align='center' width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);

        }
    }

    return output;
}

QString PCx_Report::generateHTMLTOC(QList<unsigned int> nodes) const
{
    QString output="<ul>\n";
    PCx_Tree *tree=auditModel->getAttachedTree();
    foreach(unsigned int node,nodes)
    {
        output.append(QString("<li><a href='#node%1'>%2</a></li>\n").arg(node).arg(tree->getNodeName(node).toHtmlEscaped()));
    }
    output.append("</ul>\n");
    return output;
}

