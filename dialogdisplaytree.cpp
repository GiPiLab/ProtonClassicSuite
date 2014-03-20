#include "dialogdisplaytree.h"
#include "ui_dialogdisplaytree.h"
#include "utility.h"
#include "pcx_treemodel.h"
#include <QtSql>
#include <QtGui>
#include <QMessageBox>
#include <QInputDialog>

DialogDisplayTree::DialogDisplayTree(QWidget *parent) : QWidget(parent), ui(new Ui::DialogDisplayTree)
{
    ui->setupUi(this);
    model=NULL;
    updateListOfTree();
}

DialogDisplayTree::~DialogDisplayTree()
{
    if(model!=NULL)delete model;
    delete ui;
}


void DialogDisplayTree::on_remplirButton_clicked()
{
    if(model==NULL)
    {
        qDebug()<<"Aucun arbre sélectionné, rien à faire";
        return;
    }
    QSqlQuery query;

    //Premier niveau
     int numNodes=qrand()%20+1;
     for(int i=0;i<numNodes;i++)
     {
             QString nomNode=QString("Node_%1").arg(qrand());
             query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('%2',1,1)").arg(model->getTreeId()).arg(nomNode));

             if(query.numRowsAffected()!=1)
             {
                 qCritical()<<query.lastError().text();
                 die();
             }
     }

     //Deuxieme niveau
     int numNodes2nd=qrand()%40+3;
     for(int i=0;i<numNodes2nd;i++)
     {
             QString nomNode=QString("Node2nd_%1").arg(qrand());
             query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('%2',%3,2)").arg(model->getTreeId()).arg(nomNode).arg(2+qrand()%(numNodes-2)));

             if(query.numRowsAffected()!=1)
             {
                 qCritical()<<query.lastError().text();
                 die();
             }
     }

     model->updateTree();
     ui->treeView->expandToDepth(1);
}

void DialogDisplayTree::on_comboBox_currentIndexChanged(int index)
{
    if(index==-1)return;
    if(model!=NULL)
    {
        ui->listTypesView->setModel(NULL);
        delete model;
    }

    model=new PCx_TreeModel();
    model->loadFromDatabase(ui->comboBox->currentData().toInt());

    ui->treeView->setModel(model->getModel());
    ui->treeView->expandToDepth(1);

    ui->listTypesView->setModel(model->getTypes()->getTableModel());
    ui->listTypesView->setModelColumn(1);
    connect(model->getTypes()->getTableModel(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onTypesChanged()));
}

void DialogDisplayTree::on_addTypeButton_clicked()
{
    if(model!=NULL)
    {
        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Nouveau type"), tr("Nom du type à ajouter : "),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->getTypes()->addType(text);
        }
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
    ui->comboBox->setCurrentIndex(0);
}

void DialogDisplayTree::on_deleteTreeButton_clicked()
{
    if(model!=NULL)
    {

        if(QMessageBox::question(this,tr("Attention"),tr("Vous allez supprimer cet arbre. Cette action ne peut pas être annulée. En êtes-vous sûr ?"))==QMessageBox::No)
        {
            return;
        }

        qDebug()<<"Deleting tree "<<model->getTreeId();

        ui->listTypesView->setModel(NULL);
        model->getTypes()->getTableModel()->clear();
        model->getModel()->clear();

        QSqlQuery query;

        query.exec(QString("delete from index_arbres where id='%1'").arg(model->getTreeId()));
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError().text();
            die();
        }


        query.exec(QString("drop table arbre_%1").arg(model->getTreeId()));
        if(query.numRowsAffected()==-1)
        {
            qCritical()<<query.lastError().text();
            die();
        }

        query.exec(QString("drop table types_%1").arg(model->getTreeId()));
        if(query.numRowsAffected()==-1)
        {
            qCritical()<<query.lastError().text();
            die();
        }

        qDebug()<<"Tree "<<model->getTreeId()<<" deleted.";

        updateListOfTree();
        delete model;
        model=NULL;

        ui->comboBox->setCurrentIndex(-1);
    }
}

void DialogDisplayTree::onTypesChanged()
{
    qDebug()<<"Types changed !";
    model->updateTree();
    ui->treeView->expandToDepth(1);
}

void DialogDisplayTree::on_newTreeButton_clicked()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(this,tr("Nouvel arbre"), tr("Nom du nouvel arbre : "),QLineEdit::Normal,"",&ok);

    }while(ok && text.isEmpty());

    if(ok)
    {
        PCx_TreeModel::addNewTree(text);
        updateListOfTree();
    }
}

void DialogDisplayTree::on_deleteTypeButton_clicked()
{
    if(model!=NULL)
    {
        QModelIndex index=ui->listTypesView->currentIndex();
        qDebug()<<index;
        if(index.row()>-1)
        {
            model->getTypes()->deleteType(model->getTypes()->getTableModel()->record(index.row()).field("id").value().toInt());
            //Or with string :
            //model->getTypes()->deleteType(index.data().toString());
        }
    }
}

/*
 * Pense-bete pour accéder aux items et aux id
void DialogDisplayTree::on_treeView_clicked(const QModelIndex &index)
{
    qDebug()<<"Clicked : "<<index.data().toString()<<"Id = "<<index.data(Qt::UserRole+1).toInt()<< "Type = "<<index.data(Qt::UserRole+2).toInt();

    qDebug()<<"Selection model : "<<ui->treeView->selectionModel()->selectedIndexes()[0].data(Qt::UserRole+1).toInt();

    //Access id in listTypeView :
    QModelIndex indexType=ui->listTypesView->currentIndex();
    qDebug()<<"Type selected ID : "<<model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();
}*/

void DialogDisplayTree::on_addNodeButton_clicked()
{
    if(model==NULL)return;
    QModelIndexList selection=ui->treeView->selectionModel()->selectedIndexes();

    if(!selection.isEmpty())
    {
        int selectedId=selection[0].data(Qt::UserRole+1).toInt();
        qDebug()<<"Node Selected Id : "<<selectedId;

        QModelIndex indexType=ui->listTypesView->currentIndex();
        if(indexType.row()<0)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le type de l'élément à ajouter dans la zone de droite"));
            return;
        }

        int selectedTypeId=model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();

        qDebug()<<"Type selected ID : "<<selectedTypeId;

        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Nouveau noeud"), tr("Donnez un nom au nouveau noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->addNode(selectedId,selectedTypeId,text,selection[0]);
        }

    }
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez d'abord le noeud père dans l'arbre"));
        return;
    }
}

void DialogDisplayTree::on_modifyNodeButton_clicked()
{
    if(model==NULL)return;

    QModelIndexList selection=ui->treeView->selectionModel()->selectedIndexes();

    if(!selection.isEmpty())
    {
        int selectedId=selection[0].data(Qt::UserRole+1).toInt();
        qDebug()<<"Node Selected Id : "<<selectedId;

        //No modification for the root
        if(selectedId==1)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Vous ne pouvez pas modifier la racine de l'arbre"));
            return;
        }

        QModelIndex indexType=ui->listTypesView->currentIndex();
        if(indexType.row()<0)
        {
            QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le nouveau type du noeud à modifier dans la zone de droite"));
            return;
        }

        int selectedTypeId=model->getTypes()->getTableModel()->record(indexType.row()).field("id").value().toInt();

        bool ok;
        QString text;

        do
        {
            text=QInputDialog::getText(this,tr("Modifier noeud"), tr("Nouveau nom du noeud, son type sera <b>%1</b> : ").arg(indexType.data().toString()),QLineEdit::Normal,"",&ok);

        }while(ok && text.isEmpty());

        if(ok)
        {
            model->updateNode(selection[0],text,selectedTypeId);
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Attention"),tr("Sélectionnez le noeud à modifier"));
        return;
    }
}

void DialogDisplayTree::on_treeView_doubleClicked(const QModelIndex &index)
{
    on_modifyNodeButton_clicked();
}
