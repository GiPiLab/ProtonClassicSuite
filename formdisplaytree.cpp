#include "formdisplaytree.h"
#include "ui_formdisplaytree.h"
#include <QtPrintSupport/QtPrintSupport>
#include "utils.h"
#include "pcx_stringdistancetablemodel.h"


FormDisplayTree::FormDisplayTree(unsigned int treeId, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormDisplayTree)
{
    ui->setupUi(this);
    model=new PCx_TreeModel(treeId);
    ui->treeView->setModel(model);
    QDateTime dt=QDateTime::currentDateTime();
    ui->label->setText(tr("Arbre %1 le %2 à %3").arg(model->getName()).arg(dt.date().toString("dd/MM/yyyy")).arg(dt.time().toString()));
    ui->treeView->expandToDepth(1);

    populateTableInfos();
    ui->tableWidget->resizeColumnsToContents();


    QStringList nodeNames=model->getListOfCompleteNodeNames();

    //qDebug()<<nodeNames;
    distanceModel=new PCx_StringDistanceTableModel(nodeNames);
    ui->similarityTableView->setModel(distanceModel);




}

FormDisplayTree::~FormDisplayTree()
{
    delete model;
    delete distanceModel;
    delete ui;
}

/*void FormDisplayTree::on_printViewButton_clicked()
{
    QPrinter printer;

       QPrintDialog *dialog = new QPrintDialog(&printer, this);
       dialog->setWindowTitle(tr("Imprimer"));
       if (dialog->exec() != QDialog::Accepted)
           return;
       QPainter painter;
       painter.begin(&printer);
       double xscale = printer.pageRect().width()/double(ui->treeView->width());
       double yscale = printer.pageRect().height()/double(ui->treeView->height());
       double scale = qMin(xscale, yscale)/2;
       painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                         printer.paperRect().y() + printer.pageRect().height()/2);
       painter.scale(scale, scale);
       painter.translate(-width()/2, -height()/2);

       ui->treeView->render(&painter);
}*/

void FormDisplayTree::on_expandButton_clicked()
{
    ui->treeView->expandAll();
}

void FormDisplayTree::on_collapseButton_clicked()
{
    ui->treeView->collapseAll();
    ui->treeView->expandToDepth(0);
}

void FormDisplayTree::on_exportButton_clicked()
{
    QString dot=model->toDot();

    qDebug()<<dot;

    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer l'arbre au format PDF"), "",tr("Fichiers PDF (*.pdf)"));
    if(fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    if(fi.suffix().compare("pdf",Qt::CaseInsensitive)!=0)
        fileName.append(".pdf");
    fi=QFileInfo(fileName);

    //Process dot graph
    if(dotToPdf(dot.toUtf8(),fileName)==false)
    {
        return;
    }
    QMessageBox::information(this,tr("Succès"),tr("Arbre enregistré au format PDF"));
}

void FormDisplayTree::populateTableInfos()
{
    ui->tableWidget->item(0,1)->setText(QString::number(model->getNumberOfNodes()));

    QList<QPair<unsigned int,QString> > listOfTypes=model->getTypes()->getAllTypes();
    QPair<unsigned int,QString> type;
    int row=ui->tableWidget->rowCount();

    foreach(type,listOfTypes)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        unsigned int nbOfThisType=model->getNumberOfNodesWithThisType(type.first);
        QTableWidgetItem *item1=new QTableWidgetItem("Noeuds de type "+type.second);
        QTableWidgetItem *item2=new QTableWidgetItem(QString::number(nbOfThisType));
        item2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->tableWidget->setItem(row,0,item1);
        ui->tableWidget->setItem(row,1,item2);
        row++;
    }
}

void FormDisplayTree::on_exportXLSXButton_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer l'arbre au format XLSX"), "",tr("Fichiers XLSX (*.xlsx)"));
    if(fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    if(fi.suffix().compare("xlsx",Qt::CaseInsensitive)!=0)
        fileName.append(".xlsx");
    fi=QFileInfo(fileName);


    bool res=model->toXLSX(fileName);

    if(res==true)
    {
        QMessageBox::information(this,tr("Succès"),tr("Arbre enregistré au format XLSX"));
    }
    else
    {
        QMessageBox::critical(this,tr("Erreur"),tr("Enregistrement impossible"));
    }
}
