#include "formreportingexplore.h"
#include "pcx_report.h"
#include "ui_formreportingexplore.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>


FormReportingExplore::FormReportingExplore(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportingExplore)
{
    ui->setupUi(this);
    selectedReporting=nullptr;
    document=new QTextDocument();
    report=nullptr;
    updateListOfReportings();
    ui->splitter->setStretchFactor(1,1);
}

FormReportingExplore::~FormReportingExplore()
{
    if(selectedReporting!=nullptr)
        delete selectedReporting;

    if(report!=nullptr)
        delete report;

    if(document!=nullptr)
        delete document;

    delete ui;
}

void FormReportingExplore::onListOfReportingsChanged()
{
    updateListOfReportings();
}

void FormReportingExplore::updateListOfReportings()
{
    ui->comboListReportings->clear();

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
        ui->comboListReportings->insertItem(0,p.second,p.first);
    }
    ui->comboListReportings->setCurrentIndex(0);
    this->on_comboListReportings_activated(0);
}

QSize FormReportingExplore::sizeHint() const
{
    return QSize(1000,500);
}

void FormReportingExplore::on_comboListReportings_activated(int index)
{
    if(index==-1 || ui->comboListReportings->count()==0)return;
    unsigned int selectedReportingId=ui->comboListReportings->currentData().toUInt();

    if(selectedReporting!=nullptr)
    {
        delete selectedReporting;
        selectedReporting=nullptr;
    }


    selectedReporting=new PCx_ReportingWithTreeModel(selectedReportingId);

    if(report!=nullptr)
    {
        delete report;
    }



    QSettings settings;
    int graphicsWidth=settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt();
    int graphicsHeight=settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt();

    report=new PCx_Report(selectedReporting,0,graphicsWidth,graphicsHeight,1.0);


    document->clear();


    ui->treeView->setModel(selectedReporting->getAttachedTree());
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=selectedReporting->getAttachedTree()->index(0,0);
    selectedNode=1;
    on_treeView_clicked(rootIndex);
}

QList<PCx_Report::PCRPRESETS> FormReportingExplore::getPresets() const
{
    QList<PCx_Report::PCRPRESETS> presets;
    if(ui->checkBoxA->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_A);
    if(ui->checkBoxB->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_B);
    if(ui->checkBoxC->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_C);
    if(ui->checkBoxD->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_D);
    if(ui->checkBoxS->isChecked())
        presets.append(PCx_Report::PCRPRESETS::PCRPRESET_S);
    return presets;
}

MODES::DFRFDIRI FormReportingExplore::getSelectedMode() const
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
        qWarning()<<"Invalid selection";
    }
    return MODES::DFRFDIRI::DF;
}


void FormReportingExplore::updateTextEdit()
{

    QScrollBar *sb=ui->textEdit->verticalScrollBar();
    int sbval=sb->value();

    QString output=selectedReporting->generateHTMLHeader()+selectedReporting->generateHTMLReportingTitle();
    document->clear();

    output.append(QString("<h1>%1</h1>").arg(selectedReporting->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped()));

    output.append(report->generateHTMLReportingReportForNode(getPresets(),selectedNode,getSelectedMode(),true,document));
    document->setHtml(output);

    ui->textEdit->setDocument(document);
    sb->setValue(sbval);
}


void FormReportingExplore::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int nodeId=index.data(PCx_TreeModel::NodeIdUserRole).toUInt();
    selectedNode=nodeId;
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxA_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxB_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxC_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxD_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_checkBoxS_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextEdit();
}

void FormReportingExplore::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updateTextEdit();
}

void FormReportingExplore::on_pushButtonExport_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    if(fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if(fi.suffix().compare("html",Qt::CaseInsensitive)!=0 && fi.suffix().compare("htm",Qt::CaseInsensitive)!=0)
        fileName.append(".html");
    fi=QFileInfo(fileName);


    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        return;
    }
    //Will reopen after computation
    file.close();
    file.remove();

    QString relativeImagePath=fi.fileName()+"_files";
    QString absoluteImagePath=fi.absoluteFilePath()+"_files";

    QFileInfo imageDirInfo(absoluteImagePath);

    if(!imageDirInfo.exists())
    {
        if(!fi.absoluteDir().mkdir(relativeImagePath))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Création du dossier des images impossible"));
            return;
        }
    }
    else
    {
        if(!imageDirInfo.isWritable())
        {
            QMessageBox::critical(this,tr("Attention"),tr("Ecriture impossible dans le dossier des images"));
            return;
        }
    }


    QSettings settings;

    report->getGraphics().setGraphicsWidth(settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt());
    report->getGraphics().setGraphicsHeight(settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt());
    report->getGraphics().setScale(settings.value("graphics/scale",PCx_Graphics::DEFAULTSCALE).toDouble());

    //Generate report in non-embedded mode, saving images
    QString output=selectedReporting->generateHTMLHeader();
    output.append(selectedReporting->generateHTMLReportingTitle());
    output.append(QString("<h1 id='node%2'>%1</h1>").arg(selectedReporting->getAttachedTree()->getNodeName(selectedNode).toHtmlEscaped()).arg(selectedNode));
    output.append(report->generateHTMLReportingReportForNode(getPresets(),selectedNode,getSelectedMode(),true,nullptr,absoluteImagePath,relativeImagePath));
    output.append("</body></html>");


    QString settingStyle=settings.value("output/style","CSS").toString();
    if(settingStyle=="INLINE")
    {
        //Pass HTML through a temp QTextDocument to reinject css into tags (more compatible with text editors)
        QTextDocument formattedOut;
        formattedOut.setHtml(output);
        output=formattedOut.toHtml("utf-8");

        //Cleanup the output a bit
        output.replace(" -qt-block-indent:0;","");
    }

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
        QDir dir(absoluteImagePath);
        dir.removeRecursively();
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<output;
    stream.flush();
    file.close();
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré. Les images sont stockées dans le dossier <b>%2</b>").arg(fi.fileName()).arg(relativeImagePath));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));
}
