#ifndef DIALOGEDITAUDIT_H
#define DIALOGEDITAUDIT_H

#include <QDialog>

namespace Ui {
class DialogEditAudit;
}

class DialogEditAudit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditAudit(QWidget *parent = 0);
    ~DialogEditAudit();

private:
    Ui::DialogEditAudit *ui;
    void updateListOfAudits();

};

#endif // DIALOGEDITAUDIT_H
