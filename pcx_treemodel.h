#ifndef PCX_TREEMODEL_H
#define PCX_TREEMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <QSqlDatabase>


class PCx_TreeModel
{
public:
    PCx_TreeModel();
    virtual ~PCx_TreeModel();

    unsigned int getTreeId(){return treeId;}
    bool isFinished(){return finished;}
    QString & getName(){return treeName;}
    QDateTime getCreationTime();
    QStandardItemModel * getModel(){return model;}

    void setName(const QString & name){treeName=name;}
    void finish(){finished=true;}

    bool saveToDatabase(void);
    bool loadFromDatabase(unsigned int treeId);
    bool loadFromDatabase(const QString & treeName);


private:
    QStandardItemModel * model;
    QStandardItem * root;

    unsigned int treeId;
    bool finished;
    QString treeName;
    QString creationTime;
    bool createChildren(QStandardItem *item, unsigned int nodeId);

};

#endif // PCX_TREEMODEL_H
