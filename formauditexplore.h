#ifndef FORMAUDITEXPLORE_H
#define FORMAUDITEXPLORE_H

#include <QWidget>
#include "pcx_auditwithtreemodel.h"
#include "pcx_report.h"

namespace Ui {
class FormAuditExplore;
}

class FormAuditExplore : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditExplore(QWidget *parent = 0);
    ~FormAuditExplore();

public slots:
    void onListOfAuditsChanged();
    void onSettingsChanged();

private slots:

    void on_comboListAudits_activated(int index);
    void on_treeView_clicked(const QModelIndex &index);
    void on_radioButtonGlobal_toggled(bool checked);
    void on_checkBoxPoidsRelatif_toggled(bool checked);
    void on_radioButtonDF_toggled(bool checked);
    void on_radioButtonRF_toggled(bool checked);
    void on_radioButtonDI_toggled(bool checked);
    void on_radioButtonRI_toggled(bool checked);
    void on_checkBoxEvolution_toggled(bool checked);
    void on_checkBoxEvolutionCumul_toggled(bool checked);
    void on_checkBoxBase100_toggled(bool checked);
    void on_checkBoxJoursAct_toggled(bool checked);
    //void on_printButton_clicked();
    void on_saveButton_clicked();
    void on_checkBoxResults_toggled(bool checked);
    void on_checkBoxRecapGraph_toggled(bool checked);
    void on_checkBoxOuvert_toggled(bool checked);
    void on_checkBoxEngage_toggled(bool checked);
    void on_checkBoxOuvertCumul_toggled(bool checked);
    void on_checkBoxEngageCumul_toggled(bool checked);
    void on_checkBoxRealise_toggled(bool checked);
    void on_checkBoxDisponible_toggled(bool checked);
    void on_checkBoxRealiseCumul_toggled(bool checked);
    void on_checkBoxDisponibleCumul_toggled(bool checked);

    void on_pushButtonCollapseAll_clicked();

    void on_pushButtonExpandAll_clicked();



    void on_checkBoxRawHistoryData_toggled(bool checked);

private:
    Ui::FormAuditExplore *ui;
    void updateListOfAudits();
    void updateTextBrowser();
    void getSelections();

    QTextDocument *doc;
    PCx_AuditWithTreeModel *model;
    PCx_Report *report;

    QList<PCx_Tables::PCAPRESETS> selectedTabs;
    QList<PCx_Graphics::PCAGRAPHICS> selectedGraphics;
    MODES::DFRFDIRI selectedMode;

    bool ready;
};

#endif // FORMAUDITEXPLORE_H
