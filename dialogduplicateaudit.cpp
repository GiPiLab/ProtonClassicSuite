#include "dialogduplicateaudit.h"
#include "ui_dialogduplicateaudit.h"
#include <QMessageBox>

DialogDuplicateAudit::DialogDuplicateAudit(unsigned int oldAuditId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDuplicateAudit),oldAuditId(oldAuditId)
{
    ui->setupUi(this);
    oldAudit=new PCx_Audit(oldAuditId);
    ui->labelOldName->setText(oldAudit->getAuditName());
    ui->labelOldDate->setText(oldAudit->getCreationTimeLocal().toString(Qt::SystemLocaleLongDate));
    ui->labelOldTree->setText(QString("%1 (%2 noeuds)").arg(oldAudit->getAttachedTreeName()).arg(oldAudit->getAttachedTree()->getNumberOfNodes()));
    ui->labelOldYears->setText(oldAudit->getYearsString());
    ui->spinBoxYear1->setValue(oldAudit->getYears().first());
    ui->spinBoxYear2->setValue(oldAudit->getYears().last());
    QDate date=QDate::currentDate();
    ui->spinBoxYear1->setMaximum(date.year());
    ui->spinBoxYear2->setMaximum(date.year()+1);
}

DialogDuplicateAudit::~DialogDuplicateAudit()
{
    delete oldAudit;
    delete ui;
}

void DialogDuplicateAudit::on_pushButton_clicked()
{
    QString newName=ui->lineEditNewName->text().simplified();
    if(newName.isEmpty())
    {
        QMessageBox::information(this,tr("Attention"),tr("Donnez un nom au nouvel audit"));
        return;
    }

    if(PCx_Audit::auditNameExists(newName))
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

    if(question(tr("Voulez-vous vraiment dupliquer cet audit ? L'opération peut nécessiter du temps et ne peut être interrompue."))==QMessageBox::Yes)
    {

        int res=oldAudit->duplicateAudit(newName,years,ui->checkBoxDF->isChecked(),ui->checkBoxRF->isChecked(),
                                         ui->checkBoxDI->isChecked(),ui->checkBoxRI->isChecked());

        if(res>0)
        {
            QMessageBox::information(this,"",tr("Audit dupliqué !"));
        }
        done(Accepted);
    }
}

void DialogDuplicateAudit::on_pushButton_2_clicked()
{
    done(Rejected);
}
