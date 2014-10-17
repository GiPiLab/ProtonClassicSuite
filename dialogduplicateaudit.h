#ifndef DIALOGDUPLICATEAUDIT_H
#define DIALOGDUPLICATEAUDIT_H

#include <QDialog>
#include "pcx_audit.h"

namespace Ui {
class DialogDuplicateAudit;
}

class DialogDuplicateAudit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDuplicateAudit(unsigned int oldAuditId,QWidget *parent = 0);
    ~DialogDuplicateAudit();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DialogDuplicateAudit *ui;
    unsigned int oldAuditId;
    PCx_Audit *oldAudit;
};

#endif // DIALOGDUPLICATEAUDIT_H
