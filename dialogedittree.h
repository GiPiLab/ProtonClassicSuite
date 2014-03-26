#ifndef DIALOGEDITTREE_H
#define DIALOGEDITTREE_H

#include <QDialog>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQueryModel>

#include "pcx_treemodel.h"

namespace Ui {
class DialogEditTree;
}

class DialogEditTree : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditTree(QWidget *parent = 0);
    ~DialogEditTree();

private slots:

    void on_comboBox_currentIndexChanged(int index);

    void on_addTypeButton_clicked();

    void on_deleteTreeButton_clicked();

    void onTypesChanged();

    void on_newTreeButton_clicked();

    void on_deleteTypeButton_clicked();

    void on_addNodeButton_clicked();

    void on_modifyNodeButton_clicked();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_deleteNodeButton_clicked();



private:
    Ui::DialogEditTree *ui;
    PCx_TreeModel *model;
    void updateListOfTree();
};

#endif // DIALOGEDITTREE_H
