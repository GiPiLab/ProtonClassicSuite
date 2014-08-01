#include "formauditinfos.h"
#include "ui_formauditinfos.h"
#include "pcx_report.h"


FormAuditInfos::FormAuditInfos(unsigned int auditId,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuditInfos),auditId(auditId)
{
    infos=new PCx_AuditInfos(auditId);
    ui->setupUi(this);
    QString out=PCx_Report::generateHTMLHeader(auditId);

    out.append(infos->getHTMLAuditStatistics());
    ui->textEdit->setHtml(out);
}

FormAuditInfos::~FormAuditInfos()
{
    delete infos;
    delete ui;
}

QSize FormAuditInfos::sizeHint() const
{
    return QSize(650,450);
}
