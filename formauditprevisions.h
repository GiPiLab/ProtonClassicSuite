#ifndef FORMAUDITPREVISIONS_H
#define FORMAUDITPREVISIONS_H

#include <QWidget>
#include "pcx_prevision.h"

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
    void onListOfPrevisionsChanged();
    void onSettingsChanged();


private slots:
    void on_treeView_clicked(const QModelIndex &index);


    void on_comboListPrevisions_activated(int index);
private:

    PCx_Prevision *previsionModel;
    Ui::FormAuditPrevisions *ui;
    void updateListOfPrevisions();
};

#endif // FORMAUDITPREVISIONS_H
