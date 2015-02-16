#ifndef FORMDISPLAYPREVISIONREPORT_H
#define FORMDISPLAYPREVISIONREPORT_H

#include <QWidget>
#include "pcx_prevision.h"

namespace Ui {
class FormDisplayPrevisionReport;
}

class FormDisplayPrevisionReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormDisplayPrevisionReport(QWidget *parent = 0);
    ~FormDisplayPrevisionReport();

private:
    Ui::FormDisplayPrevisionReport *ui;
};

#endif // FORMDISPLAYPREVISIONREPORT_H
