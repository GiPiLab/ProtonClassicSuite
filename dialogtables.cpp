#include "dialogtables.h"
#include "ui_dialogtables.h"
#include "utils.h"
#include <QScrollBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include "QCustomPlot/qcpdocumentobject.h"

DialogTables::DialogTables(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogTables)
{
    model=NULL;
    ui->setupUi(this);
    //ui->plot->setHidden(true);
    doc=new QTextDocument();
    ui->splitter->setStretchFactor(1,1);
    updateListOfAudits();

    interface = new QCPDocumentObject(this);
    ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, interface);
}

DialogTables::~DialogTables()
{
    delete ui;
    delete doc;
    delete interface;
    if(model!=NULL)
        delete model;
}

void DialogTables::onListOfAuditsChanged()
{
    updateListOfAudits();
}

void DialogTables::updateListOfAudits()
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

void DialogTables::updateTextBrowser()
{
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
    ui->textEdit->clear();
    QString output=QString("<html><head><link rel='stylesheet' type='text/css' href='style.css'></head><body>"
            "<h3>Audit %1</h3><hr>").arg(model->getName().toHtmlEscaped());

    unsigned int selectedNode=ui->treeView->selectionModel()->currentIndex().data(Qt::UserRole+1).toUInt();

    //Mode DF,RF,DI,RI
    if(modeGlobal==false)
    {
        if(ui->checkBoxRecap->isChecked())
        {
                output.append(model->getTabRecap(selectedNode,mode));
        }
        if(ui->checkBoxEvolution->isChecked())
        {
                output.append(model->getTabEvolution(selectedNode,mode));
        }
        if(ui->checkBoxEvolutionCumul->isChecked())
        {
                output.append(model->getTabEvolutionCumul(selectedNode,mode));
        }
        if(ui->checkBoxBase100->isChecked())
        {
                output.append(model->getTabBase100(selectedNode,mode));
        }
        if(ui->checkBoxJoursAct->isChecked())
        {
                output.append(model->getTabJoursAct(selectedNode,mode));
        }

        QWidget *parent=ui->plot->parentWidget();
        model->getG1(selectedNode,mode,ui->plot);
        ui->plot->replot();
        //model->getG2(selectedNode,mode,ui->plot);



    }
    //Global mode
    else
    {
        output.append(model->getTabResults(selectedNode));
    }
   output.append(QString("<p align='center' style='font-size:6pt'>&copy;2006-%1 Laboratoire de Recherche pour le D&eacute;veloppement Local</p></body></html>").arg(QDate::currentDate().toString("yyyy")));

   doc->clear();
   doc->addResource(QTextDocument::StyleSheetResource,QUrl("style.css"),model->getCSS());
   doc->setHtml(output);

   ui->textEdit->setDocument(doc);
   sb->setValue(sbval);



   /*ui->plot->clearGraphs();

   ui->plot->addGraph();



   ui->plot->graph(0)->setData(x,y);
   QTextCursor cursor = ui->textEdit->textCursor();

     // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
     // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
     // into the text document.
     double width = 300.0;
     double height = 300.0;
     cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(ui->plot, width, height));

     ui->textEdit->setTextCursor(cursor);
     */






}


void DialogTables::on_comboListAudits_activated(int index)
{
    if(index==-1||ui->comboListAudits->count()==0)return;
    unsigned int selectedAuditId=ui->comboListAudits->currentData().toUInt();
    Q_ASSERT(selectedAuditId>0);
    qDebug()<<"Selected audit ID = "<<selectedAuditId;

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

void DialogTables::on_treeView_clicked(const QModelIndex &index)
{
    unsigned int selectedNode=index.data(Qt::UserRole+1).toUInt();
    Q_ASSERT(selectedNode>0);

    ui->groupBoxMode->setTitle(index.data().toString());

    updateTextBrowser();
}

void DialogTables::on_radioButtonGlobal_toggled(bool checked)
{
    if(checked)
    {
        updateTextBrowser();
        ui->pageTables->setEnabled(false);
    }
    else
    {
        ui->pageTables->setEnabled(true);
    }

}

void DialogTables::on_checkBoxRecap_toggled(bool checked)
{
    Q_UNUSED(checked);
        updateTextBrowser();
}

void DialogTables::on_radioButtonDF_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_radioButtonRF_toggled(bool checked)
{
   if(checked)
       updateTextBrowser();
}

void DialogTables::on_radioButtonDI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_radioButtonRI_toggled(bool checked)
{
    if(checked)
        updateTextBrowser();
}

void DialogTables::on_checkBoxEvolution_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTables::on_checkBoxEvolutionCumul_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTables::on_checkBoxBase100_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

void DialogTables::on_checkBoxJoursAct_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateTextBrowser();
}

//TODO : page split problem
void DialogTables::on_printButton_clicked()
{
    QPrinter p;
    QPrintDialog *dialog = new QPrintDialog(&p, this);
    dialog->setWindowTitle(tr("Imprimer"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    doc->print(&p);
}

void DialogTables::on_saveButton_clicked()
{
     QFileDialog fileDialog;
     fileDialog.setDirectory(QDir::home());
     QString file = fileDialog.getSaveFileName(this, tr("Enregistrer en HTML"), "",tr("Fichiers HTML (*.html *.htm)"));
     QTextDocumentWriter writer(file);
     writer.write(doc);
}
