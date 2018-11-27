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




#include "pcx_graphics.h"
#include "utils.h"
#include <float.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QObject>


using namespace NUMBERSFORMAT;

PCx_Graphics::PCx_Graphics(PCx_Audit *model,QCustomPlot *plot,int graphicsWidth,int graphicsHeight,double scale):auditModel(model)
{
    if(model==nullptr)
    {
        qFatal("Assertion failed");
    }
    reportingModel=nullptr;
    setGraphicsWidth(graphicsWidth);
    setGraphicsHeight(graphicsHeight);
    setScale(scale);

    if(plot==nullptr)
    {        
        this->plot=new QCustomPlot();
        ownPlot=true;
    }
    else
    {
        this->plot=plot;
        ownPlot=false;
    }
    qDebug()<<"Ownplot = "<<ownPlot;

}


PCx_Graphics::PCx_Graphics(PCx_Reporting *reportingModel, QCustomPlot *plot, int graphicsWidth, int graphicsHeight, double scale):reportingModel(reportingModel)
{
    if(reportingModel==nullptr)
    {
        qFatal("Assertion failed");
    }
    auditModel=nullptr;
    setGraphicsWidth(graphicsWidth);
    setGraphicsHeight(graphicsHeight);
    setScale(scale);

    if(plot==nullptr)
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


QString PCx_Graphics::getPCAG1G8(unsigned int node, MODES::DFRFDIRI mode, PCx_Audit::ORED modeORED, bool cumule, const PCx_PrevisionItem *prevItem, unsigned int referenceNode)
{
    if(node==0 || plot==nullptr || auditModel==nullptr|| referenceNode==0)
    {
        qFatal("Assertion failed");
    }

    QString tableName=MODES::modeToTableString(mode);
    QString oredName=PCx_Audit::OREDtoTableString(modeORED);

    QSqlQuery q;

    //Will contain data read from db
    QMap<unsigned int, qint64> dataRoot,dataNode;


    q.prepare(QString("select * from audit_%1_%2 where id_node=:id or id_node=:refNode order by annee").arg(tableName).arg(auditModel->getAuditId()));
    q.bindValue(":id",node);
    q.bindValue(":refNode",referenceNode);
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

        if(q.value("id_node").toUInt()==referenceNode)
        {
            dataRoot.insert(annee,data);
            if(node==referenceNode)
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
        PCx_PrevisionItem tmpPrevRootItem=PCx_PrevisionItem(prevItem->getPrevision(),mode,referenceNode,year);
        tmpPrevRootItem.loadFromDb();
        dataRoot.insert(year,tmpPrevRootItem.getSummedPrevisionItemValue());
        //dataRoot.insert(year,)
    }



    //dataRoot and dataNode must have the same keys
    //NOTE : Remove this costly assertion
    /*if(dataRoot.keys()!=dataNode.keys())
    {
        qFatal("Assertion failed keys are not equal");
    }*/

    qint64 firstYearDataRoot=dataRoot.value(firstYear);
    qint64 firstYearDataNode=dataNode.value(firstYear);
    qint64 diffRootNode1=firstYearDataRoot-firstYearDataNode;

    //Will contains computed data
    QMap<double,double> dataPlotRoot,dataPlotNode;
    QVector<double> dataPlotRootX,dataPlotNodeX,dataPlotRootY,dataPlotNodeY;

    double minYRange=DBL_MAX-1,maxYRange=DBL_MIN+1;


    for(QMap<unsigned int, qint64>::const_iterator it=dataRoot.constBegin(), end = dataRoot.constEnd();it!=end;++it)
    {
    //foreach(unsigned int key,dataRoot.keys())
    //{
        qint64 diffRootNode2=0,diffRootNode=0,diffNode=0;

        unsigned int key=it.key();
        //Skip the first year
        if(key>firstYear)
        {
            double percentRoot=0.0,percentNode=0.0;
            diffRootNode2=it.value()-dataNode.value(key);

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

    for(QMap<double,double>::const_iterator it=dataPlotNode.constBegin(),end=dataPlotNode.constEnd();it!=end;++it)
    {
        dataPlotNodeX.append(it.key());
        dataPlotNodeY.append(it.value());
    }

    for(QMap<double,double>::const_iterator it=dataPlotRoot.constBegin(),end=dataPlotRoot.constEnd();it!=end;++it)
    {
        dataPlotRootX.append(it.key());
        dataPlotRootY.append(it.value());
    }

    if(ownPlot==true)
    {
        if(plot!=nullptr)
        {
            delete plot;
        }
        plot=new QCustomPlot();
    }
    else
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->clearPlottables();
    }

    plot->addGraph();
    plot->graph(0)->setData(dataPlotNodeX,dataPlotNodeY,true);

    plot->addGraph();
    plot->graph(1)->setData(dataPlotRootX,dataPlotRootY,true);

    //Legend
    QString nodeName=auditModel->getAttachedTree()->getNodeName(node);
    QString refNodeName=auditModel->getAttachedTree()->getNodeName(referenceNode);
    plot->graph(0)->setName(nodeName);
    plot->graph(1)->setName(QString("%2 - %1").arg(nodeName,refNodeName));

    plot->yAxis->setLabel("%");

    plot->legend->setVisible(true);
    plot->legend->setFont(QFont("DejaVu Sans"));
    plot->legend->setRowSpacing(3);
    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom);

    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    int i=0;

    for(QMap<double,double>::const_iterator it=dataPlotRoot.constBegin(), end = dataPlotRoot.constEnd();it!=end;++it)
    {
        double key=it.key();
        double val1=it.value();
        double val2=dataPlotNode.value(key);
        QCPItemText *text=new QCPItemText(plot);
        text->setText(formatDouble(val1,-1,true)+"\%");
        text->setFont(QFont("DejaVu Sans"));
        int alignment=Qt::AlignHCenter;

        if(val1<val2)
        {
            alignment|=Qt::AlignTop;
            text->setPadding(QMargins(0,8,0,0));
        }
        else
        {
            alignment|=Qt::AlignBottom;
            text->setPadding(QMargins(0,0,0,4));
        }

        text->setPositionAlignment((Qt::AlignmentFlag)alignment);
        text->position->setCoords(key,val1);


        text=new QCPItemText(plot);
        text->setText(formatDouble(val2,-1,true)+"\%");
        text->setFont(QFont("DejaVu Sans"));

        alignment=Qt::AlignHCenter;

        if(val2<val1)
        {
            alignment|=Qt::AlignTop;
            text->setPadding(QMargins(0,8,0,0));
        }
        else
        {
            alignment|=Qt::AlignBottom;
            text->setPadding(QMargins(0,0,0,4));
        }

        text->setPositionAlignment((Qt::AlignmentFlag)alignment);
        text->position->setCoords(key,val2);
        i++;
    }

    QString plotTitle;
    if(cumule==false)
    {
            plotTitle=QObject::tr("&Eacute;volution comparée des %1 de [ %4 ] hormis %2 et de [ %2 ]<br>(%3)")
                    .arg(PCx_Audit::OREDtoCompleteString(modeORED,true),nodeName.toHtmlEscaped(),
                    MODES::modeToCompleteString(mode),refNodeName.toHtmlEscaped());
    }

    else
    {
            plotTitle=QObject::tr("&Eacute;volution comparée du cumulé des %1 de [ %4 ] hormis %2 et de [ %2 ]<br>(%3)")
                    .arg(PCx_Audit::OREDtoCompleteString(modeORED,true),nodeName.toHtmlEscaped(),
                    MODES::modeToCompleteString(mode),refNodeName.toHtmlEscaped());

    }

    QColor c=getColorPen1();
    int alpha=getAlpha();
    plot->graph(0)->setPen(QPen(c));
    c.setAlpha(alpha);
    plot->graph(0)->setBrush(QBrush(c));
    c=getColorPen2();
    plot->graph(1)->setPen(QPen(c));
    c.setAlpha(alpha);
    plot->graph(1)->setBrush(QBrush(c));

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    plot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(1.0);

    plot->xAxis->setSubTickLength(0);
    plot->xAxis->setTickLength(4);

    plot->xAxis->setTickLabelFont(QFont("DejaVu Sans"));
    plot->yAxis->setTickLabelFont(QFont("DejaVu Sans"));

    plot->xAxis->setTickLabelRotation(0);
    plot->xAxis->grid()->setVisible(false);
    plot->yAxis->grid()->setZeroLinePen(plot->yAxis->grid()->pen());
    plot->xAxis->setRange(dataPlotNodeX.first()-0.8,dataPlotNodeX.last()+0.8);

    double padding=(maxYRange-minYRange)*0.2;

    plot->yAxis->setRange(minYRange-(padding*2),maxYRange+padding);
    return plotTitle;
}

QString PCx_Graphics::getPCAG9(unsigned int node)
{
    if(node==0 || plot==nullptr|| auditModel==nullptr)
    {
        qFatal("Assertion failed");
    }

    QString plotTitle;

    if(ownPlot==true)
    {
        if(plot!=nullptr)
        {
            delete plot;
        }
        plot=new QCustomPlot();
    }
    else
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->clearPlottables();
    }


    QCPBars *dfBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *rfBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *diBar=new QCPBars(plot->xAxis,plot->yAxis);
    QCPBars *riBar=new QCPBars(plot->xAxis,plot->yAxis);

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

    QVector<QPair<QString,QString> > listModesAndLabels;
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::OUVERTS),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::REALISES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::ENGAGES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES)));
    listModesAndLabels.append(QPair<QString,QString>(PCx_Audit::OREDtoTableString(PCx_Audit::ORED::DISPONIBLES),PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES)));

    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> valuesDF,valuesRF,valuesDI,valuesRI;
    int tickCounter=0;
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

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
            ++tickCounter;
            ticks.append(tickCounter);           
            textTicker->addTick(tickCounter,QString("%1 %2").arg(mode.second).arg(annee));
        }
        ++tickCounter;
        ticks.append(tickCounter);
        textTicker->addTick(tickCounter,QString());
        labels.append(QString());
        valuesDF.append(0.0);
        valuesRF.append(0.0);
        valuesDI.append(0.0);
        valuesRI.append(0.0);
    }

    dfBar->setData(ticks,valuesDF);
    rfBar->setData(ticks,valuesRF);
    diBar->setData(ticks,valuesDI);
    riBar->setData(ticks,valuesRI);

    plot->legend->setFont(QFont("DejaVu Sans",8));

    plot->legend->setRowSpacing(-4);
    plot->legend->setWrap(2);
    plot->legend->setVisible(true);

    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignHCenter);

    plot->yAxis->setLabel("%");
    plot->xAxis->setTickLength(0,0);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setTickLabelRotation(90);
    plot->xAxis->setTickLabelFont(QFont("DejaVu Sans"));
    plot->yAxis->setTickLabelFont(QFont("DejaVu Sans"));

    plot->xAxis->setTicker(textTicker);
    plot->xAxis->setRange(0,tickCounter);
    plot->yAxis->setRange(0,180);

    plotTitle=QString("Proportions des d&eacute;penses et recettes pour [ %1 ]").arg(auditModel->getAttachedTree()->getNodeName(node).toHtmlEscaped());
    return plotTitle;

}

QString PCx_Graphics::getPCAHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Audit::ORED> selectedORED,const PCx_PrevisionItem *prevItem,bool miniMode)
{
    if(auditModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    if(ownPlot==true)
    {
        if(plot!=nullptr)
        {
            delete plot;
        }
        plot=new QCustomPlot();
    }
    else
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->clearPlottables();
    }


    QString plotTitle;

    //Colors for each graph
    QColor PENCOLORS[4]=
    {
        Qt::darkCyan,
        Qt::red,
        Qt::gray,
        Qt::yellow
    };


    if(miniMode==true)
    {
        if(selectedORED.count()==2)
            plotTitle=QObject::tr("%1 (bleu) et %2 (rouge)").arg(PCx_Audit::OREDtoCompleteString(selectedORED.at(0),true),PCx_Audit::OREDtoCompleteString(selectedORED.at(1),true));

        QCPTextElement * title;
        if(plot->plotLayout()->elementCount()==1)
        {
            plot->plotLayout()->insertRow(0);
            title=new QCPTextElement(plot,plotTitle);
            title->setFont(QFont("DejaVu Sans",8));
            plot->plotLayout()->addElement(0,0,title);
        }
        else
        {
            title=(QCPTextElement *)plot->plotLayout()->elementAt(0);
            title->setText(plotTitle);
        }
    }
    else
    {
        if(prevItem!=nullptr)
        {
        plotTitle=QString("Données historiques et prévision pour %1<br>(%2)").arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),MODES::modeToCompleteString(mode));
        }
        else
        {
            plotTitle=QString("Données historiques pour %1<br>(%2)").arg(auditModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),MODES::modeToCompleteString(mode));
        }
    }
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
        for(int i=(int)PCx_Audit::ORED::OUVERTS;i<4;i++)
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
    for(int i=(int)PCx_Audit::ORED::OUVERTS;i<4;i++)
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
    double diffRange=range.upper*0.5;
    range.lower-=(diffRange/3);
    range.upper+=(diffRange);
    plot->yAxis->setRange(range);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->legend->setFont(QFont("DejaVu Sans"));
    if(miniMode==false)
    {
        plot->legend->setVisible(true);
        plot->legend->setFont(QFont("DejaVu Sans",7));
        plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignCenter);
    }

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    plot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(1.0);

    plot->xAxis->setSubTickLength(0);
    plot->xAxis->setTickLength(4);

    plot->xAxis->setTickLabelRotation(0);
    plot->yAxis->setTickLabelFont(QFont("DejaVu Sans"));
    plot->xAxis->setTickLabelFont(QFont("DejaVu Sans"));

    if(miniMode==true)
    {
        plot->xAxis->setTickLabelRotation(45);
        plot->yAxis->setTickLabelFont(QFont("DejaVu Sans",7));
        plot->xAxis->setTickLabelFont(QFont("DejaVu Sans",7));
    }

    plot->yAxis->setLabel("€");
        plot->yAxis->setNumberFormat("gbc");

    plot->xAxis->setRange(dataX.first()-0.8,dataX.last()+0.8);
    plot->xAxis->grid()->setVisible(false);

    plot->replot();
    return plotTitle;
}





QString PCx_Graphics::getPCRHistory(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR)
{
    if(reportingModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    if(ownPlot==true)
    {
        if(plot!=nullptr)
        {
            delete plot;
        }
        plot=new QCustomPlot();
    }
    else
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->clearPlottables();
    }

    QString plotTitle=QObject::tr("%1\n(%2)").arg(reportingModel->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped(),MODES::modeToCompleteString(mode));

    QCPTextElement * title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPTextElement(plot,plotTitle);
        title->setFont(QFont("DejaVu Sans",11));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPTextElement *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }

    if(selectedOREDPCR.isEmpty())
    {
        plot->replot();
        return QString();
    }

    //Colors for each graph
    Qt::GlobalColor PENCOLORS[(int)PCx_Reporting::OREDPCR::NONELAST]=
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
    QVector<double> dataX,dataY[(int)PCx_Reporting::OREDPCR::NONELAST];

    while(q.next())
    {
        unsigned int date=q.value("date").toUInt();
        dataX.append((double)date);

        for(int i=(int)PCx_Reporting::OREDPCR::OUVERTS;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
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
    for(int i=(int)PCx_Reporting::OREDPCR::OUVERTS;i<(int)PCx_Reporting::OREDPCR::NONELAST;i++)
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
    plot->legend->setFont(QFont("DejaVu Sans",8));
    plot->legend->setRowSpacing(-5);
    plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);

    plot->xAxis->setTickLabelRotation(0);
    plot->yAxis->setLabel("€");

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("MMM\nyyyy");

    plot->xAxis->setTicker(dateTimeTicker);

    plot->replot();
    return plotTitle;
}

QString PCx_Graphics::getPCRProvenance(unsigned int nodeId, MODES::DFRFDIRI mode)
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);

    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::OREDPCR::BP,
                                                PCx_Reporting::OREDPCR::REPORTS,
                                                PCx_Reporting::OREDPCR::OCDM,
                                                PCx_Reporting::OREDPCR::VCDM,
                                                PCx_Reporting::OREDPCR::VIREMENTSINTERNES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Provenance des crédits de\n%1\n(%2)").arg(nodeName,modeName),getColorDFBar());
}

QString PCx_Graphics::getPCRVariation(unsigned int nodeId, MODES::DFRFDIRI mode)
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);

    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::OREDPCR::OCDM,
                                                PCx_Reporting::OREDPCR::VCDM,
                                                PCx_Reporting::OREDPCR::VIREMENTSINTERNES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::BP,QString("Facteurs de variation des crédits de\n%1\n(%2)").arg(nodeName,modeName),getColorRFBar());
}

QString PCx_Graphics::getPCRUtilisation(unsigned int nodeId, MODES::DFRFDIRI mode)
{
    QString nodeName=reportingModel->getAttachedTree()->getNodeName(nodeId);
    QString modeName=MODES::modeToCompleteString(mode);
    QList<PCx_Reporting::OREDPCR> selectedORED={PCx_Reporting::OREDPCR::REALISES,
                                                PCx_Reporting::OREDPCR::ENGAGES,
                                                PCx_Reporting::OREDPCR::DISPONIBLES
                                               };
    return getPCRPercentBars(nodeId,mode,selectedORED,PCx_Reporting::OREDPCR::OUVERTS,QString("Utilisation des crédits de\n%1\n(%2)").arg(nodeName,modeName),getColorDIBar());
}

QString PCx_Graphics::getPCRCycles(unsigned int nodeId, MODES::DFRFDIRI mode)
{
    QList<PCx_Reporting::OREDPCR> oredPCR={PCx_Reporting::OREDPCR::OUVERTS,PCx_Reporting::OREDPCR::REALISES};
    return getPCRHistory(nodeId,mode,oredPCR);
}

QCustomPlot * PCx_Graphics::getPlot() const
{
    return plot;
}

QString PCx_Graphics::getPCRPercentBars(unsigned int selectedNodeId, MODES::DFRFDIRI mode, QList<PCx_Reporting::OREDPCR> selectedOREDPCR, PCx_Reporting::OREDPCR oredReference, const QString &plotTitle,QColor color)
{
    if(reportingModel==nullptr)
    {
        qWarning()<<"Model error";
        return QString();
    }

    if(ownPlot==true)
    {
        if(plot!=nullptr)
        {
            delete plot;
        }
        plot=new QCustomPlot();
    }
    else
    {
        plot->clearGraphs();
        plot->clearItems();
        plot->clearPlottables();
    }

    QCPTextElement * title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPTextElement(plot,plotTitle);
        title->setFont(QFont("DejaVu Sans",11));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPTextElement *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }

    QCPBars *bars=new QCPBars(plot->xAxis,plot->yAxis);

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
    QVector<double> values;

    int i=1;

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    plot->xAxis->setTicker(textTicker);


    foreach(PCx_Reporting::OREDPCR ored,selectedOREDPCR)
    {
        textTicker->addTick(i,PCx_Reporting::OREDPCRtoCompleteString(ored));
        ticks.append(i);
        i++;
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


    plot->yAxis->setLabel("%");
    plot->xAxis->setTickLength(0,0);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setRange(0,ticks.count()+1);
    plot->yAxis->rescale();

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
    return "\ndiv.g{margin-top:2em;margin-left:auto;margin-right:auto;page-break-inside:avoid;}";
}



