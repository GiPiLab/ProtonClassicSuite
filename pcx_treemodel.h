#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include "types.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QtGlobal>
#include <QSqlTableModel>
#include <QMimeData>


class PCx_TreeModel:public QStandardItemModel
{

public:
    PCx_TreeModel(QObject *parent=0);
    virtual ~PCx_TreeModel();

    int getTreeId(){return treeId;}
    bool isFinished(){return finished;}
    QString & getName(){return treeName;}
    QDateTime getCreationTime();
    Types * getTypes(){return types;}

    void setName(const QString & name){treeName=name;}
    void finish(){finished=true;}

    int addNode(int pid,  int type, const QString &name, QModelIndex &pidNodeIndex);
    bool updateNode(const QModelIndex &nodeIndex ,const QString &newName, int newType);

    bool deleteNode(const QModelIndex &nodeIndex);

    static bool addNewTree(const QString &name);
    bool loadFromDatabase(int treeId);
    bool loadFromDatabase(const QString & treeName);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void updateNodePosition(int nodeId, int newPid);

    bool updateTree();

private:
    QStandardItem * root;
    Types *types;

    int treeId;
    bool finished;
    QString treeName;
    QString creationTime;

    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName,const QString &nodeName,int typeId,int nodeId);
};

#endif // PCX_TREEMODEL_H
