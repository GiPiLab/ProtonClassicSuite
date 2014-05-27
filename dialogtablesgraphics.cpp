#include "dialogtablesgraphics.h"
#include "ui_dialogtablesgraphics.h"
#include "utils.h"
#include <QScrollBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
//#include "QCustomPlot/qcpdocumentobject.h"

DialogTablesGraphics::DialogTablesGraphics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogTablesGraphics)
{
    model=NULL;
    ready=false;
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    //We use two hidden QCustomPlot, one for G1G8 graphics with lines, one for G9 (histograms)
    ui->plotG1G8->setHidden(true);
    ui->plotG9->setHidden(true);

    doc=new QTextDocument();
    ui->textEdit->setDocument(doc);
    //NOTE : For vectorized graphics
    //interface = new QCPDocumentObject(this);
    //ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, interface);
    updateListOfAudits();
    ready=true;
    updateTextBrowser();
    ui->textEdit->moveCursor(QTextCursor::Start);
}

DialogTablesGraphics::~DialogTablesGraphics()
{
    delete ui;
    delete doc;
    //delete interface;
    if(model!=NULL)
        delete model;
}

void DialogTablesGraphics::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void DialogTablesGraphics::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(FinishedAuditsOnly);
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}

void DialogTablesGraphics::updateTextBrowser()
{
    if(!ready)return;
    imageNames.clear();
    DFRFDIRI mode=DF;
    bool modeGlobal=false;
    if(ui->radioButtonDF->isChecked())
    {
        mode=DF;
    }
    else if(ui->radioButtonRF->isChecked())
    {
        mode=RF;
    }
    else if(ui->radioButtonDI->isChecked())
    {
        mode=DI;
    }
    else if(ui->radioButtonRI->isChecked())
    {
        mode=RI;
    }
    else if(ui->radioButtonGlobal->isChecked())
    {
        modeGlobal=true;
    }
    else
    {
        qCritical()<<"Unsupported case of radio button checking";
        die();
    }

    QScrollBar *sb=ui->textEdit->verticalScrollBar();
    int sbval=sb->value();

    QString output=QString("<html><head><title>Audit %1</title><style type='text/css'>%2</style></head><body>"
                           "<h3>Audit %1</h3>").arg(model->getAuditInfos().name.toHtmlEscaped()).arg(model->getCSS());
    doc->clear();

    unsigned int selectedNode=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();

    int favoriteGraphicsWidth=(int)(ui->textEdit->width()*0.85);
    int favoriteGraphicsHeight=400;


    //Mode DF,RF,DI,RI
    if(modeGlobal==false)
    {
        if(ui->checkBoxPoidsRelatif->isChecked())
            output.append(model->getTabRecap(selectedNode,mode));

        if(ui->checkBoxEvolution->isChecked())
            output.append(model->getTabEvolution(selectedNode,mode));

        if(ui->checkBoxEvolutionCumul->isChecked())
            output.append(model->getTabEvolutionCumul(selectedNode,mode));

        if(ui->checkBoxBase100->isChecked())
            output.append(model->getTabBase100(selectedNode,mode));

        if(ui->checkBoxJoursAct->isChecked())
            output.append(model->getTabJoursAct(selectedNode,mode));

        //Graphics, a little too verbose
        //getGx draw the plot in the hidden QCustomPlot widget, which can be exported to pixmap and inserted into html with <img>
        if(ui->checkBoxPrevu->isChecked())
        {

            output.append("<p align='center'><b>"+model->getG1(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxPrevuCumul->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG2(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxRealise->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG3(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxRealiseCumul->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG4(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxEngage->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG5(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxEngageCumul->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG6(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxDisponible->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG7(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }
        if(ui->checkBoxDisponibleCumul->isChecked())
        {
            output.append("<p align='center'><b>"+model->getG8(selectedNode,mode,ui->plotG1G8)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));
        }

        //NOTE : For vectorized graphics
        //cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(ui->plot, 600, 400));
    }
    //Global mode
    else
    {
        if(ui->checkBoxResults->isChecked())
            output.append(model->getTabResults(selectedNode));
        if(ui->checkBoxRecapGraph->isChecked())
        {

            output.append("<p align='center'><b>"+model->getG9(selectedNode,ui->plotG9)+"</b></p>");
            QString imageName=insertPlotPixmapInDocResourceCache(ui->plotG9,favoriteGraphicsWidth,favoriteGraphicsHeight);
            output.append(QString("<div align='center'><img src='%1' alt='img'></div>").arg(imageName));

        }

    }
    output.append("</body></html>");
    doc->setHtml(output);

    sb->setValue(sbval);
}


void DialogTablesGraphics::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    //qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
    }
    model=new PCx_AuditModel(selectedAuditId,this);

    ui->treeView->setModel(model->getAttachedTreeModel());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    on_treeView_clicked(rootIndex);
}

void DialogTablesGraphics::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(selectedNode>0);

    ui->groupBoxMode->setTitle(index.data().toString());

    updateTextBrowser();
}

void DialogTablesGraphics::on_radioButtonGlobal_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();

        ui->checkBoxPoidsRelatif->setEnabled(false);
        ui->checkBoxBase100->setEnabled(false);
        ui->checkBoxEvolution->setEnabled(false);
        ui->checkBoxEvolutionCumul->setEnabled(false);
        ui->checkBoxJoursAct->setEnabled(false);
        ui->checkBoxResults->setEnabled(true);

        ui->checkBoxPrevu->setEnabled(false);
        ui->checkBoxPrevuCumul->setEnabled(false);
        ui->checkBoxRealise->setEnabled(false);
        ui->checkBoxRealiseCumul->setEnabled(false);
        ui->checkBoxEngage->setEnabled(false);
        ui->checkBoxEngageCumul->setEnabled(false);
        ui->checkBoxDisponible->setEnabled(false);
        ui->checkBoxDisponibleCumul->setEnabled(false);
        ui->checkBoxRecapGraph->setEnabled(true);
    }
    else
    {
        ui->checkBoxPoidsRelatif->setEnabled(true);
        ui->checkBoxBase100->setEnabled(true);
        ui->checkBoxEvolution->setEnabled(true);
        ui->checkBoxEvolutionCumul->setEnabled(true);
        ui->checkBoxJoursAct->setEnabled(true);
        ui->checkBoxResults->setEnabled(false);

        ui->checkBoxPrevu->setEnabled(true);
        ui->checkBoxPrevuCumul->setEnabled(true);
        ui->checkBoxRealise->setEnabled(true);
        ui->checkBoxRealiseCumul->setEnabled(true);
        ui->checkBoxEngage->setEnabled(true);
        ui->checkBoxEngageCumul->setEnabled(true);
        ui->checkBoxDisponible->setEnabled(true);
        ui->checkBoxDisponibleCumul->setEnabled(true);
        ui->checkBoxRecapGraph->setEnabled(false);
    }

}

void DialogTablesGraphics::on_checkBoxPoidsRelatif_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTablesGraphics::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTablesGraphics::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTablesGraphics::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxEvolution_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxEvolutionCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxBase100_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxJoursAct_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

//TODO : page split problem
void DialogTablesGraphics::on_printButton_clicked()
{
    QPrinter p;
    QPrintDialog *dialog = new QPrintDialog(&p, this);
    dialog->setWindowTitle(tr("Imprimer"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    doc->print(&p);
}

void DialogTablesGraphics::on_saveButton_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QDir::home());
    QString file = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    QTextDocumentWriter writer(file);
    writer.setFormat("html");
    writer.write(doc);
}

void DialogTablesGraphics::on_checkBoxResults_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

//Draw the plot to a pixmap and add it in the document resource cache. Returns its name
QString DialogTablesGraphics::insertPlotPixmapInDocResourceCache(QCustomPlot *plot, int width, int height) const
{
    Q_ASSERT(plot!=NULL && width>0 && height>0);
    QPixmap pixmap;
    QString imageName=QUuid::createUuid().toString()+".png";
    pixmap=plot->toPixmap(width,height);
    doc->addResource(QTextDocument::ImageResource,QUrl(imageName),QVariant(pixmap));
    return imageName;
}

void DialogTablesGraphics::on_checkBoxRecapGraph_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxPrevu_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxEngage_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxPrevuCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxEngageCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxRealise_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxDisponible_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxRealiseCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTablesGraphics::on_checkBoxDisponibleCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}
