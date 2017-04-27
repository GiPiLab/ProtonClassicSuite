#include "formdisplaytree.h"
#include "ui_formdisplaytree.h"
#include <QtPrintSupport/QtPrintSupport>
#include "utils.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

FormDisplayTree::FormDisplayTree(unsigned int treeId, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormDisplayTree)
{
    ui->setupUi(this);
    model=new PCx_TreeModel(treeId);
    ui->treeView->setModel(model);
    QDateTime dt=QDateTime::currentDateTime();
    ui->label->setText(tr("Arbre %1 le %2 à %3").arg(model->getName(),dt.date().toString("dd/MM/yyyy"),dt.time().toString()));
    ui->treeView->expandToDepth(1);
}

FormDisplayTree::~FormDisplayTree()
{
    delete model;
    delete ui;
}

void FormDisplayTree::on_printViewButton_clicked()
{
#ifndef Q_OS_ANDROID
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
#else
    QMessageBox::warning(this,tr("Non supporté"),tr("Fonction non supportée sur cette plateforme"));
#endif
}

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
#ifdef Q_OS_ANDROID
    QMessageBox::warning(this,tr("Non supporté"),tr("Fonction non supportée sur cette plateforme"));
    return;
#endif

    QList<unsigned int> listOfTrees=PCx_Tree::getListOfTreesId();
    if(!listOfTrees.contains(model->getTreeId()))
    {
        QMessageBox::warning(this,tr("Attention"),tr("Cet arbre a été supprimé !"));
        return;
    }
    QString dot=model->toDot();

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
        QMessageBox::warning(this,tr("Echec"),tr("Enregistrement du PDF impossible"));
        return;
    }
    QMessageBox::information(this,tr("Succès"),tr("Arbre enregistré au format PDF"));

    if(question(tr("Enregistrer aussi la source au format GraphViz ?"))==QMessageBox::Yes)
    {
        QString fileName = fileDialog.getSaveFileName(this, tr("Enregistrer la source de l'arbre au format DOT"), "",tr("Fichiers Graphviz (*.dot)"));
        if(fileName.isEmpty())
            return;

        QFileInfo fi(fileName);
        if(fi.suffix().compare("dot",Qt::CaseInsensitive)!=0)
            fileName.append(".dot");
        fi=QFileInfo(fileName);

        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QMessageBox::critical(this,tr("Attention"),tr("Ouverture du fichier impossible : %1").arg(file.errorString()));
            return;
        }
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream<<dot;
        file.close();
        QMessageBox::information(this,tr("Succès"),tr("Source de l'arbre enregistrée au format DOT"));
    }

}



void FormDisplayTree::on_exportXLSXButton_clicked()
{
    QList<unsigned int> listOfTrees=PCx_Tree::getListOfTreesId();
    if(!listOfTrees.contains(model->getTreeId()))
    {
        QMessageBox::warning(this,tr("Attention"),tr("Cet arbre a été supprimé !"));
        return;
    }
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


