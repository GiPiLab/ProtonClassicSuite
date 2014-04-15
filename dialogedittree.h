#ifndef DIALOGEDITTREE_H
#define DIALOGEDITTREE_H

#include <QDialog>
#include <QList>
#include <QMdiArea>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQueryModel>

#include "dialogdisplaytree.h"
#include "pcx_treemodel.h"

namespace Ui {
class DialogEditTree;
}

class DialogEditTree : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditTree(QWidget *parent = 0, QMdiArea *mdiArea = 0);
    ~DialogEditTree();

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

signals:
    void listOfTreeChanged();

private:
    Ui::DialogEditTree *ui;
    PCx_TreeModel *model;
    QMdiArea *mdiArea;
    //QList<DialogDisplayTree *> displayTrees;

    void updateListOfTree();
    //Disable editing
    void setReadOnly(bool state);
};

#endif // DIALOGEDITTREE_H
