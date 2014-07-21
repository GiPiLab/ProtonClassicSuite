#include "formtablesgraphics.h"
#include "ui_formtablesgraphics.h"
#include "utils.h"
#include <QScrollBar>
#include <QPrintDialog>
#include <QFileDialog>
//#include "QCustomPlot/qcpdocumentobject.h"

FormTablesGraphics::FormTablesGraphics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTablesGraphics)
{
    model=NULL;
    ready=false;
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);

    doc=new QTextDocument();
    ui->textEdit->setDocument(doc);
    //NOTE : For vectorized graphics
    //interface = new QCPDocumentObject(this);
    //ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, interface);
    updateListOfAudits();
    ui->textEdit->moveCursor(QTextCursor::Start);
}

FormTablesGraphics::~FormTablesGraphics()
{
    delete ui;
    delete doc;
    //delete interface;
    if(model!=NULL)
    {
        delete model;
        delete report;
    }
}

void FormTablesGraphics::onListOfAuditsChanged()
{
    updateListOfAudits();
}


void FormTablesGraphics::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QList<QPair<unsigned int,QString> >listOfAudits=PCx_AuditModel::getListOfAudits(PCx_AuditModel::FinishedAuditsOnly);
    //do not update text browser if no audit are available
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));

    ready=nonEmpty;
    QPair<unsigned int, QString> p;
    foreach(p,listOfAudits)
    {
        ui->comboListAudits->insertItem(0,p.second,p.first);
    }
    ui->comboListAudits->setCurrentIndex(0);
    on_comboListAudits_activated(0);
}


void FormTablesGraphics::updateTextBrowser()
{
    ui->saveButton->setEnabled(ready);
    if(!ready)
    {
        doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));
        return;
    }

    unsigned int selectedNode=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();

    getSelections();
    QScrollBar *sb=ui->textEdit->verticalScrollBar();
    int sbval=sb->value();

    doc->clear();

    QSettings settings;
    report->getGraphics().setGraphicsWidth(settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt());
    report->getGraphics().setGraphicsHeight(settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt());

    //Always scale 1.0 when displaying
    report->getGraphics().setScale(1.0);

    QString output=report->generateHTMLHeader();
    output.append(report->generateHTMLReportForNode(selectedTabs,QList<PCx_Tables::TABLES>(),selectedGraphics,selectedNode,selectedMode,doc));
    output.append("</body></html>");
    doc->setHtml(output);
    sb->setValue(sbval);
}


void FormTablesGraphics::getSelections()
{
    if(ui->radioButtonDF->isChecked())
        selectedMode=PCx_AuditModel::DF;

    else if(ui->radioButtonRF->isChecked())
        selectedMode=PCx_AuditModel::RF;

    else if(ui->radioButtonDI->isChecked())
        selectedMode=PCx_AuditModel::DI;

    else if(ui->radioButtonRI->isChecked())
        selectedMode=PCx_AuditModel::RI;

    else if(ui->radioButtonGlobal->isChecked())
        selectedMode=PCx_AuditModel::GLOBAL;

    else
    {
        qWarning()<<"Unsupported case of radio button checking";
        die();
    }

    selectedGraphics.clear();
    selectedTabs.clear();

    if(selectedMode!=PCx_AuditModel::GLOBAL)
    {
        if(ui->checkBoxPoidsRelatif->isChecked())
            selectedTabs.append(PCx_Tables::TABRECAP);

        if(ui->checkBoxEvolution->isChecked())
            selectedTabs.append(PCx_Tables::TABEVOLUTION);

        if(ui->checkBoxEvolutionCumul->isChecked())
            selectedTabs.append(PCx_Tables::TABEVOLUTIONCUMUL);

        if(ui->checkBoxBase100->isChecked())
            selectedTabs.append(PCx_Tables::TABBASE100);

        if(ui->checkBoxJoursAct->isChecked())
            selectedTabs.append(PCx_Tables::TABJOURSACT);

        if(ui->checkBoxPrevu->isChecked())
            selectedGraphics.append(PCx_Graphics::G1);

        if(ui->checkBoxPrevuCumul->isChecked())
            selectedGraphics.append(PCx_Graphics::G2);

        if(ui->checkBoxRealise->isChecked())
            selectedGraphics.append(PCx_Graphics::G3);

        if(ui->checkBoxRealiseCumul->isChecked())
            selectedGraphics.append(PCx_Graphics::G4);

        if(ui->checkBoxEngage->isChecked())
            selectedGraphics.append(PCx_Graphics::G5);

        if(ui->checkBoxEngageCumul->isChecked())
            selectedGraphics.append(PCx_Graphics::G6);

        if(ui->checkBoxDisponible->isChecked())
            selectedGraphics.append(PCx_Graphics::G7);

        if(ui->checkBoxDisponibleCumul->isChecked())
            selectedGraphics.append(PCx_Graphics::G8);
    }

    else
    {
        if(ui->checkBoxResults->isChecked())
            selectedTabs.append(PCx_Tables::TABRESULTS);

        if(ui->checkBoxRecapGraph->isChecked())
            selectedGraphics.append(PCx_Graphics::G9);
    }
}



void FormTablesGraphics::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

    if(model!=NULL)
    {
        delete model;
        delete report;
    }
    model=new PCx_AuditModel(selectedAuditId,this);
    report=new PCx_Report(model);

    QItemSelectionModel *m=ui->treeView->selectionModel();
    ui->treeView->setModel(model->getAttachedTreeModel());
    delete m;
    ui->treeView->expandToDepth(1);
    QModelIndex rootIndex=model->getAttachedTreeModel()->index(0,0);
    ui->treeView->setCurrentIndex(rootIndex);
    updateTextBrowser();
}

void FormTablesGraphics::on_treeView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
   // ui->groupBoxMode->setTitle(index.data().toString());

    updateTextBrowser();
}

void FormTablesGraphics::on_radioButtonGlobal_toggled(bool checked)
{
    if(checked)
    {
        selectedMode=PCx_AuditModel::GLOBAL;
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


void FormTablesGraphics::on_saveButton_clicked()
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

    unsigned int node=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();
    getSelections();

    int maximumProgressValue=selectedGraphics.count();

    QProgressDialog progress(tr("Enregistrement en cours..."),0,0,maximumProgressValue);
    progress.setMinimumDuration(1000);

    progress.setWindowModality(Qt::ApplicationModal);
    progress.setValue(0);

    QSettings settings;

    report->getGraphics().setGraphicsWidth(settings.value("graphics/width",PCx_Graphics::DEFAULTWIDTH).toInt());
    report->getGraphics().setGraphicsHeight(settings.value("graphics/height",PCx_Graphics::DEFAULTHEIGHT).toInt());
    report->getGraphics().setScale(settings.value("graphics/scale",PCx_Graphics::DEFAULTSCALE).toDouble());

    //Generate report in non-embedded mode, saving images
    QString output=report->generateHTMLHeader();
    output.append(report->generateHTMLReportForNode(selectedTabs,QList<PCx_Tables::TABLES>(),selectedGraphics,node,selectedMode,NULL,absoluteImagePath,relativeImagePath,&progress));
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
    progress.setValue(maximumProgressValue);
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),tr("Le document <b>%1</b> a bien été enregistré. Les images sont stockées dans le dossier <b>%2</b>").arg(fi.fileName()).arg(relativeImagePath));
    else
        QMessageBox::critical(this,tr("Attention"),tr("Le document n'a pas pu être enregistré !"));

}

void FormTablesGraphics::on_checkBoxResults_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxRecapGraph_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxPrevu_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxEngage_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxPrevuCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxEngageCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxRealise_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxDisponible_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxRealiseCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxDisponibleCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxPoidsRelatif_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();
    }
}

void FormTablesGraphics::on_radioButtonRF_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();
    }

}

void FormTablesGraphics::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();
    }
}

void FormTablesGraphics::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();
    }
}

void FormTablesGraphics::on_checkBoxEvolution_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxEvolutionCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxBase100_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void FormTablesGraphics::on_checkBoxJoursAct_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}
