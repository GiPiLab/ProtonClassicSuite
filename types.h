#ifndef TYPES_H
#define TYPES_H
#include <QHash>
#include <QString>
#include <QDebug>
#include <QSqlTableModel>
#include <QObject>

class Types:public QObject
{
    Q_OBJECT
public:
    Types(QObject *parent=0);
    Types(int treeId, bool loadSqlTableModel);
    ~Types();

    inline QString getNomType(int id){qDebug()<<QString("type %1 = %2").arg(id).arg(idTypesToNom[id]);return idTypesToNom[id];}

    bool loadTypesFromTree(int treeId);
    int getTreeId(){return treeId;}
    bool isOk(){return initialized;}
    QSqlTableModel *getTableModel(){return typesTableModel;}

    bool loadSqlTableModel();

private slots:
    void onTypesModelDataChanged();

private:

    QHash<int,QString> idTypesToNom;
    bool initialized;
    int treeId;
    QSqlTableModel *typesTableModel;

};

#endif // TYPES_H
