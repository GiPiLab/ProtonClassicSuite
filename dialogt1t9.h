#ifndef DIALOGT1T9_H
#define DIALOGT1T9_H

#include <QWidget>

namespace Ui {
class dialogT1T9;
}

class dialogT1T9 : public QWidget
{
    Q_OBJECT

public:
    explicit dialogT1T9(QWidget *parent = 0);
    ~dialogT1T9();

private:
    Ui::dialogT1T9 *ui;
};

#endif // DIALOGT1T9_H
