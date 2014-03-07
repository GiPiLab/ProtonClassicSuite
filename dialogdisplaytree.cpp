#include "dialogdisplaytree.h"
#include "ui_dialogdisplaytree.h"
#include "utility.h"
#include "pcx_treemodel.h"
#include <QTreeWidgetItem>
#include <QSql>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QComboBox>
#include <QtSql/QSqlError>
#include <QDebug>


DialogDisplayTree::DialogDisplayTree(QWidget *parent) : QWidget(parent), ui(new Ui::DialogDisplayTree)
{
    ui->setupUi(this);
    model=NULL;
    updateListOfTree();
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(on_treeView_clicked(QModelIndex)));
}

DialogDisplayTree::~DialogDisplayTree()
{
    if(model!=NULL)delete model;
    delete ui;
}


void DialogDisplayTree::on_remplirButton_clicked()
{

    QSqlQuery query;
    query.exec(QString("insert into index_arbres (nom) values ('arbre_%1')").arg(qrand()));


    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    QVariant lastId=query.lastInsertId();
    if(!lastId.isValid())
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<"Problème d'id";
        die();
    }

    qDebug()<<"Last inserted id = "<<lastId.toInt();

    //Arbre
    query.exec(QString("create table arbre_%1(id integer primary key autoincrement, nom text not null, pid integer not null, type integer not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    //Types associés
    query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text not null)").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        //Penser à supprimer les tables d'avant en cas d'échec pour éviter les orphelines
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    query.exec(QString("insert into types_%1 (nom) values ('Entité')").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }
    query.exec(QString("insert into types_%1 (nom) values ('Maire')").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    query.exec(QString("insert into types_%1 (nom) values ('1er adjoint')").arg(lastId.toInt()));

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

    //Racine
    query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,0)").arg(lastId.toInt()));
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
        die();
    }

   //Premier niveau
    int numNodes=qrand()%20+1;
    for(int i=0;i<numNodes;i++)
    {
            QString nomNode=QString("Node_%1").arg(qrand());
            query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('%2',1,1)").arg(lastId.toInt()).arg(nomNode));

            if(query.numRowsAffected()==-1)
            {
                qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
                die();
            }
    }

    //Deuxieme niveau
    int numNodes2nd=qrand()%40+1;
    for(int i=0;i<numNodes2nd;i++)
    {
            QString nomNode=QString("Node2nd_%1").arg(qrand());
            query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('%2',%3,2)").arg(lastId.toInt()).arg(nomNode).arg(2+qrand()%(numNodes-2)));

            if(query.numRowsAffected()==-1)
            {
                qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
                die();
            }
    }
    updateListOfTree();
}


void DialogDisplayTree::on_treeView_clicked(const QModelIndex &index)
{
    QStandardItem *item=model->getModel()->itemFromIndex(index);
    ui->label->setText(item->text());
}

void DialogDisplayTree::on_comboBox_currentIndexChanged(int index)
{
    if(index==-1)return;
    if(model!=NULL)delete model;

    model=new PCx_TreeModel();
    model->loadFromDatabase(ui->comboBox->currentData().toInt());

    ui->treeView->setModel(model->getModel());
    ui->treeView->expandToDepth(0);

    ui->tableView->setModel(model->getTypesModel());
    ui->tableView->hideColumn(0);
}

void DialogDisplayTree::on_addTypeButton_clicked()
{
    if(model!=NULL)
    {
        model->getTypesModel()->insertRow(model->getTypesModel()->rowCount());
    }
}

void DialogDisplayTree::updateListOfTree()
{
    ui->comboBox->clear();

    QSqlQuery query("select * from index_arbres");
    while(query.next())
    {
        QString item(query.value(1).toString()+" - "+query.value(2).toString()+" - "+query.value(3).toString());
        ui->comboBox->insertItem(0,item,query.value(0).toInt());
    }
}

void DialogDisplayTree::on_deleteTreeButton_clicked()
{
    if(model!=NULL)
    {
        qDebug()<<"Deleting tree "<<model->getTreeId();
        QSqlQuery query;
        query.exec(QString("drop table arbre_%1").arg(model->getTreeId()));
        if(query.numRowsAffected()==-1)
        {
            qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
            die();
        }

        query.exec(QString("delete from index_arbres where id='%1'").arg(model->getTreeId()));
        if(query.numRowsAffected()==-1)
        {
            qCritical()<<Q_FUNC_INFO<<__LINE__<<query.lastError().text();
            die();
        }
        qDebug()<<"Tree "<<model->getTreeId()<<" deleted.";
    }
}
