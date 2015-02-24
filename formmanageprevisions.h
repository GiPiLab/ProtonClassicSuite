#ifndef FORMMANAGEPREVISIONS_H
#define FORMMANAGEPREVISIONS_H

#include <QWidget>
#include "pcx_prevision.h"

namespace Ui {
class FormManagePrevisions;
}

class FormManagePrevisions : public QWidget
{
    Q_OBJECT

public slots:
    void onListOfAuditsChanged();

signals:
    void listOfPrevisionsChanged();
    void listOfAuditsChanged();


public:
    explicit FormManagePrevisions(QWidget *parent = 0);
    ~FormManagePrevisions();

private slots:
    void on_comboListPrevisions_activated(int index);

    void on_pushButtonAddPrevision_clicked();
    void on_pushButtonDelete_clicked();

    void on_pushButtonToAudit_clicked();

private:
    Ui::FormManagePrevisions *ui;
    void updateListOfPrevisions();
    void updateListOfAudits();
    PCx_Prevision *selectedPrevision;
};

#endif // FORMMANAGEPREVISIONS_H
