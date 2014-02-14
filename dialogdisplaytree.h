#ifndef DIALOGDISPLAYTREE_H
#define DIALOGDISPLAYTREE_H

#include <QWidget>
#include "pcx_treemodel.h"

namespace Ui {
class DialogDisplayTree;
}

class DialogDisplayTree : public QWidget
{
    Q_OBJECT

public:
    explicit DialogDisplayTree(QWidget *parent = 0);
    ~DialogDisplayTree();

private slots:

    void on_remplirButton_clicked();

    void on_chargerButton_clicked();

    void on_treeView_clicked(const QModelIndex &index);

    void on_comboBox_currentIndexChanged(int index);


private:
    Ui::DialogDisplayTree *ui;
    PCx_TreeModel *model;

};

#endif // DIALOGDISPLAYTREE_H
