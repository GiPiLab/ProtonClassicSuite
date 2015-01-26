#ifndef FORMAUDITPREVISIONS_H
#define FORMAUDITPREVISIONS_H

#include <QWidget>
#include "pcx_prevision.h"
#include "pcx_previsionitemtablemodel.h"

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
    void on_comboBoxOperators_activated(int index);



    void on_pushButtonAddCriteriaToAdd_clicked();

    void on_pushButtonAddCriteriaToSubstract_clicked();

    void on_pushButtonDelCriteria_clicked();

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_pushButtonDeleteAll_clicked();

private:

    PCx_Prevision *previsionModel;
    PCx_AuditWithTreeModel *auditWithTreeModel;
    PCx_PrevisionItemTableModel *previsionItemTableModel;
    MODES::DFRFDIRI currentMode;
    unsigned int currentNodeId;
    Ui::FormAuditPrevisions *ui;
    void updateListOfPrevisions();
    void updatePrevisionItemListModel();
    void updateLabels();
};

#endif // FORMAUDITPREVISIONS_H
