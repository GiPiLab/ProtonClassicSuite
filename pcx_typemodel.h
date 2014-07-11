#ifndef PCX_TYPEMODEL_H
#define PCX_TYPEMODEL_H

#include <QSqlTableModel>
#include <QSqlQueryModel>

//TODO : Refactor that class


class PCx_TypeModel:public QObject
{
    Q_OBJECT
public:
    explicit PCx_TypeModel(unsigned int treeId, bool readOnly=true, QObject *parent=0);
    ~PCx_TypeModel();

    QString getNomType(unsigned int id) const{return idTypesToNom[id];}
    const QStringList &getNomTypes() const {return nomTypes;}
    QList<QPair<unsigned int,QString> > getTypes() const;


    bool addType(const QString &type);
    bool deleteType(const QString &type);
    bool deleteType(unsigned int id);

    unsigned int getTreeId() const{return treeId;}
    QSqlTableModel *getTableModel(){return typesTableModel;}
    QSqlQueryModel *getSqlQueryModel(){return typesQueryModel;}

    static QStringList getListOfDefaultTypes();

    static bool createTableTypes(unsigned int treeId,bool populateWithDefaultTypes=true);


signals:
    void typesUpdated(void);


private slots:
    bool onTypesModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:

    bool validateType(const QString &newType);
    bool loadFromDatabase(unsigned int treeId);
    bool loadSqlTableModel();
    bool loadSqlQueryModel();

    QHash<unsigned int,QString> idTypesToNom;
    unsigned int treeId;
    QStringList nomTypes;
    QSqlTableModel *typesTableModel;
    QSqlQueryModel *typesQueryModel;
};

#endif // PCX_TYPEMODEL_H
