#include "dialogt1t9.h"
#include "ui_dialogt1t9.h"

dialogT1T9::dialogT1T9(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dialogT1T9)
{
    ui->setupUi(this);
}

dialogT1T9::~dialogT1T9()
{
    delete ui;
}
