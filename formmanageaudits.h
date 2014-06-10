#ifndef FORMMANAGEAUDITS_H
#define FORMMANAGEAUDITS_H

#include <QMdiArea>

namespace Ui {
class FormManageAudits;
}

class FormManageAudits : public QWidget
{
    Q_OBJECT

public:
    explicit FormManageAudits(QWidget *parent = 0, QMdiArea *mdiarea=0);
    ~FormManageAudits();

public slots:
    void onLOTchanged();


private slots:
    void on_addAuditButton_clicked();

    void on_comboListOfAudits_activated(int index);

    void on_deleteAuditButton_clicked();

    void on_finishAuditButton_clicked();

signals:
    void listOfAuditsChanged();

private:
    Ui::FormManageAudits *ui;
    QMdiArea *mdiarea;
    void updateListOfTrees();
    void updateListOfAudits();
};

#endif // FORMMANAGEAUDITS_H
