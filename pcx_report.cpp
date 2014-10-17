#include "pcx_report.h"
#include "utils.h"
#include "pcx_query.h"

PCx_Report::PCx_Report(PCx_Audit *model,QCustomPlot *plot,int graphicsWidth,int graphicsHeight,double scale):model(model),tables(model),graphics(model,plot,graphicsWidth,graphicsHeight,scale)
{
    Q_ASSERT(model!=nullptr);
}




QString PCx_Report::generateHTMLReportForNode(QList<PCx_Tables::TABS> listOfTabs, QList<PCx_Tables::TABLES> listOfTables, QList<PCx_Graphics::GRAPHICS> listOfGraphics,
                                              unsigned int selectedNode, PCx_Audit::DFRFDIRI mode,QTextDocument *document,const QString &absoluteImagePath,
                                              const QString &relativeImagePath,QProgressDialog *progress) const
{
    Q_ASSERT(selectedNode>0);

    QString output;

    //Either group of tables, or individual tables
    if(!listOfTabs.isEmpty())
    {
        foreach(PCx_Tables::TABS tab,listOfTabs)
        {
            switch(tab)
            {
            case PCx_Tables::TABRECAP:
                output.append(tables.getTabRecap(selectedNode,mode));
                break;
            case PCx_Tables::TABEVOLUTION:
                output.append(tables.getTabEvolution(selectedNode,mode));
                break;
            case PCx_Tables::TABEVOLUTIONCUMUL:
                output.append(tables.getTabEvolutionCumul(selectedNode,mode));
                break;
            case PCx_Tables::TABBASE100:
                output.append(tables.getTabBase100(selectedNode,mode));
                break;
            case PCx_Tables::TABJOURSACT:
                output.append(tables.getTabJoursAct(selectedNode,mode));
                break;
            case PCx_Tables::TABRESULTS:
                output.append(tables.getTabResults(selectedNode));
                break;
            }
        }
    }
    else
    {
        foreach(PCx_Tables::TABLES table,listOfTables)
        {
            switch(table)
            {
            case PCx_Tables::T1:
                output.append(tables.getT1(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T2:
                output.append(tables.getT2(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T2BIS:
                output.append(tables.getT2bis(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T3:
                output.append(tables.getT3(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T3BIS:
                output.append(tables.getT3bis(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T4:
                output.append(tables.getT4(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T5:
                output.append(tables.getT5(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T6:
                output.append(tables.getT6(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T7:
                output.append(tables.getT7(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T8:
                output.append(tables.getT8(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T9:
                output.append(tables.getT9(selectedNode,mode)+"<br>");
                break;
            case PCx_Tables::T10:
                output.append(tables.getT10(selectedNode)+"<br>");
                break;
            case PCx_Tables::T11:
                output.append(tables.getT11(selectedNode)+"<br>");
                break;
            case PCx_Tables::T12:
                output.append(tables.getT12(selectedNode)+"<br>");
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
        foreach(PCx_Graphics::GRAPHICS graph,listOfGraphics)
        {
            switch(graph)
            {
            case PCx_Graphics::G1:
                output.append("<div align='center' class='g'><b>"+graphics.getG1(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G2:
                output.append("<div align='center' class='g'><b>"+graphics.getG2(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G3:
                output.append("<div align='center' class='g'><b>"+graphics.getG3(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G4:
                output.append("<div align='center' class='g'><b>"+graphics.getG4(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G5:
                output.append("<div align='center' class='g'><b>"+graphics.getG5(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G6:
                output.append("<div align='center' class='g'><b>"+graphics.getG6(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G7:
                output.append("<div align='center' class='g'><b>"+graphics.getG7(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G8:
                output.append("<div align='center' class='g'><b>"+graphics.getG8(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G9:
                output.append("<div align='center' class='g'><b>"+graphics.getG9(selectedNode)+"</b><br>");
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

        foreach(PCx_Graphics::GRAPHICS graph,listOfGraphics)
        {
            switch(graph)
            {
            case PCx_Graphics::G1:
                output.append("<div align='center' class='g'><b>"+graphics.getG1(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G2:
                output.append("<div align='center' class='g'><b>"+graphics.getG2(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G3:
                output.append("<div align='center' class='g'><b>"+graphics.getG3(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G4:
                output.append("<div align='center' class='g'><b>"+graphics.getG4(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G5:
                output.append("<div align='center' class='g'><b>"+graphics.getG5(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G6:
                output.append("<div align='center' class='g'><b>"+graphics.getG6(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G7:
                output.append("<div align='center' class='g'><b>"+graphics.getG7(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G8:
                output.append("<div align='center' class='g'><b>"+graphics.getG8(selectedNode,mode)+"</b><br>");
                break;
            case PCx_Graphics::G9:
                output.append("<div align='center' class='g'><b>"+graphics.getG9(selectedNode)+"</b><br>");
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

QString PCx_Report::generateHTMLTOC(QList<unsigned int> nodes) const
{
    QString output="<ul>\n";
    PCx_Tree *tree=model->getAttachedTree();
    foreach(unsigned int node,nodes)
    {
        output.append(QString("<li><a href='#node%1'>%2</a></li>\n").arg(node).arg(tree->getNodeName(node).toHtmlEscaped()));
    }
    output.append("</ul>\n");
    return output;
}

