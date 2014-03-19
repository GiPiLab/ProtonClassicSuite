#include "dialogaddchangetreenode.h"
#include "ui_dialogaddchangetreenode.h"

DialogAddChangeTreeNode::DialogAddChangeTreeNode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddChangeTreeNode)
{
    ui->setupUi(this);
}

DialogAddChangeTreeNode::~DialogAddChangeTreeNode()
{
    delete ui;
}
