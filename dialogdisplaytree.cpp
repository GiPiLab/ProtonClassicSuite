#include "dialogdisplaytree.h"
#include "ui_dialogdisplaytree.h"

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
