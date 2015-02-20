#ifndef FORMDISPLAYPREVISIONREPORT_H
#define FORMDISPLAYPREVISIONREPORT_H

#include <QWidget>
#include <QTextDocument>
#include "pcx_previsionitem.h"

namespace Ui {
class FormDisplayPrevisionReport;
}

class FormDisplayPrevisionReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormDisplayPrevisionReport(PCx_PrevisionItem *previsionItem, QWidget *parent = 0);
    ~FormDisplayPrevisionReport();


private slots:
    void on_pushButtonSaveHTML_clicked();

private:
    Ui::FormDisplayPrevisionReport *ui;
    PCx_PrevisionItem *previsionItem;
    QTextDocument *doc;
    QSize sizeHint() const;
};

#endif // FORMDISPLAYPREVISIONREPORT_H
