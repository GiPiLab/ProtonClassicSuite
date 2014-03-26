#ifndef PCX_TYPEMODEL_H
#define PCX_TYPEMODEL_H
#include <QtGui>
#include <QtSql>

class PCx_TypeModel:public QObject
{
    Q_OBJECT
public:
    PCx_TypeModel(unsigned int treeId, QObject *parent=0);
    ~PCx_TypeModel();

    inline QString getNomType(int id){return idTypesToNom[id];}
    QStringList getNomTypes(){return nomTypes;}

    bool addType(const QString &type);
    bool deleteType(const QString &type);
    bool deleteType(int id);

    int getTreeId(){return treeId;}
    QSqlTableModel *getTableModel(){return typesTableModel;}
    QSqlQueryModel *getQueryModel(){return typesQueryModel;}

    static QStringList getListOfDefaultTypes();

signals:
    void typesUpdated(void);


private slots:
    bool onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:

    bool validateType(const QString &newType);
    bool loadFromDatabase(unsigned int treeId);
    bool loadSqlTableModel();
    void loadSqlQueryModel();

    QHash<int,QString> idTypesToNom;
    unsigned int treeId;
    QStringList nomTypes;
    QSqlTableModel *typesTableModel;
    QSqlQueryModel *typesQueryModel;

};

#endif // PCX_TYPEMODEL_H
