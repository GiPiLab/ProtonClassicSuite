/*
* ProtonClassicSuite
* 
* Copyright Thibault et Gilbert Mondary, Laboratoire de Recherche pour le Développement Local (2006--)
* 
* labo@gipilab.org
* 
* Ce logiciel est un programme informatique servant à cerner l'ensemble des données budgétaires
* de la collectivité territoriale (audit, reporting infra-annuel, prévision des dépenses à venir)
* 
* 
* Ce logiciel est régi par la licence CeCILL soumise au droit français et
* respectant les principes de diffusion des logiciels libres. Vous pouvez
* utiliser, modifier et/ou redistribuer ce programme sous les conditions
* de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
* sur le site "http://www.cecill.info".
* 
* En contrepartie de l'accessibilité au code source et des droits de copie,
* de modification et de redistribution accordés par cette licence, il n'est
* offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
* seule une responsabilité restreinte pèse sur l'auteur du programme, le
* titulaire des droits patrimoniaux et les concédants successifs.
* 
* A cet égard l'attention de l'utilisateur est attirée sur les risques
* associés au chargement, à l'utilisation, à la modification et/ou au
* développement et à la reproduction du logiciel par l'utilisateur étant 
* donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
* manipuler et qui le réserve donc à des développeurs et des professionnels
* avertis possédant des connaissances informatiques approfondies. Les
* utilisateurs sont donc invités à charger et tester l'adéquation du
* logiciel à leurs besoins dans des conditions permettant d'assurer la
* sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
* à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 
* 
* Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
* pris connaissance de la licence CeCILL, et que vous en avez accepté les
* termes.
*
*/

#include "pcx_report.h"
#include "utils.h"
#include "pcx_query.h"


PCx_Report::PCx_Report(PCx_Audit *model,QCustomPlot *plot,int graphicsWidth,int graphicsHeight,double scale):auditModel(model),tables(model),graphics(model,plot,graphicsWidth,graphicsHeight,scale)
{
    if(model==nullptr)
    {
        qFatal("Assertion failed");
    }
    reportingModel=nullptr;
}

PCx_Report::PCx_Report(PCx_Reporting *reportingModel, QCustomPlot *plot, int graphicsWidth, int graphicsHeight, double scale):reportingModel(reportingModel),tables(reportingModel),graphics(reportingModel,plot,graphicsWidth,graphicsHeight,scale)
{
    auditModel=nullptr;
}

QString PCx_Report::generateHTMLAuditReportForNode(QList<PCx_Tables::PCAPRESETS> listOfTabs, QList<PCx_Tables::PCATABLES> listOfTables, QList<PCx_Graphics::PCAGRAPHICS> listOfGraphics,
                                                   unsigned int selectedNode, MODES::DFRFDIRI mode, unsigned int referenceNode, QTextDocument *document, const QString &absoluteImagePath,
                                                   const QString &relativeImagePath, QProgressDialog *progress,const PCx_PrevisionItem *prevItem) const
{
    if(selectedNode==0 || referenceNode==0 || auditModel==nullptr)
    {
        qFatal("Assertion failed");
    }

    QString output="\n<div class='auditNodeContainer'>\n";

    //Either group of tables, or individual tables
    if(!listOfTabs.isEmpty())
    {
        foreach(PCx_Tables::PCAPRESETS tab,listOfTabs)
        {
            switch(tab)
            {
            case PCx_Tables::PCAPRESETS::PCAOVERVIEW:
                output.append(tables.getPCAPresetOverview(selectedNode,mode,referenceNode));
                break;
            case PCx_Tables::PCAPRESETS::PCAEVOLUTION:
                output.append(tables.getPCAPresetEvolution(selectedNode,mode,referenceNode));
                break;
            case PCx_Tables::PCAPRESETS::PCAEVOLUTIONCUMUL:
                output.append(tables.getPCAPresetEvolutionCumul(selectedNode,mode,referenceNode));
                break;
            case PCx_Tables::PCAPRESETS::PCABASE100:
                output.append(tables.getPCAPresetBase100(selectedNode,mode,referenceNode));
                break;
            case PCx_Tables::PCAPRESETS::PCADAYOFWORK:
                output.append(tables.getPCAPresetDayOfWork(selectedNode,mode));
                break;
            case PCx_Tables::PCAPRESETS::PCARESULTS:
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
            case PCx_Tables::PCATABLES::PCARAWDATA:
                output.append(tables.getPCARawData(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT1:
                output.append(tables.getPCAT1(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT2:
                output.append(tables.getPCAT2(selectedNode,mode,referenceNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT2BIS:
                output.append(tables.getPCAT2bis(selectedNode,mode,referenceNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT3:
                output.append(tables.getPCAT3(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT3BIS:
                output.append(tables.getPCAT3bis(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT4:
                output.append(tables.getPCAT4(selectedNode,mode,referenceNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT5:
                output.append(tables.getPCAT5(selectedNode,mode,referenceNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT6:
                output.append(tables.getPCAT6(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT7:
                output.append(tables.getPCAT7(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT8:
                output.append(tables.getPCAT8(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT9:
                output.append(tables.getPCAT9(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT10:
                output.append(tables.getPCAT10(selectedNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT11:
                output.append(tables.getPCAT11(selectedNode)+"<br>");
                break;
            case PCx_Tables::PCATABLES::PCAT12:
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
            case PCx_Graphics::PCAGRAPHICS::PCAHISTORY:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAHistory(selectedNode,mode,{PCx_Audit::ORED::OUVERTS,PCx_Audit::ORED::REALISES,PCx_Audit::ORED::ENGAGES},prevItem,false)+"</b><br>");
                break;

            case PCx_Graphics::PCAGRAPHICS::PCAG1:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG1(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG2:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG2(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG3:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG3(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG4:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG4(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG5:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG5(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG6:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG6(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG7:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG7(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG8:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG8(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG9:
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
        QString encodedRelativeImagePath=QUrl::toPercentEncoding(relativeImagePath);

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

        if(absoluteImagePath.isEmpty()|| encodedRelativeImagePath.isEmpty())
        {
            qWarning()<<"Please pass an absolute and relative path to store images";
            die();
        }

        foreach(PCx_Graphics::PCAGRAPHICS graph,listOfGraphics)
        {
            switch(graph)
            {
            case PCx_Graphics::PCAGRAPHICS::PCAHISTORY:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAHistory(selectedNode,mode,{PCx_Audit::ORED::OUVERTS,PCx_Audit::ORED::REALISES,PCx_Audit::ORED::ENGAGES},prevItem,false)+"</b><br>");
                break;

            case PCx_Graphics::PCAGRAPHICS::PCAG1:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG1(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG2:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG2(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG3:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG3(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG4:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG4(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG5:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG5(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG6:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG6(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG7:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG7(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG8:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG8(selectedNode,mode,prevItem,referenceNode)+"</b><br>");
                break;
            case PCx_Graphics::PCAGRAPHICS::PCAG9:
                output.append("<div align='center' class='g'><b>"+graphics.getPCAG9(selectedNode)+"</b><br>");
                break;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents,100);

            QString imageName=generateUniqueFileName(suffix);
            QString imageAbsoluteName=imageName;
            imageName.prepend(encodedRelativeImagePath+"/");
            imageAbsoluteName.prepend(absoluteImagePath+"/");


            if(!plot->saveRastered(imageAbsoluteName,graphicsWidth,graphicsHeight,scale,imgFormat,quality))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }

            /*if(!plot->saveSvg(imageAbsoluteName,graphicsWidth,graphicsHeight))
            {
                qCritical()<<"Unable to save "<<imageAbsoluteName;
                die();
            }*/

            output.append(QString("<img width='%1' height='%2' alt='GRAPH' src='%3'></div><br>")
                          .arg(graphicsWidth).arg(graphicsHeight).arg(imageName));
            if(progress!=nullptr)
                progress->setValue(++progressValue);
        }
    }
    output.append("\n</div>\n");
    return output;
}

QString PCx_Report::generateHTMLReportingReportForNode(QList<PCx_Report::PCRPRESETS> listOfPresets, unsigned int selectedNode, MODES::DFRFDIRI mode, bool includeGraphics, QTextDocument *document, const QString &absoluteImagePath, const QString &relativeImagePath, QProgressDialog *progress) const
{
    if(reportingModel==nullptr)
    {
        qCritical()<<"NULL model error";
        return QString();
    }
    bool inlineImageMode=false;


    QString encodedRelativeImagePath=QUrl::toPercentEncoding(relativeImagePath);

    if(document!=nullptr)
        inlineImageMode=true;

    else if(absoluteImagePath.isEmpty()|| encodedRelativeImagePath.isEmpty())
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

    QString output="\n<div class='reportingNodeContainer'>\n";

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_S))
    {
        output.append("<h4>Synthèse de fonctionnement (RF-DF)</h4>");
        output.append(tables.getPCRRFDF(selectedNode));
        output.append("<h4>Synthèse d'investissement (RI-DI)</h4>");
        output.append(tables.getPCRRIDI(selectedNode));
        output.append("<h4>Synthèse globale (RF-DF)+(RI-DI)</h4>");
        output.append(tables.getPCRRFDFRIDI(selectedNode));
        listOfPresets.removeAll(PCRPRESETS::PCRPRESET_S);
    }

    if(listOfPresets.isEmpty())
        return output;

    QString modeName=MODES::modeToCompleteString(mode);

    output.append(QString("<h2>%1</h2>").arg(modeName));

    output.append(tables.getPCRRatioParents(selectedNode,mode));

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_A))
    {
        output.append("<h4>Provenance des crédits</h4>");
        output.append(tables.getPCRProvenance(selectedNode,mode));

        if(includeGraphics)
        {
            graphics.getPCRProvenance(selectedNode,mode);

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
                imageName.prepend(encodedRelativeImagePath+"/");
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

    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_B))
    {
        output.append("<h4>Facteurs de variation des crédits</h4>");
        output.append(tables.getPCRVariation(selectedNode,mode));

        if(includeGraphics)
        {
            graphics.getPCRVariation(selectedNode,mode);


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
                imageName.prepend(encodedRelativeImagePath+"/");
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
    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_C))
    {
        output.append("<h4>Utilisation des crédits</h4>");
        output.append(tables.getPCRUtilisation(selectedNode,mode));

        if(includeGraphics)
        {
            graphics.getPCRUtilisation(selectedNode,mode);

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
                imageName.prepend(encodedRelativeImagePath+"/");
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

    }

    if(listOfPresets.contains(PCRPRESETS::PCRPRESET_D))
    {
        output.append("<h4>Détection des cycles et des écarts</h4>");
        output.append(tables.getPCRCycles(selectedNode,mode));

        if(includeGraphics)
        {
            graphics.getPCRCycles(selectedNode,mode);

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
                imageName.prepend(encodedRelativeImagePath+"/");
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
    }

    output.append("\n</div>\n");
    return output;
}

QString PCx_Report::generateHTMLTOC(QList<unsigned int> nodes) const
{
    QString output="<ul>\n";
    PCx_Tree *tree=nullptr;
    if(auditModel!=nullptr)
        tree=auditModel->getAttachedTree();
    else if(reportingModel!=nullptr)
    {
        tree=reportingModel->getAttachedTree();
    }

    if(tree==nullptr)
    {
        qWarning()<<"Invalid model used";
        return QString();
    }

    foreach(unsigned int node,nodes)
    {
        output.append(QString("<li><a href='#node%1'>%2</a></li>\n").arg(node).arg(tree->getNodeName(node).toHtmlEscaped()));
    }
    output.append("</ul>\n");
    return output;
}

