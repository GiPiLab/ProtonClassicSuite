#include "dialogdisplaytree.h"
#include "ui_dialogdisplaytree.h"
#include <QtPrintSupport/QtPrintSupport>


DialogDisplayTree::DialogDisplayTree(PCx_TreeModel * treeModel,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDisplayTree)
{
    ui->setupUi(this);
    model=new PCx_TreeModel(treeModel->getTreeId());
    ui->treeView->setModel(model);
    QDateTime dt=QDateTime::currentDateTime();
    ui->label->setText(tr("Arbre %1 le %2 à %3").arg(model->getName()).arg(dt.date().toString("dd/MM/yyyy")).arg(dt.time().toString()));
    ui->treeView->expandToDepth(1);
}

DialogDisplayTree::~DialogDisplayTree()
{
    delete model;
    delete ui;
}

void DialogDisplayTree::on_printViewButton_clicked()
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

}
