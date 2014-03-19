#ifndef TYPES_H
#define TYPES_H
#include <QHash>
#include <QString>
#include <QStringList>
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

    inline QString getNomType(int id){return idTypesToNom[id];}
    QStringList getNomTypes(){return nomTypes;}

    bool addType(const QString &type);
    bool deleteType(const QString &type);
    bool deleteType(int id);

    bool loadTypesFromTree(int treeId);
    int getTreeId(){return treeId;}
    bool isOk(){return initialized;}
    QSqlTableModel *getTableModel(){return typesTableModel;}

    bool loadSqlTableModel();

    static QStringList getListOfDefaultTypes();

private slots:
    void onTypesModelDataChanged();

private:

    QHash<int,QString> idTypesToNom;
    bool initialized;
    int treeId;
    QStringList nomTypes;
    QSqlTableModel *typesTableModel;

};

#endif // TYPES_H
