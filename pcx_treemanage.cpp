#include "pcx_treemanage.h"
#include "pcx_tree.h"
#include "utils.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QUuid>
#include "xlsxdocument.h"


namespace PCx_TreeManage{


QStringList getListOfDefaultTypes()
{
    QStringList listOfTypes;
    listOfTypes<<QObject::tr("Maire adjoint")<<QObject::tr("Conseiller")<<QObject::tr("Division")<<QObject::tr("Service");
    return listOfTypes;
}




int createRandomTree(const QString &name,unsigned int nbNodes)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && nbNodes>0 && nbNodes<=PCx_Tree::MAXNODES);
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    query.exec();

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
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

    if(lastId<=0)
    {
        QSqlDatabase::database().rollback();
        die();
    }

    unsigned int maxNumType=getListOfDefaultTypes().size();

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


/**
 * @brief addTree add a tree. This function must be enclosed in a sql transaction
 * @param name : the tree name
 * @param createRoot : create the root ?
 * @param addTypes : add default types ?
 * @return the ID of the new tree
 */
int addTree(const QString &name, bool createRoot, bool addTypes)
{
    QSqlQuery query;

    if(treeNameExists(name))
    {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
            return -1;
    }

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    query.exec();

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        return -1;
    }

    QVariant lastId=query.lastInsertId();

    if(!lastId.isValid())
    {
        qCritical()<<"Invalid last inserted Id";
        return -1;
    }
    unsigned int treeId=lastId.toUInt();

    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(treeId));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        return -1;
    }


    query.exec(QString("create index idx_arbre_%1_pid on arbre_%1(pid)").arg(treeId));

    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(treeId));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        return -1;
    }

    if(addTypes==true)
    {
        QStringList listOfTypes=getListOfDefaultTypes();
        foreach(QString oneType,listOfTypes)
        {
            query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(treeId));
            query.bindValue(":nomtype",oneType);
            query.exec();
            if(query.numRowsAffected()!=1)
            {
                qCritical()<<query.lastError();
                return -1;
            }
        }
    }

    if(createRoot)
    {
    //Racine
        query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(treeId));
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            return -1;
        }
    }
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



int importTreeFromXLSX(const QString &fileName, const QString &treeName)
{
    Q_ASSERT(!fileName.isEmpty() && !treeName.isEmpty());
    QFileInfo fi(fileName);
    if(!fi.isReadable()||!fi.isFile())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier invalide ou non lisible"));
        return -1;
    }

    QXlsx::Document xlsx(fileName);

    if(!xlsx.read(1,1).isValid()||
            !xlsx.read(1,2).isValid()||
            !xlsx.read(1,3).isValid()||
            !xlsx.read(1,4).isValid())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide"));
        return -1;
    }


    int i=2;

    QSet<QString> foundTypes;
    QMap<QPair<QString,QString>,QPair<QString,QString> > nodeToPid;
    QList<QPair<QString,QString> > firstLevelNodes;
    QSet<QPair<QString,QString> > firstLevelNodesSet;
    bool duplicateFound=false;

    int rowCount=xlsx.dimension().rowCount();

    do
    {
        QVariant i1,i2,i3,i4;
        i1=xlsx.read(i,1);
        i2=xlsx.read(i,2);
        i3=xlsx.read(i,3);
        i4=xlsx.read(i,4);

        if(!(i1.isValid() && i2.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. Le fichier doit contenir des données sur au moins les deux premières colonnes").arg(i));
            return -1;
        }

        if((!i3.isValid() && i4.isValid()) || (i3.isValid() && !i4.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. Si les colonnes 3 et 4 sont renseignées, elles doivent l'être simultanément").arg(i));
            return -1;
        }

        QPair<QString,QString> node1;

        node1.first=i1.toString().simplified();
        node1.second=i2.toString().simplified();

        if(node1.first.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type du noeud manquant ou invalide ligne %1 colonne 1 !").arg(i));
            return -1;
        }

        if(node1.second.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Nom du noeud manquant ou invalide ligne %1 colonne 2 !").arg(i));
            return -1;
        }

        foundTypes.insert(node1.first);

        if(i3.isValid()&&i4.isValid())
        {
            QPair<QString,QString> pidNode;
            pidNode.first=i3.toString().simplified();
            pidNode.second=i4.toString().simplified();
            if(!pidNode.first.isEmpty() && !pidNode.second.isEmpty())
            {
                //NOTE : Check for duplicate nodes in tree
                if(nodeToPid.contains(node1))
                {
                    if(nodeToPid.value(node1)!=pidNode)
                    {
                        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 ne peut pas avoir plusieurs pères !")
                                              .arg(QString(node1.first+" "+node1.second)));
                        return -1;
                    }
                    else
                    {
                        duplicateFound=true;
                    }
                }
                else
                {
                    nodeToPid.insert(node1,pidNode);
                }
                foundTypes.insert(pidNode.first);
            }
            else
            {
                if(pidNode.first.isEmpty())
                {
                    QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type du noeud père manquant ligne %1 colonne 3 !").arg(i));
                    return -1;
                }

                if(pidNode.second.isEmpty())
                {
                    QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Nom du noeud père manquant ligne %1 colonne 4 !").arg(i));
                    return -1;
                }
            }
        }
        else
        {
            firstLevelNodes.append(node1);
        }
        i++;
        if(i>(int)PCx_Tree::MAXNODES+1)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Trop de noeuds dans l'arbre (%1) !").arg(PCx_Tree::MAXNODES));
            return -1;
        }        

    }while(i<=rowCount);

    firstLevelNodesSet=firstLevelNodes.toSet();

    if(firstLevelNodesSet.size()<firstLevelNodes.size()||duplicateFound)
    {
        QMessageBox::information(0,QObject::tr("Information"),QObject::tr("Il y a des doublons dans le fichier, ils ne seront pas pris en compte."));
    }


    if(firstLevelNodesSet.size()==0)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Aucun noeud n'est accroché à la racine. Laissez les colonnes 3 et 4 vides pour les spécifier."));
        return -1;
    }


    QPair<QString,QString> firstLevelNode;
    foreach(firstLevelNode,firstLevelNodesSet)
    {
        if(nodeToPid.contains(firstLevelNode))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 ne peut pas avoir plusieurs pères !")
                                  .arg(QString(firstLevelNode.first+" "+firstLevelNode.second)));
            return -1;
        }
    }

    QHash<QString,unsigned int> typesToIdTypes;

    i=1;
    QStringList foundTypesList=foundTypes.toList();
    foundTypesList.sort();
    foreach(const QString &oneType, foundTypesList)
    {
        typesToIdTypes.insert(oneType,i);
        i++;
    }

    //Check for orphan ancestors
    QPair<QString, QString> ancestor;

    foreach(ancestor,nodeToPid.values())
    {
        if(!nodeToPid.contains(ancestor) && !firstLevelNodesSet.contains(ancestor))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 est orphelin !")
                                  .arg(QString(ancestor.first+" "+ancestor.second)));
            return -1;
        }
    }



    QPair<QString,QString> aNode;
    QMap<QPair<QString,QString>, unsigned int> nodeToIdNode;
    unsigned int k=2;
    foreach(aNode,firstLevelNodesSet)
    {
        nodeToIdNode.insert(aNode,k);
        k++;
    }

    foreach(aNode,nodeToPid.keys())
    {
        nodeToIdNode.insert(aNode,k);
        k++;
    }

    QSqlDatabase::database().transaction();
    int treeId=addTree(treeName,true,false);
    if(treeId<=0)
    {
        QSqlDatabase::database().rollback();
        return -1;
    }

    PCx_Tree tree(treeId);

    foreach(const QString & oneType,foundTypesList)
    {
        unsigned int oneTypeId=tree.addType(oneType);
        if(oneTypeId==0)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type %1 invalide").arg(oneType));
            QSqlDatabase::database().rollback();
            return -1;
        }
    }

    foreach(aNode,firstLevelNodesSet)
    {
        if(tree.addNode(1,typesToIdTypes.value(aNode.first),aNode.second)==0)
        {
            QSqlDatabase::database().rollback();
            return -1;
        }
    }
    foreach(aNode,nodeToPid.keys())
    {
        QPair<QString, QString> aPid;
        aPid=nodeToPid.value(aNode);
        if(tree.addNode(nodeToIdNode.value(aPid),typesToIdTypes.value(aNode.first),aNode.second)==0)
        {
            QSqlDatabase::database().rollback();
            return -1;
        }
    }

    QSqlDatabase::database().commit();
    return treeId;
   }
}

