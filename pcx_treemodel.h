#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include "pcx_typemodel.h"
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
    PCx_TreeModel(unsigned int treeId, QObject *parent=0);
    virtual ~PCx_TreeModel();

    int getTreeId(){return treeId;}
    bool isFinished(){return finished;}
    QString & getName(){return treeName;}
    QDateTime getCreationTime();
    PCx_TypeModel* getTypes(){return types;}

    void setName(const QString & name){treeName=name;}
    void finish(){finished=true;}

    int addNode(int pid,  int type, const QString &name, const QModelIndex &pidNodeIndex);
    bool updateNode(const QModelIndex &nodeIndex ,const QString &newName, unsigned int newType);

    bool deleteNode(const QModelIndex &nodeIndex);

    static bool addNewTree(const QString &name);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void updateNodePosition(int nodeId, int newPid);

    bool updateTree();

private:
    QStandardItem * root;
    PCx_TypeModel *types;

    int treeId;
    bool finished;
    QString treeName;
    QString creationTime;

    bool loadFromDatabase(unsigned int treeId);
    bool createChildrenItems(QStandardItem *item, unsigned int nodeId);
    bool deleteNodeAndChildren(unsigned int nodeId);
    QStandardItem *createItem(const QString &typeName,const QString &nodeName,int typeId,int nodeId);
};

#endif // PCX_TREEMODEL_H
