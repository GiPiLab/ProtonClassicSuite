#ifndef FORMMANAGEAUDITS_H
#define FORMMANAGEAUDITS_H

#include<QWidget>
#include "pcx_audit.h"

namespace Ui {
class FormManageAudits;
}

class FormManageAudits : public QWidget
{
    Q_OBJECT

public:
    explicit FormManageAudits(QWidget *parent = 0);
    ~FormManageAudits();

public slots:
    void onLOTchanged();
    void onListOfAuditsChanged();
    void updateRandomButtonVisibility();



private slots:
    void on_addAuditButton_clicked();

    void on_comboListOfAudits_activated(int index);

    void on_deleteAuditButton_clicked();

    void on_finishAuditButton_clicked();

    void on_cloneAuditButton_clicked();

    void on_pushButtonDisplayTree_clicked();

    void on_statisticsAuditButton_clicked();

    void on_pushButtonRandomDF_clicked();

    void on_pushButtonRandomRF_clicked();

    void on_pushButtonRandomDI_clicked();

    void on_pushButtonRandomRI_clicked();

    void on_pushButtonClearDF_clicked();

    void on_pushButtonClearRF_clicked();

    void on_pushButtonClearDI_clicked();

    void on_pushButtonClearRI_clicked();

    void on_pushButtonLoadDF_clicked();

    void on_pushButtonLoadRF_clicked();

    void on_pushButtonLoadDI_clicked();

    void on_pushButtonLoadRI_clicked();

    void on_pushButtonExportDF_clicked();

    void on_pushButtonExportRF_clicked();

    void on_pushButtonExportDI_clicked();

    void on_pushButtonExportRI_clicked();

    void on_pushButtonSkel_clicked();

signals:
    void listOfAuditsChanged();
    void auditDataUpdated(unsigned int auditId);

private:
    Ui::FormManageAudits *ui;
    void updateListOfTrees();
    void updateListOfAudits();
    PCx_Audit *selectedAudit;
    bool exportLeaves(MODES::DFRFDIRI mode);
    bool importLeaves(MODES::DFRFDIRI mode);
    void updateButtonsVisibility();
};

#endif // FORMMANAGEAUDITS_H
