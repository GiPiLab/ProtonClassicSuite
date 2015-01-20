#include "formauditprevisions.h"
#include "ui_formauditprevisions.h"

FormAuditPrevisions::FormAuditPrevisions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditPrevisions)
{
    ui->setupUi(this);
}

FormAuditPrevisions::~FormAuditPrevisions()
{
    delete ui;
}
