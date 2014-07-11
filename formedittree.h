#ifndef FORMEDITTREE_H
#define FORMEDITTREE_H

#include <QMdiArea>
#include "formdisplaytree.h"
#include "pcx_treemodel.h"

namespace Ui {
class FormEditTree;
}

class FormEditTree : public QWidget
{
    Q_OBJECT

public:
    explicit FormEditTree(QWidget *parent = 0, QMdiArea *mdiArea = 0);
    ~FormEditTree();

private slots:

    void on_addTypeButton_clicked();

    void on_deleteTreeButton_clicked();

    void onTypesChanged();

    void on_newTreeButton_clicked();

    void on_deleteTypeButton_clicked();

    void on_addNodeButton_clicked();

    void on_modifyNodeButton_clicked();

    void on_deleteNodeButton_clicked();

    void on_finishTreeButton_clicked();

    void on_viewTreeButton_clicked();

    void on_comboBox_activated(int index);

    void on_treeView_activated(const QModelIndex &index);

    void on_duplicateTreeButton_clicked();

signals:
    void listOfTreeChanged();

private:
    Ui::FormEditTree *ui;
    PCx_TreeModel *model;
    QMdiArea *mdiArea;

    void updateListOfTree();
    //Disable editing
    void setReadOnly(bool state);
};

#endif // FORMEDITTREE_H
