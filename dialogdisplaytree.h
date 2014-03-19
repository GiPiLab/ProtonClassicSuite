#ifndef DIALOGDISPLAYTREE_H
#define DIALOGDISPLAYTREE_H

#include <QWidget>
#include <QtSql/QSqlTableModel>
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

    void on_comboBox_currentIndexChanged(int index);

    void on_addTypeButton_clicked();

    void on_deleteTreeButton_clicked();

    void onTypesChanged();

    void on_newTreeButton_clicked();

    void on_deleteTypeButton_clicked();

    //void on_treeView_clicked(const QModelIndex &index);

     void on_addNodeButton_clicked();

private:
    Ui::DialogDisplayTree *ui;
    PCx_TreeModel *model;
    void updateListOfTree();
};

#endif // DIALOGDISPLAYTREE_H
