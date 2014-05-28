#ifndef DIALOGTABLESGRAPHICS_H
#define DIALOGTABLESGRAPHICS_H

#include <QWidget>
#include "pcx_auditmodel.h"

namespace Ui {
class DialogTablesGraphics;
}

class DialogTablesGraphics : public QWidget
{
    Q_OBJECT

public:
    explicit DialogTablesGraphics(QWidget *parent = 0);
    ~DialogTablesGraphics();

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
    Ui::DialogTablesGraphics *ui;
    void updateListOfAudits();
    void updateTextBrowser();

    QByteArray plotToBase64ByteArray(QCustomPlot *plot, int width, int height) const;

    QTextDocument *doc;
    PCx_AuditModel *model;
    bool ready;
    };

#endif // DIALOGTABLESGRAPHICS_H
