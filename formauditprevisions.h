#ifndef FORMAUDITPREVISIONS_H
#define FORMAUDITPREVISIONS_H

#include <QWidget>
#include "pcx_auditwithtreemodel.h"

namespace Ui {
class FormAuditPrevisions;
}

class FormAuditPrevisions : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditPrevisions(QWidget *parent = 0);
    ~FormAuditPrevisions();

public slots:
    void onListOfAuditsChanged();
    void onSettingsChanged();


private slots:
    void on_treeView_clicked(const QModelIndex &index);


    void on_comboListAudits_activated(int index);
private:

    PCx_AuditWithTreeModel *auditModel;
    Ui::FormAuditPrevisions *ui;
    void updateListOfAudits();
};

#endif // FORMAUDITPREVISIONS_H
