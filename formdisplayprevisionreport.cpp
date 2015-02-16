#include "formdisplayprevisionreport.h"
#include "ui_formdisplayprevisionreport.h"

FormDisplayPrevisionReport::FormDisplayPrevisionReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDisplayPrevisionReport)
{
    ui->setupUi(this);
}

FormDisplayPrevisionReport::~FormDisplayPrevisionReport()
{
    delete ui;
}
