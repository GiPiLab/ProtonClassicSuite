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

            }
            if(percentRoot<minYRange)
                minYRange=percentRoot;
            if(percentRoot>maxYRange)
                maxYRange=percentRoot;
            dataPlotRoot.insert(key,percentRoot);

            if(firstYearDataNode!=0.0)
            {
                percentNode=diffNode*100/firstYearDataNode;
            }
            if(percentNode<minYRange)
                minYRange=percentNode;
            if(percentNode>maxYRange)
                maxYRange=percentNode;
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
    plot->clearPlottables();

    plot->addGraph();
    plot->graph(0)->setData(dataPlotNodeX,dataPlotNodeY);

    plot->addGraph();
    plot->graph(1)->setData(dataPlotRootX,dataPlotRootY);

    //Legend
    plot->graph(0)->setName(QString(attachedTree->getNodeName(node)));
    plot->graph(1)->setName(tr("Total - %1").arg(attachedTree->getNodeName(node)));

    plot->legend->setVisible(true);
    plot->legend->setFont(QFont(QFont().family(),8));
    plot->legend->setRowSpacing(-5);
    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom);

    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    //Add value labels to points
    Q_ASSERT(dataPlotRoot.keys()==dataPlotNode.keys());
    int i=0;

    foreach(double key,dataPlotRoot.keys())
    {
        double val1=dataPlotRoot.value(key);
        double val2=dataPlotNode.value(key);
        QCPItemText *text=new QCPItemText(plot);
        text->setText(formatDouble(val1,1)+"\%");
        int alignment=Qt::AlignHCenter;
       /* if(i==0)
            alignment=Qt::AlignRight;
        else if(i==j)
            alignment=Qt::AlignLeft;*/

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

        alignment=Qt::AlignHCenter;
        /*if(i==0)
            alignment=Qt::AlignRight;
        else if(i==j)
            alignment=Qt::AlignLeft;*/

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
    plot->xAxis->setTickLength(0,4);
    plot->xAxis->setTickStep(1);
    plot->xAxis->setSubTickCount(0);
    plot->xAxis->grid()->setVisible(false);
    plot->yAxis->grid()->setZeroLinePen(plot->yAxis->grid()->pen());

    plot->xAxis->setRange(dataPlotNodeX.first()-0.8,dataPlotNodeX.last()+0.8);

    // qDebug()<<minYRange<<maxYRange;
    double padding=(maxYRange-minYRange)*0.2;

    plot->yAxis->setRange(minYRange-(padding*2),maxYRange+padding);
    return plotTitle;
}

QString PCx_AuditModel::getG9(unsigned int node,QCustomPlot *plot) const
{
    Q_ASSERT(node>0 && plot!=NULL);
    QString plotTitle;

    plot->clearItems();
    plot->clearGraphs();
    plot->clearPlottables();

    QCPBars *dfBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *rfBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *diBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *riBar=new QCPBars(plot->xAxis,plot->yAxis);
    plot->addPlottable(dfBar);
    plot->addPlottable(rfBar);
    plot->addPlottable(diBar);
    plot->addPlottable(riBar);

    QPen pen;
    pen.setWidth(0);

    dfBar->setName(modeToCompleteString(DF));
    pen.setColor(QColor(255,0,0,70));
    dfBar->setPen(pen);
    dfBar->setBrush(QColor(255,0,0,70));

    rfBar->setName(modeToCompleteString(RF));
    pen.setColor(QColor(0,255,0,70));
    rfBar->setPen(pen);
    rfBar->setBrush(QColor(0,255,0,70));

    diBar->setName(modeToCompleteString(DI));
    pen.setColor(QColor(0,0,255,70));
    diBar->setPen(pen);
    diBar->setBrush(QColor(0,0,255,70));

    riBar->setName(modeToCompleteString(RI));
    pen.setColor(QColor(0,255,255,70));
    riBar->setPen(pen);
    riBar->setBrush(QColor(0,255,255,70));

    rfBar->moveAbove(dfBar);
    diBar->moveAbove(rfBar);
    riBar->moveAbove(diBar);

    QSqlQuery q;
    q.prepare(QString("select a.annee, a.ouverts 'ouvertsDF', b.ouverts 'ouvertsRF', c.ouverts 'ouvertsDI', d.ouverts 'ouvertsRI',"
                      "a.realises 'realisesDF', b.realises 'realisesRF', c.realises 'realisesDI', d.realises 'realisesRI',"
                      "a.engages 'engagesDF', b.engages 'engagesRF', c.engages 'engagesDI', d.engages 'engagesRI',"
                      "a.disponibles 'disponiblesDF', b.disponibles 'disponiblesRF', c.disponibles 'disponiblesDI', d.disponibles 'disponiblesRI' "
                      "from audit_DF_%1 a, audit_RF_%1 b, audit_DI_%1 c,audit_RI_%1 d "
                      "where a.id_node=:id_node and a.id_node=b.id_node and b.id_node=c.id_node "
                      "and c.id_node=d.id_node and a.annee=b.annee and b.annee=c.annee and c.annee=d.annee "
                      "order by a.annee").arg(auditId));
    q.bindValue(":id_node",node);
    q.exec();
    if(!q.isActive())
    {
        qCritical()<<q.lastError().text();
        die();
    }

    QList<QPair<QString,QString> > listModesAndLabels;
    listModesAndLabels.append(QPair<QString,QString>(OREDtoTableString(ouverts),OREDtoCompleteString(ouverts)));
    listModesAndLabels.append(QPair<QString,QString>(OREDtoTableString(realises),OREDtoCompleteString(realises)));
    listModesAndLabels.append(QPair<QString,QString>(OREDtoTableString(engages),OREDtoCompleteString(engages)));
    listModesAndLabels.append(QPair<QString,QString>(OREDtoTableString(disponibles),OREDtoCompleteString(disponibles)));

    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> valuesDF,valuesRF,valuesDI,valuesRI;
    int tickCounter=0;

    QPair<QString,QString>mode;
    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();

        foreach (mode,listModesAndLabels)
        {
            double dataDF=0.0,dataRF=0.0,dataDI=0.0,dataRI=0.0,sum=0.0;
            double percentDF=0.0,percentRF=0.0,percentDI=0.0,percentRI=0.0;
            dataDF=q.value(mode.first+"DF").toDouble();
            dataRF=q.value(mode.first+"RF").toDouble();
            dataDI=q.value(mode.first+"DI").toDouble();
            dataRI=q.value(mode.first+"RI").toDouble();

            //In case of negative disponible
            if(dataDF<0.0)
                dataDF=0.0;
            if(dataRF<0.0)
                dataRF=0.0;
            if(dataDI<0.0)
                dataDI=0.0;
            if(dataRI<0.0)
                dataRI=0.0;

            sum=dataDF+dataRF+dataDI+dataRI;
            if(sum>0.0)
            {
                percentDF=100*dataDF/sum;
                percentRF=100*dataRF/sum;
                percentDI=100*dataDI/sum;
                percentRI=100*dataRI/sum;
            }
            valuesDF.append(percentDF);
            valuesRF.append(percentRF);
            valuesDI.append(percentDI);
            valuesRI.append(percentRI);
            ticks.append(++tickCounter);
            labels.append(QString("%1 %2").arg(mode.second).arg(annee));
        }
        ticks.append(++tickCounter);
        labels.append(QString());
        valuesDF.append(0.0);
        valuesRF.append(0.0);
        valuesDI.append(0.0);
        valuesRI.append(0.0);
    }
    //qDebug()<<ticks;
    //qDebug()<<"ValueDF = "<<valuesDF;
    //qDebug()<<"ValueRF = "<<valuesRF;
    //qDebug()<<"ValueDI = "<<valuesDI;
    //qDebug()<<"ValueRI = "<<valuesRI;

    dfBar->setData(ticks,valuesDF);
    rfBar->setData(ticks,valuesRF);
    diBar->setData(ticks,valuesDI);
    riBar->setData(ticks,valuesRI);

    QFont legendFont=QFont();
    legendFont.setPointSize(8);
    plot->legend->setFont(legendFont);
    plot->legend->setRowSpacing(-8);
    //plot->legend->setIconSize(50,5);
    plot->legend->setVisible(true);

    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignHCenter);

    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->yAxis->setLabel("%");
    plot->xAxis->setTickLength(0,0);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setTickLabelRotation(90);
    plot->xAxis->setTickVector(ticks);
    plot->xAxis->setTickVectorLabels(labels);
    plot->xAxis->setSubTickCount(0);
    plot->xAxis->setRange(0,tickCounter);
    plot->yAxis->setRange(0,150);

    plotTitle=QString("Proportions des d&eacute;penses et recettes pour [ %1 ]").arg(attachedTree->getNodeName(node).toHtmlEscaped());
    return plotTitle;

}


