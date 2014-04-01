#ifndef DIALOGDISPLAYTREE_H
#define DIALOGDISPLAYTREE_H

#include <QDialog>
#include "pcx_treemodel.h"

namespace Ui {
class DialogDisplayTree;
}

class DialogDisplayTree : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDisplayTree(PCx_TreeModel *treeModel, QWidget *parent = 0);
    ~DialogDisplayTree();

private:
    Ui::DialogDisplayTree *ui;
    PCx_TreeModel *model;
};

#endif // DIALOGDISPLAYTREE_H
