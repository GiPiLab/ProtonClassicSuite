#ifndef DIALOGADDCHANGETREENODE_H
#define DIALOGADDCHANGETREENODE_H

#include <QDialog>

namespace Ui {
class DialogAddChangeTreeNode;
}

class DialogAddChangeTreeNode : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddChangeTreeNode(QWidget *parent = 0);
    ~DialogAddChangeTreeNode();

private:
    Ui::DialogAddChangeTreeNode *ui;
};

#endif // DIALOGADDCHANGETREENODE_H
