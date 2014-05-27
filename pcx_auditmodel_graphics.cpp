#include "pcx_auditmodel.h"
#include "utils.h"
#include <float.h>


QString PCx_AuditModel::getG1G8(unsigned int node, DFRFDIRI mode, ORED modeORED, bool cumule, QCustomPlot *plot) const
{
    Q_ASSERT(node>0 && plot!=NULL);
    QString tableName=modetoTableString(mode);
    QString oredName=OREDtoTableString(modeORED);

    QSqlQuery q;

    //Will contain data read from db
    QMap<double, double> dataRoot,dataNode;

    q.prepare(QString("select * from audit_%1_%2 where id_node=:id or id_node=1 order by annee").arg(tableName).arg(auditId));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    unsigned int firstYear=0;

    while(q.next())
    {
        //Plotting requires double
        double annee=q.value("annee").toDouble();
        if(firstYear==0)
            firstYear=annee;

        double data=q.value(oredName).toDouble();

        if(q.value("id_node").toUInt()==1)
        {
            dataRoot.insert(annee,data);
            if(node==1)
                dataNode.insert(annee,data);
        }
        else
        {
            dataNode.insert(annee,data);
        }
    }

    //dataRoot and dataNode must have the same keys
    Q_ASSERT(dataRoot.keys()==dataNode.keys());

    double firstYearDataRoot=dataRoot.value(firstYear);
    double firstYearDataNode=dataNode.value(firstYear);
    double diffRootNode1=firstYearDataRoot-firstYearDataNode;

    //Will contains computed data
    QMap<double,double> dataPlotRoot,dataPlotNode;
    QVector<double> dataPlotRootX,dataPlotNodeX,dataPlotRootY,dataPlotNodeY;

    double minYRange=DBL_MAX-1,maxYRange=DBL_MIN+1;
    foreach(unsigned int key,dataRoot.keys())
    {
        double percentRoot=0.0,percentNode=0.0;
        double diffRootNode2=0.0,diffRootNode=0.0,diffNode=0.0;

        //Skip the first year
        if(key>firstYear)
        {
            diffRootNode2=dataRoot.value(key)-dataNode.value(key);

            diffRootNode=diffRootNode2-diffRootNode1;
            diffNode=dataNode.value(key)-firstYearDataNode;

            if(diffRootNode1!=0.0)
            {
                percentRoot=diffRootNode*100/diffRootNode1;
                if(percentRoot<minYRange)
                    minYRange=percentRoot;
                if(percentRoot>maxYRange)
                    maxYRange=percentRoot;
            }
            dataPlotRoot.insert(key,percentRoot);

            if(firstYearDataNode!=0.0)
            {
                percentNode=diffNode*100/firstYearDataNode;
                if(percentNode<minYRange)
                    minYRange=percentNode;
                if(percentNode>maxYRange)
                    maxYRange=percentNode;
            }
            dataPlotNode.insert(key,percentNode);

            //cumule==false => G1, G3, G5, G7, otherwise G2, G4, G6, G8
            if(cumule==false)
            {
                diffRootNode1=diffRootNode2;
                firstYearDataNode=dataNode.value(key);
            }
        }
    }

    dataPlotNodeX=dataPlotNode.keys().toVector();
    dataPlotNodeY=dataPlotNode.values().toVector();
    dataPlotRootX=dataPlotRoot.keys().toVector();
    dataPlotRootY=dataPlotRoot.values().toVector();

    plot->clearItems();
    plot->clearGraphs();

    plot->addGraph();
    plot->graph(0)->setData(dataPlotNodeX,dataPlotNodeY);

    plot->addGraph();
    plot->graph(1)->setData(dataPlotRootX,dataPlotRootY);

    //Legend
    plot->graph(0)->setName(QString(attachedTree->getNodeName(node)));
    plot->graph(1)->setName(tr("Total - %1").arg(attachedTree->getNodeName(node)));

    plot->legend->setVisible(true);
    plot->legend->setFont(QFont(QFont().family(),8));
    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom);

    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    //Add value labels to points
    Q_ASSERT(dataPlotRoot.keys()==dataPlotNode.keys());
    int i=0;
    int j=dataPlotRoot.count()-1;

    foreach(double key,dataPlotRoot.keys())
    {
        double val1=dataPlotRoot.value(key);
        double val2=dataPlotNode.value(key);
        QCPItemText *text=new QCPItemText(plot);
        text->setText(formatDouble(val1,1)+"\%");
        int alignment=Qt::AlignCenter;
        if(i==0)
            alignment=Qt::AlignRight;
        else if(i==j)
            alignment=Qt::AlignLeft;

        if(val1<val2)
            alignment|=Qt::AlignTop;
        else
            alignment|=Qt::AlignBottom;

        text->setPadding(QMargins(3,3,3,3));
        text->setPositionAlignment((Qt::AlignmentFlag)alignment);
        text->position->setCoords(key,val1);
        plot->addItem(text);

        text=new QCPItemText(plot);
        text->setText(formatDouble(val2,1)+"\%");

        alignment=Qt::AlignCenter;
        if(i==0)
            alignment=Qt::AlignRight;
        else if(i==j)
            alignment=Qt::AlignLeft;

        if(val2<val1)
            alignment|=Qt::AlignTop;
        else
            alignment|=Qt::AlignBottom;

        text->setPadding(QMargins(3,3,3,3));
        text->setPositionAlignment((Qt::AlignmentFlag)alignment);
        text->position->setCoords(key,val2);
        plot->addItem(text);
        i++;
    }


    QString plotTitle;
    if(cumule==false)
    {
        if(modeORED!=engages)
            plotTitle=tr("&Eacute;volution du %1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(OREDtoCompleteString(modeORED)).arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));
        else
            plotTitle=tr("&Eacute;volution de l'%1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(OREDtoCompleteString(modeORED)).arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));
    }

    else
    {
        if(modeORED!=engages)
            plotTitle=tr("&Eacute;volution du cumulé du %1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(OREDtoCompleteString(modeORED)).arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));
        else
            plotTitle=tr("&Eacute;volution du cumulé de l'%1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(OREDtoCompleteString(modeORED)).arg(attachedTree->getNodeName(node).toHtmlEscaped()).arg(modeToCompleteString(mode));
    }

    /*
     * Embedded plot title, not used here
     *
     * QCPPlotTitle *title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPPlotTitle(plot,plotTitle);
        title->setAutoMargins(QCP::msAll);
        title->setMargins(QMargins(0,0,0,0));
        //title->setFont(QFont(QFont().family(),12));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPPlotTitle *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }
    */

    plot->graph(0)->setPen(QPen(QColor(255,0,0)));
    plot->graph(0)->setBrush(QBrush(QColor(255,0,0,70)));
    plot->graph(1)->setPen(QPen(QColor(0,0,255)));
    plot->graph(1)->setBrush(QBrush(QColor(0,0,255,70)));

    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setAutoSubTicks(false);
    plot->xAxis->setTickStep(1);
    plot->xAxis->setSubTickCount(0);
    plot->xAxis->grid()->setVisible(false);
    plot->yAxis->grid()->setZeroLinePen(plot->yAxis->grid()->pen());

    plot->xAxis->setRange(dataPlotNodeX.first()-0.2,dataPlotNodeX.last()+0.2);
    plot->yAxis->setRange(minYRange-10,maxYRange+10);
    return plotTitle;
}


