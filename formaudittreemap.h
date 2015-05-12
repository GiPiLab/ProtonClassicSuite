#ifndef FORMAUDITTREEMAP_H
#define FORMAUDITTREEMAP_H

#include <QWidget>
#include <QStack>
#include "pcx_audit.h"

namespace Ui {
class FormAuditTreemap;
}

class FormAuditTreemap : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuditTreemap(QWidget *parent = 0);
    ~FormAuditTreemap();

public slots:
    void onListOfAuditsChanged();

private slots:
    void on_comboListAudits_activated(int index);

    void on_comboBoxMode_activated(int index);

    void on_comboBoxORED_activated(int index);

    void on_comboBoxYear_activated(int index);

    void on_treeMapWidget_clicked(const QString &name, int id, int year, double value);

    void on_treeMapWidget_highlighted(const QString &name, int id, int year, double value);

    void on_pushButtonGoToPid_clicked();

    void on_pushButtonSave_clicked();

    void on_radioButtonGroupByMode_toggled(bool checked);

    void on_radioButtonGroupByORED_toggled(bool checked);

    void on_radioButtonGroupByYear_toggled(bool checked);

    void on_radioButtonGroupByNode_toggled(bool checked);

private:

    void updateListOfAudits();
    void updateTreeMapWidget();

    QSize sizeHint()const;

    Ui::FormAuditTreemap *ui;
    PCx_Audit *selectedAudit;
    int selectedYear;
    int selectedNode;

    MODES::DFRFDIRI selectedMode;
    PCx_Audit::ORED selectedORED;
};

#endif // FORMAUDITTREEMAP_H
