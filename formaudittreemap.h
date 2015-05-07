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

    void on_pushButtonPrevious_clicked();

    void on_pushButtonSave_clicked();

private:
    static const QString TOUT;
    void updateListOfAudits();
    void updateTreeMapWidget();

    QSize sizeHint()const;

    Ui::FormAuditTreemap *ui;
    PCx_Audit *selectedAudit;
    int selectedYear;
    int selectedNode;
    QStack<int> previousSelectedNodes;

    MODES::DFRFDIRI selectedMode;
    PCx_Audit::ORED selectedORED;
};

#endif // FORMAUDITTREEMAP_H
