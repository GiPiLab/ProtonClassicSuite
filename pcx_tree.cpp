#include "pcx_tree.h"
#include "utils.h"
#include "xlsxdocument.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QUuid>
#include <QElapsedTimer>
#include <QFileInfo>


PCx_Tree::PCx_Tree(unsigned int treeId):treeId(treeId)
{
    Q_ASSERT(treeId>0);
    QSqlQuery query;

    query.prepare("SELECT nom,termine,le_timestamp from index_arbres where id=:id");
    query.bindValue(":id",treeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        this->treeName=query.value("nom").toString();
        this->treeId=treeId;
        this->finished=query.value("termine").toBool();
        this->creationTime=query.value("le_timestamp").toString();
    }
    else
    {
        qCritical()<<"Invalid tree ID"<<treeId;
        die();
    }

    loadTypesFromDb();
}

unsigned int PCx_Tree::addNode(unsigned int pid, unsigned int typeId, const QString &name)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(pid>0);
    Q_ASSERT(typeId>0);

    if(getNumberOfNodes()>=MAXNODES)
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Nombre maximum de noeuds atteint !"));
        return 0;
    }

    QSqlQuery q;

    //NOTE : Check for duplicate nodes in tree
    if(nodeExists(name,typeId))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom dans l'arbre' !"));
        return 0;
    }

    q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":pid",pid);
    q.bindValue(":type",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();

    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    return q.lastInsertId().toUInt();
}


bool PCx_Tree::updateNode(unsigned int nodeId, const QString &newName, unsigned int newType)
{
    Q_ASSERT(!newName.isNull());
    Q_ASSERT(!newName.isEmpty());
    Q_ASSERT(newType>0);

    QSqlQuery q;

    if(nodeExists(newName,newType))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un noeud portant ce nom !"));
        return false;
    }

    if(!isTypeIdValid(newType))
    {
        qCritical()<<"Invalid type !";
        return false;
    }

    q.prepare(QString("update arbre_%1 set nom=:nom, type=:type where id=:id").arg(treeId));
    q.bindValue(":nom",newName);
    q.bindValue(":type",newType);
    q.bindValue(":id",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    return true;
}

QStringList PCx_Tree::getListOfCompleteNodeNames() const
{
    QStringList nodeNames;
    QSqlQuery q;
    if(!q.exec(QString("select nom,type from arbre_%1 where id>1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodeNames.append(QString("%1 %2").arg(idTypeToName(q.value("type").toUInt())).arg(q.value("nom").toString()));
    }
    return nodeNames;
}


QStringList PCx_Tree::getListOfNodeNames() const
{
    QStringList nodeNames;
    QSqlQuery q;
    if(!q.exec(QString("select distinct nom from arbre_%1 where id>1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodeNames.append(q.value("nom").toString());
    }
    return nodeNames;
}

QList<unsigned int> PCx_Tree::getLeavesId() const
{
    QList<unsigned int> leaves;
    QList<unsigned int> nodes;
    nodes=getNodesId();
    foreach (unsigned int node, nodes)
    {
        if(isLeaf(node))
        {
            leaves.append(node);
        }
    }
    //qDebug()<<"Leaves for tree "<<treeId<< " = "<<leaves;
    return leaves;
}


QList<unsigned int> PCx_Tree::getNonLeavesId() const
{
    QList<unsigned int> nonleaves;
    QList<unsigned int> nodes;
    nodes=getNodesId();
    foreach (unsigned int node, nodes)
    {
        if(!isLeaf(node))
        {
            nonleaves.append(node);
        }
    }
    //qDebug()<<"Non-leaves for tree "<<treeId<< " = "<<nonleaves;
    return nonleaves;
}

QSet<unsigned int> PCx_Tree::getNodesWithSharedName() const
{
    QSqlQuery q(QString("select * from arbre_%1").arg(treeId));
    QSet<unsigned int> nodes;
    QHash<QString,QPair<unsigned int,unsigned int> > nameToTypeAndId;
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        QString name=q.value("nom").toString().toLower();
        if(nameToTypeAndId.contains(name))
        {
            nodes.insert(q.value("id").toUInt());
            nodes.insert(nameToTypeAndId.value(name).second);
        }
        else
        {
            QPair<unsigned int,unsigned int>typeAndId;
            typeAndId.first=q.value("type").toUInt();
            typeAndId.second=q.value("id").toUInt();
            nameToTypeAndId.insert(name,typeAndId);
        }
    }
    return nodes;
}

bool PCx_Tree::isLeaf(unsigned int nodeId) const
{
    Q_ASSERT(nodeId>0);
    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where pid=:nodeid").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        if(q.value(0).toInt()==0)
            return true;
        else return false;
    }
    return false;
}

unsigned int PCx_Tree::getTreeDepth() const
{
    unsigned int maxDepth=0;
    QList<unsigned int> nodes=getNodesId();
    unsigned int depth,pid,curNode;
    foreach(unsigned int node, nodes)
    {
        if(node==1)
            continue;
        depth=0;

        curNode=node;
        do
        {
            pid=getParentId(curNode);
            depth++;
            curNode=pid;
        }while(pid!=1);

        if(depth>maxDepth)
            maxDepth=depth;
    }
    return maxDepth;
}



QList<unsigned int> PCx_Tree::getIdsOfNodesWithThisType(unsigned int typeId) const
{
    Q_ASSERT(typeId>0);
    QSqlQuery q;
    QList<unsigned int> nodes;
    q.prepare(QString("select id from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodes.append(q.value(0).toUInt());
    }
    return nodes;
}

unsigned int PCx_Tree::getNumberOfNodesWithThisType(unsigned int typeId) const
{
    Q_ASSERT(typeId>0);
    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
        return 0;
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_Tree::sortNodesBFS(QList<unsigned int> &nodes) const
{
    QList<unsigned int>sortedNodes;

    QList<unsigned int>toCheck;
    toCheck.append(1);

    while(!toCheck.isEmpty())
    {
        unsigned int node=toCheck.takeFirst();
        if(nodes.contains(node))
            sortedNodes.append(node);
        toCheck.append(getChildren(node));
    }
    return sortedNodes;
}


QList<unsigned int> PCx_Tree::sortNodesDFS(QList<unsigned int> &nodes,unsigned int currentNode) const
{
    QList<unsigned int> sortedNodes;

    if(nodes.contains(currentNode))
        sortedNodes.append(currentNode);

    QList<unsigned int> children=getChildren(currentNode);
    foreach(unsigned int childId,children)
    {
        sortedNodes.append(sortNodesDFS(nodes,childId));
    }
    return sortedNodes;
}

unsigned int PCx_Tree::getParentId(unsigned int nodeId) const
{
    QSqlQuery q;
    q.prepare(QString("select pid from arbre_%1 where id=:nodeid").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        return q.value(0).toUInt();
    }
    return 0;
}

QList<unsigned int> PCx_Tree::getChildren(unsigned int nodeId) const
{
    QList<unsigned int> listOfChildren;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:nodeid order by nom").arg(treeId));
    q.bindValue(":nodeid",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        listOfChildren.append(q.value(0).toUInt());
    }
    return listOfChildren;
}



void PCx_Tree::updateNodePid(unsigned int nodeId, unsigned int newPid)
{
    Q_ASSERT(nodeId>1 && newPid > 0);
    QSqlQuery q;

    q.prepare(QString("update arbre_%1 set pid=:pid where id=:id").arg(treeId));
    q.bindValue(":pid",newPid);
    q.bindValue(":id",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
}

bool PCx_Tree::finishTree()
{
    Q_ASSERT(treeId>0);
    if(this->finished==false)
    {
        QSqlQuery q;
        q.prepare("update index_arbres set termine=1 where id=:id");
        q.bindValue(":id",treeId);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }
        this->finished=true;
    }
    return true;
}





bool PCx_Tree::nodeExists(const QString &name, unsigned int typeId) const
{
    QSqlQuery q;

    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(":nom",name);
    q.bindValue(":type",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            return true;
        }
    }
    return false;
}

int PCx_Tree::getNodeIdFromTypeAndNodeName(const QPair<QString, QString> &typeAndNodeName) const
{
    int typeId=nameToIdType(typeAndNodeName.first);
    if(typeId==-1)
    {
        qWarning()<<"Unable to find the node identifier";
        return -1;
    }
    QSqlQuery q;
    QString nodeNameSpl=typeAndNodeName.second.simplified();
    q.prepare(QString("select id from arbre_%1 where nom=:nameId and type=:typeId").arg(treeId));
    q.bindValue(":nameId",nodeNameSpl);
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return -1;
    }
    if(!q.next())
    {
        qWarning()<<"Unable to find the node identifier";
        return -1;
    }
    return q.value(0).toInt();
}


QPair<QString,QString> PCx_Tree::getTypeNameAndNodeName(unsigned int node) const
{
    Q_ASSERT(node>0);
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(":id",node);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    QPair<QString,QString> typeNameAndNodeName;

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=idTypeToName(q.value("type").toUInt());
            typeNameAndNodeName.first=typeName;
            typeNameAndNodeName.second=q.value("nom").toString();
        }
        //Root does not has type
        else
        {
            typeNameAndNodeName.first=QString();
            typeNameAndNodeName.second=q.value("nom").toString();
        }
        return typeNameAndNodeName;
    }
    else
    {
        qWarning()<<"Missing node"<<node;
    }
    return QPair<QString,QString>();

}


QString PCx_Tree::getNodeName(unsigned int node) const
{
    Q_ASSERT(node>0);
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(":id",node);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=idTypeToName(q.value("type").toUInt());
            return QString("%1 %2").arg(typeName).arg(q.value("nom").toString());
        }
        //Root does not has type
        else
        {
            return q.value("nom").toString();
        }
    }
    else
    {
        qWarning()<<"Missing node"<<node;
    }
    return QString();
}


bool PCx_Tree::deleteNode(unsigned int nodeId)
{
    Q_ASSERT(nodeId>0);
    QList<unsigned int> listOfChildrens;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:pid").arg(treeId));
    q.bindValue(":pid",nodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        return false;
    }
    while(q.next())
    {
        listOfChildrens.append(q.value(0).toUInt());
    }

    if(listOfChildrens.size()==0)
    {
        q.prepare(QString("delete from arbre_%1 where id=:id").arg(treeId));
        q.bindValue(":id",nodeId);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }
        return true;
    }
    else
    {
        foreach(unsigned int child,listOfChildrens)
        {
            deleteNode(child);
        }
        deleteNode(nodeId);
    }
    return true;
}



QString PCx_Tree::toDot() const
{
    QList<unsigned int> nodes=getNodesId();

    QString out="graph g{\nrankdir=LR;\n";

    foreach(unsigned int node,nodes)
    {
        //Escape double quotes for dot format
        out.append(QString("%1 [label=\"%2\"];\n").arg(node).arg(getNodeName(node).replace('"',QString("\\\""))));
    }

    foreach(unsigned int node,nodes)
    {
        unsigned int pid=getParentId(node);

        if(pid!=0)
        {
            out.append(QString("\t%1--%2;\n").arg(pid).arg(node));
        }
    }
    out.append("}\n");
    return out;
}

bool PCx_Tree::toXLSX(const QString &fileName) const
{
    Q_ASSERT(!fileName.isEmpty());
    QList<unsigned int> nodes=getNodesId();

    QXlsx::Document xlsx;

    xlsx.write(1,1,"Type noeud");
    xlsx.write(1,2,"Nom noeud");
    xlsx.write(1,3,"Type père");
    xlsx.write(1,4,"Nom père");

    QPair<QString,QString>typeNameAndNodeName,pidTypeNameAndPidNodeName;

    int row=2;
    foreach(unsigned int node,nodes)
    {
        unsigned int pid=getParentId(node);
        typeNameAndNodeName=getTypeNameAndNodeName(node);

        if(pid>1)
        {
            xlsx.write(row,1,typeNameAndNodeName.first);
            xlsx.write(row,2,typeNameAndNodeName.second);
            pidTypeNameAndPidNodeName=getTypeNameAndNodeName(pid);
            xlsx.write(row,3,pidTypeNameAndPidNodeName.first);
            xlsx.write(row,4,pidTypeNameAndPidNodeName.second);
            row++;
        }
        else if(pid==1)
        {
            xlsx.write(row,1,typeNameAndNodeName.first);
            xlsx.write(row,2,typeNameAndNodeName.second);
            row++;
        }

    }
    return xlsx.saveAs(fileName);
}

void PCx_Tree::loadTypesFromDb()
{
    idTypesToNames.clear();
    listOfTypeNames.clear();
    QSqlQuery query;
    if(!query.exec(QString("select * from types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QString typeName=query.value(1).toString();
        idTypesToNames.insert(query.value(0).toUInt(),typeName);
        listOfTypeNames.append(typeName);
    }
}

int PCx_Tree::_internalAddTree(const QString &name,bool createRoot)
{
    QSqlQuery query;

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }

    QVariant lastId=query.lastInsertId();

    if(!lastId.isValid())
    {
        qCritical()<<"Invalid last inserted Id";
        die();
    }
    int treeId=lastId.toInt();

    if(!query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("create index idx_arbre_%1_pid on arbre_%1(pid)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(createRoot)
    {
        if(!query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(treeId)))
        {
            qCritical()<<query.lastError();
            die();
        }
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            die();
        }
    }
    return lastId.toInt();
}


bool PCx_Tree::validateType(const QString &newType)
{
    qDebug()<<"Type to validate = "<<newType;
    if(newType.isEmpty())
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Vous ne pouvez pas utiliser un type vide !"));
        return false;
    }
    else if(listOfTypeNames.contains(newType))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Le type <b>%1</b> existe déjà !").arg(newType));
        return false;
    }
    return true;
}

int PCx_Tree::nameToIdType(const QString &typeName) const
{
    Q_ASSERT(!typeName.isNull() && !typeName.isEmpty());
    QString typeNameSpl=typeName.simplified();
    if(listOfTypeNames.contains(typeNameSpl))
        return idTypesToNames.key(typeNameSpl);
    else return -1;
}

QList<QPair<unsigned int, QString> > PCx_Tree::getAllTypes() const
{
    QList<QPair<unsigned int,QString> > types;
    QSqlQuery query;
    if(!query.exec(QString("select * from types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QPair<unsigned int,QString> p;
        p.first=query.value(0).toUInt();
        p.second=query.value(1).toString();
        types.append(p);
    }
    return types;
}

unsigned int PCx_Tree::addType(const QString &typeName)
{
    unsigned int typeId=0;

    if(validateType(typeName)==false)
        return 0;

    else
    {
        QSqlQuery q;
        q.prepare(QString("insert into types_%1 (nom) values(:nom)").arg(treeId));
        q.bindValue(":nom",typeName);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }

        typeId=q.lastInsertId().toUInt();

        idTypesToNames.insert(typeId,typeName);
        listOfTypeNames.append(typeName);
    }
    return typeId;
}

bool PCx_Tree::deleteType(unsigned int typeId)
{
    Q_ASSERT(typeId>0);

    QSqlQuery query;

    query.prepare(QString("select count(*) from arbre_%1 where type=:type").arg(treeId));
    query.bindValue(":type",typeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        qDebug()<<"Number of nodes of type "<<typeId<<" = "<<query.value(0).toInt();
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe des noeuds de ce type dans l'arbre. Supprimez-les d'abord."));
            return false;
        }

    }
    else return false;

    query.prepare(QString("delete from types_%1 where id=:id").arg(treeId));
    query.bindValue(":id",typeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }
    loadTypesFromDb();
    return true;
}

unsigned int PCx_Tree::getNumberOfNodes() const
{
    QSqlQuery q;
    if(!q.exec(QString("select count(*) from arbre_%1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_Tree::getNodesId() const
{
    Q_ASSERT(treeId>0);
    QSqlQuery q;
    QList<unsigned int> nodeIds;
    if(!q.exec(QString("select id from arbre_%1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodeIds.append(q.value(0).toUInt());
    }
    return nodeIds;
}

int PCx_Tree::duplicateTree(const QString &newName)
{
    Q_ASSERT(!newName.isEmpty());
    QSqlQuery q;

    if(treeNameExists(newName))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
        return -1;
    }

    QSqlDatabase::database().transaction();
    int newTreeId=_internalAddTree(newName,false);
    Q_ASSERT(newTreeId>0);

    if(!q.exec(QString("insert into types_%1 select * from types_%2").arg(newTreeId).arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("insert into arbre_%1 select * from arbre_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return newTreeId;
}

QStringList PCx_Tree::getListOfDefaultTypes()
{
    QStringList listOfTypes;
    listOfTypes<<QObject::tr("Maire adjoint")<<QObject::tr("Conseiller")<<QObject::tr("Division")<<QObject::tr("Service");
    return listOfTypes;
}




int PCx_Tree::createRandomTree(const QString &name,unsigned int nbNodes)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty() && nbNodes>0 && nbNodes<=PCx_Tree::MAXNODES);
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

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

    int lastId=addTree(name);

    if(lastId<=0)
    {
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
            deleteTree(lastId);
            die();
        }
    }

    return lastId;
}


int PCx_Tree::addTree(const QString &name)
{
    QSqlQuery query;

    if(treeNameExists(name))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
        return -1;
    }

    QSqlDatabase::database().transaction();
    int treeId=_internalAddTree(name,true);
    Q_ASSERT(treeId>0);

    QStringList listOfTypes=getListOfDefaultTypes();
    foreach(QString oneType,listOfTypes)
    {
        query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(treeId));
        query.bindValue(":nomtype",oneType);
        if(!query.exec())
        {
            qCritical()<<query.lastError();
            die();
        }
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            die();
        }
    }
    QSqlDatabase::database().commit();
    return treeId;
}

QList<unsigned int> PCx_Tree::getListOfTreesId(bool finishedOnly)
{
    QList<unsigned int> listOfTrees;

    QSqlQuery query;

    if(!query.exec("select * from index_arbres order by datetime(le_timestamp)"))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        if(query.value("termine").toBool()==true)
        {
            listOfTrees.append(query.value("id").toUInt());
        }
        else if(finishedOnly==false)
        {
             listOfTrees.append(query.value("id").toUInt());
        }
    }
    return listOfTrees;
}

QList<QPair<unsigned int, QString> > PCx_Tree::getListOfTrees(bool finishedOnly)
{
    QList<QPair<unsigned int,QString> > listOfTrees;
    QDateTime dt;

    QSqlQuery query;

    if(!query.exec("select * from index_arbres order by datetime(le_timestamp)"))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QString item;

        dt=QDateTime::fromString(query.value(3).toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();

        if(query.value("termine").toBool()==true)
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


bool PCx_Tree::treeNameExists(const QString &name)
{
    Q_ASSERT(!name.isEmpty());
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            return true;
        }
    }
    return false;
}

int PCx_Tree::deleteTree(unsigned int treeId)
{
    Q_ASSERT(treeId>0);

    QSqlQuery query;

    if(!query.exec(QString("select count(*) from index_arbres where id='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        if(query.value(0).toInt()==0)
        {
            qWarning()<<"Trying to delete an inexistant tree !";
            return -1;
        }
    }

    if(!query.exec(QString("select count(*) from index_audits where id_arbre='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
           return 0;
        }
    }


    QSqlDatabase::database().transaction();
    if(!query.exec(QString("delete from index_arbres where id='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }


    if(!query.exec(QString("drop table arbre_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("drop table types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    QSqlDatabase::database().commit();

    //qDebug()<<"Tree "<<treeId<<" deleted.";
    return 1;
}



int PCx_Tree::importTreeFromXLSX(const QString &fileName, const QString &treeName)
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

    if(treeNameExists(treeName))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
        return -1;
    }

    QSqlDatabase::database().transaction();

    int treeId=_internalAddTree(treeName,true);
    Q_ASSERT(treeId>0);

    PCx_Tree tree(treeId);

    foreach(const QString & oneType,foundTypesList)
    {
        unsigned int oneTypeId=tree.addType(oneType);
        if(oneTypeId==0)
        {
            QMessageBox::warning(0,QObject::tr("Erreur"),QObject::tr("Type %1 invalide").arg(oneType));
            QSqlDatabase::database().rollback();
            return -1;
        }
    }

    foreach(aNode,firstLevelNodesSet)
    {
        tree.addNode(1,typesToIdTypes.value(aNode.first),aNode.second);
    }
    foreach(aNode,nodeToPid.keys())
    {
        QPair<QString, QString> aPid;
        aPid=nodeToPid.value(aNode);
        tree.addNode(nodeToIdNode.value(aPid),typesToIdTypes.value(aNode.first),aNode.second);
    }

    QSqlDatabase::database().commit();
    return treeId;
   }
