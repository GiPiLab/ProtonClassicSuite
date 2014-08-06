#ifndef FORMAUDITINFOS_H
#define FORMAUDITINFOS_H

#include <QWidget>
#include "pcx_audit.h"

namespace Ui {
class FormAuditInfos;
}

class FormAuditInfos : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditInfos(unsigned int auditId,QWidget *parent = 0);
    ~FormAuditInfos();

private slots:
    void on_pushButton_clicked();

private:
    QSize sizeHint() const;

    Ui::FormAuditInfos *ui;
    unsigned int auditId;
    PCx_Audit *infos;
};

#endif // FORMAUDITINFOS_H
