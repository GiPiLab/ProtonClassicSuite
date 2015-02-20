#include "pcx_graphics.h"
#include "utils.h"
#include <float.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QObject>


using namespace NUMBERSFORMAT;

PCx_Graphics::PCx_Graphics(PCx_Audit *model,QCustomPlot *plot,int graphicsWidth,int graphicsHeight,double scale):auditModel(model)
{
    Q_ASSERT(model!=nullptr);
    reportingModel=nullptr;
    setGraphicsWidth(graphicsWidth);
    setGraphicsHeight(graphicsHeight);
    setScale(scale);

    if(plot==0)
    {
        this->plot=new QCustomPlot();
        ownPlot=true;
    }
    else
    {
        this->plot=plot;
        ownPlot=false;
    }
}


PCx_Graphics::PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot, int graphicsWidth, int graphicsHeight, double scale):reportingModel(reportingModel)
{
    Q_ASSERT(reportingModel!=nullptr);
    auditModel=nullptr;
    setGraphicsWidth(graphicsWidth);
    setGraphicsHeight(graphicsHeight);
    setScale(scale);

    if(plot==0)
    {
        this->plot=new QCustomPlot();
        ownPlot=true;
    }
    else
    {
        this->plot=plot;
        ownPlot=false;
    }

}

PCx_Graphics::~PCx_Graphics()
{
    if(ownPlot==true)
        delete plot;
}


QString PCx_Graphics::getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule, const PCx_PrevisionItem *prevItem) const
{
    Q_ASSERT(node>0 && plot!=nullptr);
    if(auditModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }
    QString tableName=MODES::modeToTableString(mode);
    QString oredName=PCx_Audit::OREDtoTableString(modeORED);

    QSqlQuery q;

    //Will contain data read from db
    QMap<unsigned int, qint64> dataRoot,dataNode;

    q.prepare(QString("select * from audit_%1_%2 where id_node=:id or id_node=1 order by annee").arg(tableName).arg(auditModel->getAuditId()));
    q.bindValue(":id",node);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    unsigned int firstYear=0;

    while(q.next())
    {
        unsigned int annee=q.value("annee").toUInt();
        if(firstYear==0)
            firstYear=annee;

        qint64 data=q.value(oredName).toLongLong();

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

    if(prevItem!=nullptr)
    {
        unsigned int year=prevItem->getYear();
        dataNode.insert(year,prevItem->getSummedPrevisionItemValue());
        PCx_PrevisionItem tmpPrevRootItem=PCx_PrevisionItem(prevItem->getPrevision(),mode,1,year);
        tmpPrevRootItem.loadFromDb();
        dataRoot.insert(year,tmpPrevRootItem.getSummedPrevisionItemValue());
        //dataRoot.insert(year,)
    }



    //dataRoot and dataNode must have the same keys
    Q_ASSERT(dataRoot.keys()==dataNode.keys());

    qint64 firstYearDataRoot=dataRoot.value(firstYear);
    qint64 firstYearDataNode=dataNode.value(firstYear);
    qint64 diffRootNode1=firstYearDataRoot-firstYearDataNode;

    //Will contains computed data
    QMap<double,double> dataPlotRoot,dataPlotNode;
    QVector<double> dataPlotRootX,dataPlotNodeX,dataPlotRootY,dataPlotNodeY;

    double minYRange=DBL_MAX-1,maxYRange=DBL_MIN+1;
    foreach(unsigned int key,dataRoot.keys())
    {
        qint64 diffRootNode2=0,diffRootNode=0,diffNode=0;

        //Skip the first year
        if(key>firstYear)
        {
            double percentRoot=0.0,percentNode=0.0;
            diffRootNode2=dataRoot.value(key)-dataNode.value(key);

            diffRootNode=diffRootNode2-diffRootNode1;
            diffNode=dataNode.value(key)-firstYearDataNode;

            if(diffRootNode1!=0)
            {
                percentRoot=diffRootNode*100.0/diffRootNode1;

            }
            if(percentRoot<minYRange)
                minYRange=percentRoot;
            if(percentRoot>maxYRange)
                maxYRange=percentRoot;
            dataPlotRoot.insert((double)key,percentRoot);

            if(firstYearDataNode!=0)
            {
                percentNode=diffNode*100.0/firstYearDataNode;
            }
            if(percentNode<minYRange)
                minYRange=percentNode;
            if(percentNode>maxYRange)
                maxYRange=percentNode;
            dataPlotNode.insert((double)key,percentNode);

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
    QString nodeName=auditModel->getAttachedTree()->getNodeName(node);
    plot->graph(0)->setName(nodeName);
    plot->graph(1)->setName(QString("Total - %1").arg(nodeName));

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
        text->setText(formatDouble(val1,-1,true)+"\%");
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
        text->setText(formatDouble(val2,-1,true)+"\%");

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
        if(modeORED!=PCx_Audit::ORED::ENGAGES)
            plotTitle=QObject::tr("&Eacute;volution comparée du %1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(PCx_Audit::OREDtoCompleteString(modeORED)).arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));
        else
            plotTitle=QObject::tr("&Eacute;volution comparée de l'%1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(PCx_Audit::OREDtoCompleteString(modeORED)).arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));
    }

    else
    {
        if(modeORED!=PCx_Audit::ORED::ENGAGES)
            plotTitle=QObject::tr("&Eacute;volution comparée du cumulé du %1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(PCx_Audit::OREDtoCompleteString(modeORED)).arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));
        else
            plotTitle=QObject::tr("&Eacute;volution comparée du cumulé de l'%1 de la collectivité hormis %2 et de [ %2 ]<br>(%3)").arg(PCx_Audit::OREDtoCompleteString(modeORED)).arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));
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

    QColor c=getColorPen1();
    int alpha=getAlpha();
    plot->graph(0)->setPen(QPen(c));
    c.setAlpha(alpha);
    plot->graph(0)->setBrush(QBrush(c));
    c=getColorPen2();
    plot->graph(1)->setPen(QPen(c));
    c.setAlpha(alpha);
    plot->graph(1)->setBrush(QBrush(c));

    plot->xAxis->setAutoTicks(true);
    plot->xAxis->setAutoTickLabels(true);
    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setAutoSubTicks(false);
    plot->xAxis->setTickLength(0,4);
    plot->xAxis->setTickStep(1);
    plot->xAxis->setSubTickCount(0);
    plot->xAxis->setTickLabelRotation(0);
    plot->xAxis->grid()->setVisible(false);
    plot->yAxis->grid()->setZeroLinePen(plot->yAxis->grid()->pen());
    plot->xAxis->setRange(dataPlotNodeX.first()-0.8,dataPlotNodeX.last()+0.8);

    //qDebug()<<"Min YRange="<<minYRange<<" Max YRange="<<maxYRange;
    double padding=(maxYRange-minYRange)*0.2;

    plot->yAxis->setRange(minYRange-(padding*2),maxYRange+padding);
    return plotTitle;
}

QString PCx_Graphics::getPCAG9(unsigned int node) const
{
    Q_ASSERT(node>0 && plot!=nullptr);
    if(auditModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }


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
    QColor c=getColorDFBar();
    int alpha=getAlpha();

    dfBar->setName(MODES::modeToCompleteString(MODES::DFRFDIRI::DF));
    pen.setColor(c);
    dfBar->setPen(pen);
    c.setAlpha(alpha);
    dfBar->setBrush(c);

    c=getColorRFBar();

    rfBar->setName(MODES::modeToCompleteString(MODES::DFRFDIRI::RF));
    pen.setColor(c);
    rfBar->setPen(pen);
    c.setAlpha(alpha);
    rfBar->setBrush(c);

    c=getColorDIBar();

    diBar->setName(MODES::modeToCompleteString(MODES::DFRFDIRI::DI));
    pen.setColor(c);
    diBar->setPen(pen);
    c.setAlpha(alpha);
    diBar->setBrush(c);

    c=getColorRIBar();

    riBar->setName(MODES::modeToCompleteString(MODES::DFRFDIRI::RI));
    pen.setColor(c);
    riBar->setPen(pen);
    c.setAlpha(alpha);
    riBar->setBrush(c);

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
                      "order by a.annee").arg(auditModel->getAuditId()));
    q.bindValue(":id_node",node);
    q.exec();
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    QList<QPair<QString,QString> > listModesAndLabels;
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::DISPONIBLES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)));

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
            qint64 dataDF=0,dataRF=0,dataDI=0,dataRI=0,sum=0;
            double percentDF=0.0,percentRF=0.0,percentDI=0.0,percentRI=0.0;
            dataDF=q.value(mode.first+"DF").toLongLong();
            dataRF=q.value(mode.first+"RF").toLongLong();
            dataDI=q.value(mode.first+"DI").toLongLong();
            dataRI=q.value(mode.first+"RI").toLongLong();

            //In case of negative disponible
            if(dataDF<0)
                dataDF=0;
            if(dataRF<0)
                dataRF=0;
            if(dataDI<0)
                dataDI=0;
            if(dataRI<0)
                dataRI=0;

            sum=dataDF+dataRF+dataDI+dataRI;
            if(sum>0)
            {
                percentDF=100.0*dataDF/sum;
                percentRF=100.0*dataRF/sum;
                percentDI=100.0*dataDI/sum;
                percentRI=100.0*dataRI/sum;
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
    /*qDebug()<<"G9:ticks = "<<ticks;
    qDebug()<<"G9:ValueDF = "<<valuesDF;
    qDebug()<<"G9:ValueRF = "<<valuesRF;
    qDebug()<<"G9:ValueDI = "<<valuesDI;
    qDebug()<<"G9:ValueRI = "<<valuesRI;*/

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

    plotTitle=QString("Proportions des d&eacute;penses et recettes pour [ %1 ]").arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped());
    return plotTitle;

}

QString PCx_Graphics::getPCAHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Audit::ORED> selectedORED,const PCx_PrevisionItem *prevItem,bool miniMode) const
{
    if(auditModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    plot->clearItems();
    plot->clearGraphs();
    plot->clearPlottables();


    QString plotTitle;

    //Colors for each graph
    QColor PENCOLORS[4]=
    {
        Qt::darkCyan,
        Qt::gray,
        Qt::blue,
        Qt::red
    };

    if(miniMode==true)
    {
        if(selectedORED.count()==2)
            plotTitle=QObject::tr("Crédits %1s (bleu) et %2s (gris)").arg(PCx_Audit::OREDtoCompleteString(selectedORED.at(0)))
                    .arg(PCx_Audit::OREDtoCompleteString(selectedORED.at(1)));

        QCPPlotTitle * title;
        if(plot->plotLayout()->elementCount()==1)
        {
            plot->plotLayout()->insertRow(0);
            title=new QCPPlotTitle(plot,plotTitle);
            title->setFont(QFont(QFont().family(),8));
            plot->plotLayout()->addElement(0,0,title);
        }
        else
        {
            title=(QCPPlotTitle *)plot->plotLayout()->elementAt(0);
            title->setText(plotTitle);
        }
    }
    /*else
    {
        plotTitle=QString("Données historiques et prévision pour %1<br>(%2)").arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped())
                .arg(MODES::modeToCompleteString(mode));
    }*/
    if(selectedORED.isEmpty())
    {
        plot->replot();
        return QString();
    }

    QSqlQuery q;

    q.prepare(QString("select * from audit_%1_%2 where id_node=:id order by annee").arg(MODES::modeToTableString(mode)).arg(auditModel->getAuditId()));
    q.bindValue(":id",selectedNodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    QVector<double> dataX,dataY[4];

    while(q.next())
    {
        unsigned int date=q.value("annee").toUInt();
        dataX.append((double)date);

        //WARNING : ORED fixed size here
        for(int i=PCx_Audit::ORED::OUVERTS;i<4;i++)
        {
            if(selectedORED.contains((PCx_Audit::ORED)i))
            {
                qint64 data=q.value(PCx_Audit::OREDtoTableString((PCx_Audit::ORED)i)).toLongLong();
                dataY[i].append(NUMBERSFORMAT::fixedPointToDouble(data));
            }
        }
    }

    if(dataX.isEmpty())
    {
        plot->replot();
        return QString();
    }

    if(prevItem!=nullptr)
    {
        dataX.append((double)prevItem->getYear());
        qint64 summedPrev=prevItem->getSummedPrevisionItemValue();
       // if(summedPrev!=0)
            dataY[0].append(NUMBERSFORMAT::fixedPointToDouble(summedPrev));

    }

    bool first=true;
    //WARNING : ORED fixed size here
    for(int i=PCx_Audit::ORED::OUVERTS;i<4;i++)
    {
        if(selectedORED.contains((PCx_Audit::ORED)i))
        {
            plot->addGraph();
            plot->graph()->setData(dataX,dataY[i]);
            plot->graph()->setName(PCx_Audit::OREDtoCompleteString((PCx_Audit::ORED)i));
            plot->graph()->setPen(QPen(PENCOLORS[i]));
            plot->graph()->setScatterStyle(QCPScatterStyle::ssDisc);
            plot->graph()->rescaleAxes(!first);
            first=false;
        }
    }

    QCPRange range;

    range=plot->yAxis->range();
    range.lower-=(range.lower*20.0/100.0);
    range.upper+=(range.upper*10.0/100.0);
    plot->yAxis->setRange(range);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    if(miniMode==false)
    {
        plot->legend->setVisible(true);
        plot->legend->setFont(QFont(QFont().family(),7));
        //plot->legend->setRowSpacing(-8);
        plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignRight);
    }


    plot->xAxis->setAutoTicks(true);
    plot->xAxis->setAutoTickLabels(true);
    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setAutoSubTicks(false);
    plot->xAxis->setTickLength(0,4);
    plot->xAxis->setTickStep(1);
    plot->xAxis->setSubTickCount(0);
    if(miniMode==true)
    {
        plot->xAxis->setTickLabelRotation(45);
        plot->yAxis->setTickLabelFont(QFont(QFont().family(),7));
        plot->xAxis->setTickLabelFont(QFont(QFont().family(),7));
    }

    //plot->yAxis->setVisible(false);
    plot->yAxis->setAutoTicks(true);
    plot->yAxis->setAutoTickCount(4);
    plot->yAxis->setAutoTickLabels(true);
    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setSubTickCount(0);
    plot->yAxis->setNumberFormat("gbc");


    plot->xAxis->setRange(dataX.first()-0.8,dataX.last()+0.8);
    plot->xAxis->grid()->setVisible(false);

  //  plot->yAxis->grid()->setZeroLinePen(plot->yAxis->grid()->pen());

    // plot->xAxis->setAutoTickStep(false);
   // plot->xAxis->setTickStep(3600*24*15);


    plot->replot();
    return plotTitle;
}



QString PCx_Graphics::getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR) const
{
    if(reportingModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    plot->clearItems();
    plot->clearGraphs();
    plot->clearPlottables();

    QString plotTitle=QObject::tr("%1\n(%2)").arg(reportingModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));

    QCPPlotTitle * title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPPlotTitle(plot,plotTitle);
        title->setFont(QFont(QFont().family(),11));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPPlotTitle *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }

    if(selectedOREDPCR.isEmpty())
    {
        plot->replot();
        return QString();
    }

    //Colors for each graph
    Qt::GlobalColor PENCOLORS[PCx_Reporting::OREDPCR::NONELAST]=
    {
        Qt::blue,
        Qt::red,
        Qt::yellow,
        Qt::green,
        Qt::magenta,
        Qt::cyan,
        Qt::darkRed,
        Qt::darkBlue,
        Qt::darkGray,
        Qt::darkGreen,
        Qt::darkYellow
    };

    QSqlQuery q;


    q.prepare(QString("select * from reporting_%1_%2 where id_node=:id order by date").arg(MODES::modeToTableString(mode)).arg(reportingModel->getReportingId()));
    q.bindValue(":id",selectedNodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    QVector<double> dataX,dataY[PCx_Reporting::OREDPCR::NONELAST];

    while(q.next())
    {
        unsigned int date=q.value("date").toUInt();
        dataX.append((double)date);

        for(int i=PCx_Reporting::OREDPCR::OUVERTS;i<PCx_Reporting::OREDPCR::NONELAST;i++)
        {
            if(selectedOREDPCR.contains((PCx_Reporting::OREDPCR)i))
            {
                qint64 data=q.value(PCx_Reporting::OREDPCRtoTableString((PCx_Reporting::OREDPCR)i)).toLongLong();
                dataY[i].append(NUMBERSFORMAT::fixedPointToDouble(data));
            }
        }
    }

    if(dataX.isEmpty())
    {
        plot->replot();
        return QString();
    }

    bool first=true;
    for(int i=PCx_Reporting::OREDPCR::OUVERTS;i<PCx_Reporting::OREDPCR::NONELAST;i++)
    {
        if(selectedOREDPCR.contains((PCx_Reporting::OREDPCR)i))
        {
            plot->addGraph();
            plot->graph()->setData(dataX,dataY[i]);
            plot->graph()->setName(PCx_Reporting::OREDPCRtoCompleteString((PCx_Reporting::OREDPCR)i));
            plot->graph()->setPen(QPen(PENCOLORS[i]));
            plot->graph()->setScatterStyle(QCPScatterStyle::ssDisc);
            plot->graph()->rescaleAxes(!first);
            first=false;
        }
    }

    QCPRange range;
    unsigned int year=QDateTime::fromTime_t((int)dataX.first()).date().year();
    QDateTime dtBegin(QDate(year,1,1));
    QDateTime dtEnd(QDate(year,12,31));
    plot->xAxis->setRangeLower((double)dtBegin.toTime_t());
    plot->xAxis->setRangeUpper((double)dtEnd.toTime_t());

    range=plot->yAxis->range();
    range.lower-=(range.lower*20.0/100.0);
    range.upper+=(range.upper*10.0/100.0);
    plot->yAxis->setRange(range);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->legend->setVisible(true);
    plot->legend->setFont(QFont(QFont().family(),8));
    plot->legend->setRowSpacing(-5);
    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);

    plot->xAxis->setAutoTicks(true);

    plot->xAxis->setAutoTickLabels(true);
    plot->xAxis->setTickLabelRotation(0);

    plot->yAxis->setAutoTicks(true);
    plot->yAxis->setAutoTickLabels(true);
    plot->yAxis->setAutoTickLabels(true);

    plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
   // plot->xAxis->setAutoTickStep(false);
   // plot->xAxis->setTickStep(3600*24*15);

    plot->xAxis->setDateTimeFormat("MMM\nyyyy");


    plot->replot();
    return plotTitle;
}

QString PCx_Graphics::getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);

    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::BP,
                                                PCx_Reporting::REPORTS,
                                                PCx_Reporting::OCDM,
                                                PCx_Reporting::VCDM,
                                                PCx_Reporting::VIREMENTSINTERNES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Provenance des crédits de\n%1\n(%2)").arg(nodeName).arg(modeName),getColorDFBar());
}

QString PCx_Graphics::getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);

    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::OCDM,
                                                PCx_Reporting::VCDM,
                                                PCx_Reporting::VIREMENTSINTERNES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::BP,QString("Facteurs de variation des crédits de\n%1\n(%2)").arg(nodeName).arg(modeName),getColorRFBar());
}

QString PCx_Graphics::getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);
    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::REALISES,
                                                PCx_Reporting::ENGAGES,
                                                PCx_Reporting::DISPONIBLES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Utilisation des crédits de\n%1\n(%2)").arg(nodeName).arg(modeName),getColorDIBar());
}

QString PCx_Graphics::getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode) const
{
    QList<PCx_Reporting::OREDPCR> oredPCR={PCx_Reporting::OREDPCR::OUVERTS,PCx_Reporting::OREDPCR::REALISES};
    return getPCRHistory(nodeId,mode,oredPCR);
}

QString PCx_Graphics::getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference, const QString &plotTitle,QColor color) const
{
    if(reportingModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    plot->clearItems();
    plot->clearGraphs();
    plot->clearPlottables();

    QCPPlotTitle * title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPPlotTitle(plot,plotTitle);
        title->setFont(QFont(QFont().family(),11));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPPlotTitle *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }

    QCPBars *bars=new QCPBars(plot->xAxis,plot->yAxis);
    plot->addPlottable(bars);

    QPen pen;
    pen.setWidth(0);

    int alpha=getAlpha();

    pen.setColor(color);
    bars->setPen(pen);
    color.setAlpha(alpha);
    bars->setBrush(color);


    QSqlQuery q;
    q.prepare(QString("select * from reporting_%1_%2 where id_node=:id_node order by date desc limit 1")
              .arg(MODES::modeToTableString(mode))
              .arg(reportingModel->getReportingId()));
    q.bindValue(":id_node",selectedNodeId);
    q.exec();
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }

    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> values;

    int i=1;
    foreach(PCx_Reporting::OREDPCR ored,selectedOREDPCR)
    {
        labels.append(PCx_Reporting::OREDPCRtoCompleteString(ored));
        ticks.append(i++);

    }

    while(q.next())
    {
            qint64 refVal=q.value(PCx_Reporting::OREDPCRtoTableString(oredReference)).toLongLong();
            if(refVal==0)
                return plotTitle;

            foreach(PCx_Reporting::OREDPCR ored,selectedOREDPCR)
            {
                qint64 val=q.value(PCx_Reporting::OREDPCRtoTableString(ored)).toLongLong();
                values.append((double)val/(double)refVal*100.0);
            }
    }

    bars->setData(ticks,values);

    plot->legend->setVisible(false);

    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->yAxis->setLabel("%");
    plot->xAxis->setTickLength(0,0);
    plot->xAxis->grid()->setVisible(false);
    //plot->xAxis->setTickLabelRotation(90);
    plot->xAxis->setTickVector(ticks);
    plot->xAxis->setTickVectorLabels(labels);
    plot->xAxis->setSubTickCount(0);
    //plot->graph()->rescaleAxes();
    plot->xAxis->setRange(0,ticks.count()+1);
    plot->yAxis->rescale();
    //plot->yAxis->setRange(0,150);

    return plotTitle;
}

void PCx_Graphics::setGraphicsWidth(int width)
{
    graphicsWidth=width;
    if(graphicsWidth<MINWIDTH)
        graphicsWidth=MINWIDTH;
    if(graphicsWidth>MAXWIDTH)
        graphicsWidth=MAXWIDTH;
}

void PCx_Graphics::setGraphicsHeight(int height)
{
    graphicsHeight=height;
    if(graphicsHeight<MINHEIGHT)
        graphicsHeight=MINHEIGHT;
    if(graphicsHeight>MAXHEIGHT)
        graphicsHeight=MAXHEIGHT;
}

void PCx_Graphics::setScale(double scale)
{
    this->scale=scale;
    if(this->scale<MINSCALE)
        this->scale=MINSCALE;
    if(this->scale>MAXSCALE)
        this->scale=MAXSCALE;
}

QColor PCx_Graphics::getColorPen1()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/pen1",PCx_Graphics::DEFAULTPENCOLOR1).toUInt();
    qDebug()<<oldcolor;
    return QColor(oldcolor);
}

QColor PCx_Graphics::getColorPen2()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/pen2",PCx_Graphics::DEFAULTPENCOLOR2).toUInt();
    return QColor(oldcolor);
}

QColor PCx_Graphics::getColorDFBar()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/penDFBar",PCx_Graphics::DEFAULTCOLORDFBAR).toUInt();
    return QColor(oldcolor);
}

QColor PCx_Graphics::getColorRFBar()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/penRFBar",PCx_Graphics::DEFAULTCOLORRFBAR).toUInt();
    return QColor(oldcolor);
}

QColor PCx_Graphics::getColorDIBar()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/penDIBar",PCx_Graphics::DEFAULTCOLORDIBAR).toUInt();
    return QColor(oldcolor);
}

QColor PCx_Graphics::getColorRIBar()
{
    QSettings settings;
    unsigned int oldcolor=settings.value("graphics/penRIBar",PCx_Graphics::DEFAULTCOLORRIBAR).toUInt();
    return QColor(oldcolor);
}

int PCx_Graphics::getAlpha()
{
    QSettings settings;
    int alpha=settings.value("graphics/alpha",PCx_Graphics::DEFAULTALPHA).toInt();
    return alpha;
}

QString PCx_Graphics::getCSS()
{
    return "\ndiv.g{margin-left:auto;margin-right:auto;page-break-inside:avoid;}";
}



