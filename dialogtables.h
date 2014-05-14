#ifndef DIALOGTABLES_H
#define DIALOGTABLES_H

#include <QWidget>
#include "pcx_auditmodel.h"

namespace Ui {
class DialogTables;
}

class DialogTables : public QWidget
{
    Q_OBJECT

public:
    explicit DialogTables(QWidget *parent = 0);
    ~DialogTables();

public slots:
    void onListOfAuditsChanged();


private slots:

    void on_comboListAudits_activated(int index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_radioButtonGlobal_toggled(bool checked);

    void on_checkBoxRecap_toggled(bool checked);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_checkBoxEvolution_toggled(bool checked);

    void on_checkBoxEvolutionCumul_toggled(bool checked);

    void on_checkBoxBase100_toggled(bool checked);

    void on_checkBoxJoursAct_toggled(bool checked);

private:
    Ui::DialogTables *ui;
    void updateListOfAudits();
    void updateTextBrowser();

    QTextDocument *doc;
    PCx_AuditModel *model;


};

#endif // DIALOGTABLES_H
