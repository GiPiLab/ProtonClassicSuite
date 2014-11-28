#include "formreportinggraphics.h"
#include "ui_formreportinggraphics.h"
#include "QCustomPlot/qcustomplot.h"
#include "pcx_graphics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QFileDialog>

FormReportingGraphics::FormReportingGraphics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingGraphics)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    //Root ID
    selectedNodeId=1;
    ui->splitter->setStretchFactor(1,1);
    updateListOfReportings();
}

FormReportingGraphics::~FormReportingGraphics()
{
    delete ui;
    if(selectedReporting!=nullptr)
        delete selectedReporting;
}

void FormReportingGraphics::onListOfReportingsChanged()
{
    updateListOfReportings();
}

void FormReportingGraphics::updateListOfReportings()
{
    ui->comboListOfReportings->clear();

    QList<QPair<unsigned int,QString> > listOfReportings=PCx_Reporting::getListOfReportings();
    QPair<unsigned int,QString> p;
    if(listOfReportings.count()==0)
    {
        setEnabled(false);
        return;
    }
    else
        setEnabled(true);

    foreach(p,listOfReportings)
    {
        ui->comboListOfReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListOfReportings->setCurrentIndex(0);
    this->on_comboListOfReportings_activated(0);
}


void FormReportingGraphics::on_comboListOfReportings_activated(int index)
{
    if(index==-1 || ui->comboListOfReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListOfReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }

    selectedReporting=new PCx_ReportingWithTreeModel(selectedReportingId);
    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNodeId=1;
    on_treeView_clicked(rootIndex);

}

void FormReportingGraphics::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    selectedNodeId=nodeId;
    updatePlot();
}

MODES::DFRFDIRI FormReportingGraphics::getSelectedMode() const
{
    if(ui->radioButtonDF->isChecked())
        return MODES::DFRFDIRI::DF;
    if(ui->radioButtonRF->isChecked())
        return MODES::DFRFDIRI::RF;
    if(ui->radioButtonDI->isChecked())
        return MODES::DFRFDIRI::DI;
    if(ui->radioButtonRI->isChecked())
        return MODES::DFRFDIRI::RI;

    else
    {
        qDebug()<<"Invalid selection";
    }
    return MODES::DFRFDIRI::DF;
}

QList<PCx_Reporting::OREDPCR> FormReportingGraphics::getSelectedOREDPCR() const
{
    QList<PCx_Reporting::OREDPCR> selection;
    if(ui->checkBoxBP->isChecked())
        selection.append(PCx_Reporting::OREDPCR::BP);
    if(ui->checkBoxReports->isChecked())
        selection.append(PCx_Reporting::OREDPCR::REPORTS);
    if(ui->checkBoxOCDM->isChecked())
        selection.append(PCx_Reporting::OREDPCR::OCDM);
    if(ui->checkBoxVCDM->isChecked())
        selection.append(PCx_Reporting::OREDPCR::VCDM);
    if(ui->checkBoxVInternes->isChecked())
        selection.append(PCx_Reporting::OREDPCR::VIREMENTSINTERNES);
    if(ui->checkBoxBudgetVote->isChecked())
        selection.append(PCx_Reporting::OREDPCR::BUDGETVOTE);
    if(ui->checkBoxRattachesN1->isChecked())
        selection.append(PCx_Reporting::OREDPCR::RATTACHENMOINS1);
    if(ui->checkBoxPrevus->isChecked())
        selection.append(PCx_Reporting::OREDPCR::OUVERTS);
    if(ui->checkBoxRealises->isChecked())
        selection.append(PCx_Reporting::OREDPCR::REALISES);
    if(ui->checkBoxEngages->isChecked())
        selection.append(PCx_Reporting::OREDPCR::ENGAGES);
    if(ui->checkBoxDisponibles->isChecked())
        selection.append(PCx_Reporting::OREDPCR::DISPONIBLES);
    return selection;
}

void FormReportingGraphics::updatePlot()
{
    MODES::DFRFDIRI mode=getSelectedMode();
    QList<PCx_Reporting::OREDPCR> selectedOREDPCR=getSelectedOREDPCR();

    QCustomPlot *plot=ui->plot;

    plot->clearItems();
    plot->clearGraphs();
    plot->clearPlottables();


    if(selectedOREDPCR.isEmpty())
    {
        plot->replot();
        return;
    }

    Qt::GlobalColor PENCOLORS[PCx_Reporting::OREDPCR::NONE]=
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


    q.prepare(QString("select * from reporting_%1_%2 where id_node=:id order by date").arg(MODES::modeToTableString(mode)).arg(selectedReporting->getReportingId()));
    q.bindValue(":id",selectedNodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    QVector<double> dataX,dataY[PCx_Reporting::OREDPCR::NONE];

    while(q.next())
    {
        unsigned int date=q.value("date").toUInt();
        dataX.append((double)date);

        for(int i=PCx_Reporting::OREDPCR::OUVERTS;i<PCx_Reporting::OREDPCR::NONE;i++)
        {
            if(selectedOREDPCR.contains((PCx_Reporting::OREDPCR)i))
            {
                qint64 data=q.value(PCx_Reporting::OREDPCRtoTableString((PCx_Reporting::OREDPCR)i)).toLongLong();
                dataY[i].append(NUMBERSFORMAT::fixedPointToDouble(data));
            }
        }
    }

    bool first=true;
    for(int i=PCx_Reporting::OREDPCR::OUVERTS;i<PCx_Reporting::OREDPCR::NONE;i++)
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

    QString plotTitle=QObject::tr("%1 (%2)").arg(selectedReporting->getAttachedTree()->getNodeName(selectedNodeId).toHtmlEscaped()).arg(MODES::modeToCompleteString(mode));

    QCPPlotTitle * title;
    if(plot->plotLayout()->elementCount()==1)
    {
        plot->plotLayout()->insertRow(0);
        title=new QCPPlotTitle(plot,plotTitle);
        title->setFont(QFont(QFont().family(),12));
        plot->plotLayout()->addElement(0,0,title);
    }
    else
    {
        title=(QCPPlotTitle *)plot->plotLayout()->elementAt(0);
        title->setText(plotTitle);
    }

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

}

void FormReportingGraphics::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updatePlot();
}

void FormReportingGraphics::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        updatePlot();
}

void FormReportingGraphics::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updatePlot();
}

void FormReportingGraphics::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updatePlot();
}

void FormReportingGraphics::on_checkBoxBP_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxReports_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxOCDM_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxVCDM_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxBudgetVote_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxVInternes_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxRattachesN1_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxPrevus_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxRealises_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxEngages_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

void FormReportingGraphics::on_checkBoxDisponibles_toggled(bool checked)
{
    Q_UNUSED(checked);
    updatePlot();
}

QSize FormReportingGraphics::sizeHint() const
{
    return QSize(800,500);
}

void FormReportingGraphics::on_pushButtonExportPlot_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en PNG"), "",tr("Images PNG (*.png)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("png",Qt::CaseInsensitive)!=0)
        fileName.append(".png");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        return;
    }
    file.close();

    if(ui->plot->savePng(fileName,0,0,4.0)==true)
        QMessageBox::information(this,tr("Information"),tr("Image enregistrée"));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Image non enregistrée !"));
}
