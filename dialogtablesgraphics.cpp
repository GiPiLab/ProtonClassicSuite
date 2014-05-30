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
    favoriteGraphicsWidth=650;
    favoriteGraphicsHeight=400;
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    ui->plotG1G8->setHidden(true);

    doc=new QTextDocument();
    ui->textEdit->setDocument(doc);
    //NOTE : For vectorized graphics
    //interface = new QCPDocumentObject(this);
    //ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, interface);
    updateListOfAudits();
    updateTextBrowser();
    ui->textEdit->moveCursor(QTextCursor::Start);
    //int favoriteGraphicsWidth=(int)(ui->textEdit->width()*0.85);
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
    //do not update text browser if no audit are available
    bool nonEmpty=!listOfAudits.isEmpty();
    this->setEnabled(nonEmpty);
    ready=nonEmpty;
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
    ui->saveButton->setEnabled(ready);
    if(!ready)
    {
        doc->setHtml(tr("<h1 align='center'><br><br><br><br><br>Remplissez un audit et n'oubliez pas de le terminer</h1>"));
        return;
    }

    unsigned int selectedNode;
    quint8 tabsMask=0;
    quint16 graphicsMask=0;
    DFRFDIRI mode=DF;
    getSelections(&selectedNode,&tabsMask,&graphicsMask,&mode);

    QScrollBar *sb=ui->textEdit->verticalScrollBar();
    int sbval=sb->value();

    doc->clear();

    QString output=model->generateHTMLReportForNode(tabsMask,0,graphicsMask,selectedNode,mode,ui->plotG1G8,
                                                    favoriteGraphicsWidth,favoriteGraphicsHeight);
    doc->setHtml(output);
    sb->setValue(sbval);
}

void DialogTablesGraphics::getSelections(unsigned int *selectedNode, quint8 *selectedTablePages, quint16 *selectedGraphics, DFRFDIRI *selectedMode)
{
    DFRFDIRI mode=DF;

    if(ui->radioButtonDF->isChecked())
        mode=DF;

    else if(ui->radioButtonRF->isChecked())
        mode=RF;

    else if(ui->radioButtonDI->isChecked())
        mode=DI;

    else if(ui->radioButtonRI->isChecked())
        mode=RI;

    else if(ui->radioButtonGlobal->isChecked())
        mode=GLOBAL;

    else
    {
        qCritical()<<"Unsupported case of radio button checking";
        die();
    }

    quint8 tabsMask=0;
    quint16 graphicsMask=0;

    if(mode!=GLOBAL)
    {
        if(ui->checkBoxPoidsRelatif->isChecked())
            tabsMask|=TABRECAP;

        if(ui->checkBoxEvolution->isChecked())
            tabsMask|=TABEVOLUTION;

        if(ui->checkBoxEvolutionCumul->isChecked())
            tabsMask|=TABEVOLUTIONCUMUL;

        if(ui->checkBoxBase100->isChecked())
            tabsMask|=TABBASE100;

        if(ui->checkBoxJoursAct->isChecked())
            tabsMask|=TABJOURSACT;

        if(ui->checkBoxPrevu->isChecked())
            graphicsMask|=G1;

        if(ui->checkBoxPrevuCumul->isChecked())
            graphicsMask|=G2;

        if(ui->checkBoxRealise->isChecked())
            graphicsMask|=G3;

        if(ui->checkBoxRealiseCumul->isChecked())
            graphicsMask|=G4;

        if(ui->checkBoxEngage->isChecked())
            graphicsMask|=G5;

        if(ui->checkBoxEngageCumul->isChecked())
            graphicsMask|=G6;

        if(ui->checkBoxDisponible->isChecked())
            graphicsMask|=G7;

        if(ui->checkBoxDisponibleCumul->isChecked())
            graphicsMask|=G8;
    }

    else
    {
        if(ui->checkBoxResults->isChecked())
            tabsMask|=TABRESULTS;

        if(ui->checkBoxRecapGraph->isChecked())
            graphicsMask|=G9;
    }
    *selectedTablePages=tabsMask;
    *selectedGraphics=graphicsMask;
    *selectedMode=mode;
    *selectedNode=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();
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
    Q_UNUSED(index);
   // ui->groupBoxMode->setTitle(index.data().toString());

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

//FIXME : page split problem
/*void DialogTablesGraphics::on_printButton_clicked()
{
    QPrinter p;
    QPrintDialog *dialog = new QPrintDialog(&p, this);
    dialog->setWindowTitle(tr("Imprimer"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    doc->print(&p);
}*/

void DialogTablesGraphics::on_saveButton_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QDir::home());
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible"));
        return;
    }

    QTextStream stream(&file);

    quint8 tabs;
    quint16 graphs;
    DFRFDIRI mode;
    unsigned int node;
    getSelections(&node,&tabs,&graphs,&mode);

    QString output=model->generateHTMLReportForNode(tabs,0,graphs,node,mode,ui->plotG1G8,favoriteGraphicsWidth,favoriteGraphicsHeight,2.0,true);
    //Pass HTML through a temp QTextDocument to reinject css into tags (more compatible with text editors)
    QTextDocument formattedOut;
    formattedOut.setHtml(output);
    stream<<formattedOut.toHtml("utf-8");
    stream.flush();
    if(stream.status()==QTextStream::Ok)
        QMessageBox::information(this,tr("Information"),"Le document a bien été enregistré");
    else
        QMessageBox::critical(this,tr("Attention"),"Le document n'a pas pu être enregistré !");
    file.close();
}

void DialogTablesGraphics::on_checkBoxResults_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
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
