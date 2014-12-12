#ifndef FORMEDITAUDIT_H
#define FORMEDITAUDIT_H

#include "pcx_editableauditmodel.h"
#include "auditdatadelegate.h"

namespace Ui {
class FormEditAudit;
}

class FormEditAudit : public QWidget
{
    Q_OBJECT


public:
    explicit FormEditAudit(QWidget *parent = 0);
    ~FormEditAudit();

public slots:
    void onListOfAuditsChanged();
    void updateRandomButtonVisibility();


private slots:
    void on_comboListAudits_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_randomDataButton_clicked();

    void on_clearDataButton_clicked();

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonExpandAll_clicked();

    void on_statsButton_clicked();

    void on_pushButtonExportLeaves_clicked();

    void on_pushButtonImportLeaves_clicked();

private:
    Ui::FormEditAudit *ui;
    void updateListOfAudits();
    PCx_EditableAuditModel *auditModel;
    QSize sizeHint()const;
    //Perhaps only would be sufficient ?
    auditDataDelegate *delegateDF,*delegateRF,*delegateDI,*delegateRI;
};

#endif // FORMEDITAUDIT_H
