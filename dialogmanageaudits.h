#ifndef DIALOGMANAGEAUDITS_H
#define DIALOGMANAGEAUDITS_H

#include <QDialog>
#include <QMdiArea>

namespace Ui {
class DialogManageAudits;
}

class DialogManageAudits : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManageAudits(QWidget *parent = 0, QMdiArea *mdiarea=0);
    ~DialogManageAudits();

public slots:
    void onLOTchanged();


private slots:
    void on_addAuditButton_clicked();

private:
    Ui::DialogManageAudits *ui;
    QMdiArea *mdiarea;
    void updateListOfTrees();
    void updateListOfAudits();
};

#endif // DIALOGMANAGEAUDITS_H
