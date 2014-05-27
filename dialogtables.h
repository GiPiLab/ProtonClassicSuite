#ifndef DIALOGTABLES_H
#define DIALOGTABLES_H

#include <QWidget>
#include "pcx_auditmodel.h"
#include "QCustomPlot/qcpdocumentobject.h"

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

    void on_checkBoxPoidsRelatif_toggled(bool checked);

    void on_radioButtonDF_toggled(bool checked);

    void on_radioButtonRF_toggled(bool checked);

    void on_radioButtonDI_toggled(bool checked);

    void on_radioButtonRI_toggled(bool checked);

    void on_checkBoxEvolution_toggled(bool checked);

    void on_checkBoxEvolutionCumul_toggled(bool checked);

    void on_checkBoxBase100_toggled(bool checked);

    void on_checkBoxJoursAct_toggled(bool checked);

    void on_printButton_clicked();

    void on_saveButton_clicked();

    void on_checkBoxResults_toggled(bool checked);

    void on_checkBoxRecapGraph_toggled(bool checked);

    void on_checkBoxPrevu_toggled(bool checked);

    void on_checkBoxEngage_toggled(bool checked);

    void on_checkBoxPrevuCumul_toggled(bool checked);

    void on_checkBoxEngageCumul_toggled(bool checked);

    void on_checkBoxRealise_toggled(bool checked);

    void on_checkBoxDisponible_toggled(bool checked);

    void on_checkBoxRealiseCumul_toggled(bool checked);

    void on_checkBoxDisponibleCumul_toggled(bool checked);

private:
    Ui::DialogTables *ui;
    void updateListOfAudits();
    void updateTextBrowser();
    QString insertPlotPixmapInDocResourceCache(const QCustomPlot *plot,int width,int height) const;

    QTextDocument *doc;
    PCx_AuditModel *model;
    QCPDocumentObject *interface;
    bool ready;
    QStringList imageNames;


};

#endif // DIALOGTABLES_H
