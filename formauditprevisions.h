#ifndef FORMAUDITPREVISIONS_H
#define FORMAUDITPREVISIONS_H

#include <QWidget>
#include <QUrl>
#include "pcx_graphics.h"
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

    void onAnchorClicked(QUrl url);



    void on_pushButtonAddCriteriaToAdd_clicked();

    void on_pushButtonAddCriteriaToSubstract_clicked();

    void on_pushButtonDelCriteria_clicked();

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_pushButtonDeleteAll_clicked();

    void on_pushButtonApplyToNode_clicked();

    void on_pushButtonApplyToLeaves_clicked();

    void on_checkBoxDisplayLeafCriteria_toggled(bool checked);

    void on_pushButtonDisplayReport_clicked();

    void on_pushButtonSaveBigReport_clicked();

    void on_checkBoxShowCriteriaToApply_toggled(bool checked);

    void on_pushButtonExpandAll_clicked();

    void on_pushButtonCollapseAll_clicked();

private:

    PCx_Prevision *previsionModel;
    PCx_AuditWithTreeModel *auditWithTreeModel;
    PCx_PrevisionItem* currentPrevisionItem,*recentPrevisionItem;
    PCx_PrevisionItemTableModel *currentPrevisionItemTableModel,*recentPrevisionItemTableModel;

    MODES::DFRFDIRI currentMode;
    unsigned int currentNodeId;
    Ui::FormAuditPrevisions *ui;
    QSize sizeHint() const;
    PCx_Graphics *graphics;

    void updateListOfPrevisions();
    void updatePrevisionItemTableModel();
    void updateLabels();
};

#endif // FORMAUDITPREVISIONS_H
