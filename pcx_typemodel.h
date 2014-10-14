#ifndef PCX_TYPEMODEL_H
#define PCX_TYPEMODEL_H

#include <QSqlTableModel>
#include <QSqlQueryModel>

class PCx_TypeModel:public QObject
{
    Q_OBJECT
public:
    explicit PCx_TypeModel(unsigned int treeId, bool readOnly=true, QObject *parent=0);
    ~PCx_TypeModel();

    QString idTypeToName(unsigned int id) const{return idToName[id];}
    bool isTypeIdValid(unsigned int id) const{return idToName.contains(id);}
    int nameToIdType(const QString &typeName) const;
    const QStringList &getTypeNames() const {return listOfTypeNames;}

    QList<QPair<unsigned int,QString> > getAllTypes() const;


    unsigned int addType(const QString &typeName);
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

    QHash<unsigned int,QString> idToName;
    unsigned int treeId;
    QStringList listOfTypeNames;
    QSqlTableModel *typesTableModel;
    QSqlQueryModel *typesQueryModel;
};

#endif // PCX_TYPEMODEL_H
