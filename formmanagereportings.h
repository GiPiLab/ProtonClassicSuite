#ifndef FORMMANAGEREPORTINGS_H
#define FORMMANAGEREPORTINGS_H

#include <QWidget>

namespace Ui {
class FormManageReportings;
}

class FormManageReportings : public QWidget
{
    Q_OBJECT

public:
    explicit FormManageReportings(QWidget *parent = 0);
    ~FormManageReportings();

public slots:
    void onLOTchanged();

signals:
    void listOfReportingsChanged();

private slots:
    void on_pushButtonDisplayTree_clicked();

    void on_pushButtonAddReporting_clicked();

    void on_pushButtonDeleteReporting_clicked();

    void on_comboListOfReportings_activated(int index);

private:
    Ui::FormManageReportings *ui;
    void updateListOfTrees();
    void updateListOfReportings();
};

#endif // FORMMANAGEREPORTINGS_H
