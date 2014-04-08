#include "dialogeditaudit.h"
#include "ui_dialogeditaudit.h"
#include "pcx_auditmodel.h"

DialogEditAudit::DialogEditAudit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditAudit)
{
    ui->setupUi(this);
    updateListOfAudits();
}

DialogEditAudit::~DialogEditAudit()
{
    delete ui;
}

void DialogEditAudit::updateListOfAudits()
{
    ui->comboListAudits->clear();

    QHash<int,QString> listOfAudits=PCx_AuditModel::getListOfAudits(false);
    foreach(int auditId,listOfAudits.keys())
    {
        ui->comboListAudits->insertItem(0,listOfAudits[auditId],auditId);
    }
    ui->comboListAudits->setCurrentIndex(0);
}
