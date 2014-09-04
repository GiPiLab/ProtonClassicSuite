#include "pcx_treemanage.h"
#include "pcx_treemodel.h"
#include "utils.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QUuid>



namespace PCx_TreeManage{

int duplicateTree(unsigned int treeId, const QString &newName)
{
    Q_ASSERT(!newName.isEmpty());
    QSqlQuery q;
    q.prepare("select count(*) from index_arbres where nom=:name");
    q.bindValue(":name",newName);
    q.exec();

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<q.lastError();
        die();
    }

    QSqlDatabase::database().transaction();
    int newTreeId=addTree(newName,false,false);
    if(newTreeId==-1)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    q.exec(QString("insert into types_%1 select * from types_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("insert into arbre_%1 select * from arbre_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return newTreeId;
}



int createRandomTree(const QString &name,unsigned int nbNodes)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && nbNodes>0 && nbNodes<=PCx_TreeModel::MAXNODES);
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    query.exec();

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }
    QSqlDatabase::database().transaction();
    int lastId=addTree(name);

    if(lastId==-1)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    unsigned int maxNumType=PCx_TypeModel::getListOfDefaultTypes().size();

    for(unsigned int i=1;i<nbNodes;i++)
    {
        QSqlQuery q;
        unsigned int type=(qrand()%maxNumType)+1;
        unsigned int pid=(qrand()%i)+1;
        QString name=QUuid::createUuid().toString();
        q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(lastId));
        q.bindValue(":nom",name);
        q.bindValue(":pid",pid);
        q.bindValue(":type",type);
        q.exec();

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            QSqlDatabase::database().rollback();
            die();
        }
    }

    QSqlDatabase::database().commit();
    return lastId;
}


int addTree(const QString &name, bool createRoot, bool addTypes)
{
    QSqlQuery query;

    if(treeNameExists(name))
    {
            QMessageBox::warning(NULL,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
    }

    QSqlDatabase::database().transaction();

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        QSqlDatabase::database().rollback();
        return -1;
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<"Invalid last inserted Id";
        QSqlDatabase::database().rollback();
        return -1;
    }

    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toUInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        QSqlDatabase::database().rollback();
        return -1;
    }

    query.exec(QString("create index idx_arbre_%1_pid on arbre_%1(pid)").arg(lastId.toUInt()));

    //When used to duplicate a tree, createRoot=false then create an empty type table
    if(!PCx_TypeModel::createTableTypes(lastId.toUInt(),addTypes))
    {
        QSqlDatabase::database().rollback();
        return -1;
    }

    if(createRoot)
    {
    //Racine
        query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toUInt()));
        if(query.numRowsAffected()!=1)
        {
            QSqlDatabase::database().rollback();
            qCritical()<<query.lastError();
            return -1;
        }
    }
    QSqlDatabase::database().commit();
    return lastId.toInt();
}


QList<QPair<unsigned int, QString> > getListOfTrees(bool finishedOnly)
{
    QList<QPair<unsigned int,QString> > listOfTrees;
    QDateTime dt;

    QSqlQuery query("select * from index_arbres order by datetime(le_timestamp)");
    while(query.next())
    {
        QString item;

        dt=QDateTime::fromString(query.value(3).toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();

        if(query.value(2).toBool()==true)
        {
            item=QString("%1 - %2 (arbre terminé)").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
            QPair<unsigned int, QString> p;
            p.first=query.value(0).toUInt();
            p.second=item;
            listOfTrees.append(p);
        }
        else if(finishedOnly==false)
        {
             item=QString("%1 - %2").arg(query.value(1).toString()).arg(dtLocal.toString(Qt::SystemLocaleShortDate));
             QPair<unsigned int, QString> p;
             p.first=query.value(0).toUInt();
             p.second=item;
             listOfTrees.append(p);
        }
    }
    return listOfTrees;
}


bool treeNameExists(const QString &name)
{
    Q_ASSERT(!name.isEmpty());
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    query.exec();

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            return true;
        }
    }
    return false;
}

QString idTreeToName(unsigned int treeId)
{
    Q_ASSERT(treeId>0);
    QSqlQuery q(QString("select * from index_arbres where id='%1'").arg(treeId));
    if(q.next())
    {
        return q.value("nom").toString();
    }
    else
    {
        qWarning()<<"Inexistant tree !";
        return NULL;
    }
}

//Returns 0 if the tree is linked to an audit, -1 for a non-existant tree (should not happens) and 1 on success
int deleteTree(unsigned int treeId)
{
    Q_ASSERT(treeId>0);

    QSqlQuery query;

    query.exec(QString("select count(*) from index_arbres where id='%1'").arg(treeId));
    if(query.next())
    {
        if(query.value(0).toInt()==0)
        {
            qWarning()<<"Trying to delete an inexistant tree !";
            return -1;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }

    query.exec(QString("select count(*) from index_audits where id_arbre='%1'").arg(treeId));
    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
           return 0;
        }
    }
    else
    {
        qCritical()<<query.lastError();
        die();
    }


    QSqlDatabase::database().transaction();
    query.exec(QString("delete from index_arbres where id='%1'").arg(treeId));
    if(query.numRowsAffected()!=1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }


    query.exec(QString("drop table arbre_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }

    query.exec(QString("drop table types_%1").arg(treeId));
    if(query.numRowsAffected()==-1)
    {
        QSqlDatabase::database().rollback();
        qCritical()<<query.lastError();
        die();
    }

    QSqlDatabase::database().commit();

    qDebug()<<"Tree "<<treeId<<" deleted.";
    return 1;
}

int importTreeFromTSV(const QString &fileName, const QString &treeName)
{
    Q_ASSERT(!fileName.isEmpty() && !treeName.isEmpty());
    QFileInfo fi(fileName);
    if(!fi.isReadable()||!fi.isFile())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier invalide ou non lisible"));
        return -1;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Erreur d'ouverture du fichier : %1").arg(file.errorString()));
        return -1;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    QString line;

    line=stream.readLine();
    if(line.isEmpty())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide"));
        file.close();
        return -1;
    }

    line=stream.readLine();

    int i=2;

    QSet<QString> foundTypes,foundTypesAndNamesLeft,foundTypesAndNamesRight;

    QHash<QString,QString> typeAndNodeToPidTypeAndNode;
    QSet<QString> firstLevelTypesAndNodes;


    do
    {
        QStringList items=line.split("\t",QString::SkipEmptyParts);
        if(items.size()!=4 && items.size()!=2)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. Le fichier doit être sur deux ou quatre colonnes").arg(i));
            file.close();
            return -1;
        }

        QString type1=items.at(0).simplified();
        QString name1=items.at(1).simplified();

        if(type1.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type manquant ligne %1 colonne 1 !").arg(i));
            file.close();
            return -1;
        }

        if(name1.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Nom manquant ligne %1 colonne 2 !").arg(i));
            file.close();
            return -1;
        }

        foundTypes.insert(type1);
        foundTypesAndNamesLeft.insert(type1+"\t"+name1);

        if(items.size()==4)
        {
            QString type2=items.at(2).simplified();
            QString name2=items.at(3).simplified();
            if(!type2.isEmpty() && !name2.isEmpty())
            {
                foundTypesAndNamesRight.insert(type2+"\t"+name2);
                foundTypes.insert(type2);
            }
        }
        else
        {
            firstLevelTypesAndNodes.insert(type1+"\t"+name1);
        }
        i++;
        line=stream.readLine();

    }while(!line.isEmpty());
    file.close();

    Q_ASSERT(foundTypes.size()>0 && foundTypesAndNamesLeft.size()>0);

    if(firstLevelTypesAndNodes.size()==0)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Aucun noeud n'est accroché à la racine. Laissez les colonnes 3 et 4 vides pour les spécifier."));
        return -1;
    }

    QSqlDatabase::database().transaction();
    int treeId=addTree(treeName,true,false);
    if(treeId<=0)
    {
        QSqlDatabase::database().rollback();
        return -1;
    }

    QHash<QString,unsigned int> typesToIdTypes;

    PCx_TreeModel treeModel(treeId,false);

    foreach(QString oneType,foundTypes)
    {
        unsigned int oneTypeId=treeModel.getTypes()->addType(oneType);
        if(oneTypeId==0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type %1 invalide").arg(oneType));
            QSqlDatabase::database().rollback();
            return -1;
        }
        typesToIdTypes.insert(oneType,oneTypeId);
    }

    qDebug()<<foundTypes;
    qDebug()<<foundTypesAndNamesLeft<<foundTypesAndNamesRight;
    qDebug()<<firstLevelTypesAndNodes;
    qDebug()<<typesToIdTypes;

    QSqlDatabase::database().rollback();
    return 0;
}
}

