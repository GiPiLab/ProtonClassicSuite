#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include "types.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <QSqlTableModel>


class PCx_TreeModel
{
public:
    PCx_TreeModel();
    virtual ~PCx_TreeModel();

    int getTreeId(){return treeId;}
    bool isFinished(){return finished;}
    QString & getName(){return treeName;}
    QDateTime getCreationTime();
    QStandardItemModel * getModel(){return model;}
    Types * getTypes(){return types;}

    void setName(const QString & name){treeName=name;}
    void finish(){finished=true;}

    int addChild(int pid,int type,const QString &name);

    static bool addNewTree(const QString &name);
    bool loadFromDatabase(int treeId);
    bool loadFromDatabase(const QString & treeName);

    bool updateTree();

private:
    QStandardItemModel * model;
    QStandardItem * root;
    Types *types;

    int treeId;
    bool finished;
    QString treeName;
    QString creationTime;
    bool createChildren(QStandardItem *item, unsigned int nodeId);

};

#endif // PCX_TREEMODEL_H
