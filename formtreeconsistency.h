#ifndef FORMTREECONSISTENCY_H
#define FORMTREECONSISTENCY_H

#include <QWidget>
#include "pcx_tree.h"


namespace Ui {
class FormTreeConsistency;
}

class FormTreeConsistency : public QWidget
{
    Q_OBJECT

public:
    explicit FormTreeConsistency(unsigned int treeId, QWidget *parent = 0);
    ~FormTreeConsistency();

private:

    QSize sizeHint() const;

    Ui::FormTreeConsistency *ui;    
    void populateTableInfos();
    PCx_Tree *tree;
    void populateListOfNodesWithSameNameButDifferentTypes();
};

#endif // FORMTREECONSISTENCY_H
