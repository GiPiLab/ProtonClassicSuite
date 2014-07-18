#include "dialogduplicateaudit.h"
#include "ui_dialogduplicateaudit.h"
#include <QMessageBox>

DialogDuplicateAudit::DialogDuplicateAudit(unsigned int oldAuditId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDuplicateAudit)
{
    Q_ASSERT(oldAuditId>0);
    model=new PCx_AuditModel(oldAuditId,parent,true);
    ui->setupUi(this);
    ui->labelOldName->setText(model->getAuditInfos().name);
    ui->labelOldDate->setText(model->getAuditInfos().creationTimeLocal.toString(Qt::SystemLocaleLongDate));
    ui->labelOldTree->setText(model->getAuditInfos().attachedTreeName);
    ui->labelOldYears->setText(model->getAuditInfos().yearsString);
    ui->spinBoxYear1->setValue(model->getAuditInfos().years.first());
    ui->spinBoxYear2->setValue(model->getAuditInfos().years.last());
    QDate date=QDate::currentDate();
    ui->spinBoxYear1->setMaximum(date.year());
    ui->spinBoxYear2->setMaximum(date.year()+1);
}

DialogDuplicateAudit::~DialogDuplicateAudit()
{
    delete model;
    delete ui;
}

void DialogDuplicateAudit::on_pushButton_clicked()
{
    QString newName=ui->lineEditNewName->text();
    if(newName.isEmpty())
    {
        QMessageBox::information(this,tr("Attention"),tr("Donnez un nom au nouvel audit"));
        return;
    }

    if(model->auditNameExists(newName))
    {
        QMessageBox::warning(this,tr("Attention"),tr("Il existe déjà un audit portant ce nom"));
        return;
    }

    unsigned int year1=ui->spinBoxYear1->value();
    unsigned int year2=ui->spinBoxYear2->value();

    if(year1>=year2)
    {
        QMessageBox::warning(this,tr("Attention"),tr("L'audit doit porter sur au moins deux années"));
        return;
    }
    //DUPLICATE AUDIT

    QList<unsigned int> years;
    for(unsigned int i=year1;i<=year2;i++)
        years.append(i);

    unsigned int res=model->duplicateAudit(newName,years,ui->checkBoxDF->isChecked(),ui->checkBoxRF->isChecked(),
                          ui->checkBoxDI->isChecked(),ui->checkBoxRI->isChecked());

    if(res>0)
    {
        QMessageBox::information(this,"",tr("Audit dupliqué !"));
    }
    done(Accepted);
}

void DialogDuplicateAudit::on_pushButton_2_clicked()
{
    done(Rejected);
}
