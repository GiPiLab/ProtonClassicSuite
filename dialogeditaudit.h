#ifndef DIALOGEDITAUDIT_H
#define DIALOGEDITAUDIT_H

#include <QDialog>
#include <QtSql>
#include "pcx_auditmodel.h"

namespace Ui {
class DialogEditAudit;
}

class DialogEditAudit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditAudit(QWidget *parent = 0);
    ~DialogEditAudit();

private slots:
    void on_comboListAudits_activated(int index);

    void on_treeView_activated(const QModelIndex &index);

private:
    Ui::DialogEditAudit *ui;
    void updateListOfAudits();
    PCx_AuditModel *auditModel;
};

#endif // DIALOGEDITAUDIT_H
